/*
  *    File: QN8027RadioApi.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <Wire.h>

#include "language.h"
#include "QN8027RadioApi.hpp"
#include "RfPower.hpp"
#include "memdebug.h"

// I2C:
static const uint8_t    I2C_QN8027_ADDR = 0x2c;     // I2C Address of QN8027 FM Radio Chip.
static const uint8_t    I2C_DEV_CNT     = 1;        // Number of expected i2c devices on bus.
static const uint32_t I2C_FREQ_HZ       = 100000;   // I2C master clock frequency

#define TAKE_SEMAPHORE(Sem, Skip)   if (!Skip) {xSemaphoreTakeRecursive (Sem, portMAX_DELAY);}
#define GIVE_SEMAPHORE(Sem, Skip)   if (!Skip) {xSemaphoreGiveRecursive (Sem);}

// *********************************************************************************************
void cQN8027RadioApi::begin ()
{
    // DEBUG_START;

    // Initialize i2c.
    Wire.begin (SDA_PIN, SCL_PIN);
    Wire.setClock (I2C_FREQ_HZ);        // 100KHz i2c speed.
    pinMode (SCL_PIN, INPUT_PULLUP);    // I2C Clock Pin.

    if (!checkRadioIsPresent ())
    {
        TestStatus = QN8027RadioFmTestStatus_e::FM_TEST_FAIL;
        Log.errorln (String (F ("-> QN8027 is Missing")).c_str ());
    }
    else
    {
        Log.verboseln (String (F ("-> QN8027 is Present")).c_str ());
        RadioSemaphore = xSemaphoreCreateRecursiveMutex ();

        // DEBUG_V(String("RadioSemaphore: 0x") + String(uint32_t(RadioSemaphore), HEX));
        if (NULL == RadioSemaphore)
        {
            Log.errorln (String (F ("Could not allocate a semaphore for access to the radio hardware")).c_str ());
        }
        else
        {
            // DEBUG_V(String("fmRadioTestCode: 0x") + String(fmRadioTestCode, HEX))
            initRadioChip ();   // If QN8027 fails we will warn user on UI homeTab.
            Log.infoln (String (F ("FM Radio RDS/RBDS Started.")).c_str ());
        }
    }

    // DEBUG_END;
}

// *********************************************************************************************
// calibrateAntenna(): Calibrate the QN8027 Antenna Interface. On exit, true if Calibration OK.
// This is an undocumented feature that was found during PixelRadio project development.
// This routine is called once during startup(). Takes about 2 Secs to execute.
bool cQN8027RadioApi::calibrateAntenna (bool SkipSemaphore)
{
    // DEBUG_START;

    bool Response = true;
    uint8_t regVal1;
    uint8_t regVal2;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        setFrequency (108.0F, true);  // High end of FM tuning range.
        waitForIdle (50, true);
        setRfCarrier (OFF, true);
        waitForIdle (15, true);
        setRfCarrier (ON, true);
        waitForIdle (50, true);
        FmRadio.reCalibrate ();
        waitForIdle (120, true);
        regVal1 = FmRadio.read1Byte (ANT_REG);

        setFrequency (85.0F, true);  // High end of FM tuning range.
        waitForIdle (50, true);
        setRfCarrier (OFF, true);
        waitForIdle (15, true);
        setRfCarrier (ON, true);
        waitForIdle (50, true);
        FmRadio.reCalibrate ();
        waitForIdle (120, true);
        regVal2 = FmRadio.read1Byte (ANT_REG);

        Log.verboseln (String (F ("-> QN8027 RF Port Test: Low RF Range= 0x%02X, High RF Range= 0x%02X")).c_str (), regVal1, regVal2);

        if ((regVal1 == 0x00) && (regVal2 == 0x00))
        {
            Response = false;
            Log.errorln ("-> QN8027 RF Port Calibration Failed, Possible Oscillator Failure.");
        }
        else if ((regVal1 <= 0x01) || (regVal1 >= 0x1f) || (regVal2 <= 0x01) || (regVal2 >= 0x1f))
        {
            Response = false;
            Log.errorln ("-> QN8027 RF Port has Poor Calibration, RF Tuning Range Impaired.");
        }
        else
        {
            Response = true;
            Log.infoln ("-> QN8027 RF Port Matching OK, Calibration Successful.");
        }

        /*
          *    radio.setFrequency(103.0F); // Recalibrate at middle FM range.
          *    waitForIdle(50, true);
          *    radio.Switch(OFF);
          *    waitForIdle(15, true);
          *    radio.Switch(ON);
          *    waitForIdle(50, true);
          */

        FmRadio.reCalibrate ();
        waitForIdle (120, true);

        setRfCarrierOFF (true);
        waitForIdle (50, true);

        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
//
// checkRadioIsPresent(): Check to see if QN8027 FM Radio Chip is installed. Return true if Ok.
bool cQN8027RadioApi::checkRadioIsPresent (bool SkipSemaphore)
{
    // DEBUG_START;

    bool response = false;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);

        Wire.beginTransmission (QN8027_I2C_ADDR);

        if (Wire.endTransmission (true) == 0)
        {
            // Receive 0 = success (ACK response)
            response = true;
        }

        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
    return response;
}

// *********************************************************************************************
// initRadioChip(); Initialize the QN8027 FM Radio Chip. Returns Test Result Code.
void cQN8027RadioApi::initRadioChip (bool SkipSemaphore)
{
    // DEBUG_START;

    uint8_t regVal;

    Log.infoln (String (F ("Initializing QN8027 FM Radio Chip ...")).c_str ());

    do  // once
    {
        regVal = FmRadio.read1Byte (CID1_REG);

        if ((regVal == 0x00) || ((regVal & 0x0C) != 0x00))
        {
            Log.errorln (String (F ("-> Incorrect CID1 Chip Family ID: 0x%02X")).c_str (), regVal);
            TestStatus = QN8027RadioFmTestStatus_e::FM_TEST_MISSING;
            break;
        }

        Log.verboseln (String (F ("-> CID1 Chip Family ID: 0x%02X")).c_str (), regVal);

        regVal = FmRadio.read1Byte (CID2_REG);

        if ((regVal & 0xF0) != 0x40)
        {
            Log.errorln (String (F ("-> Incorrect CID2 Version: 0x%02X")).c_str (), regVal);
            TestStatus = QN8027RadioFmTestStatus_e::FM_TEST_MISSING;
            break;
        }

        Log.verboseln (String (F ("-> CID2 Chip Version: 0x%02X")).c_str (), regVal);

        FmRadio.reset ();
        delay (30);

        FmRadio.setClockSource (0x00);          // XTAL on pins 1 & 2.
        FmRadio.setCrystalFreq (12);
        FmRadio.setCrystalCurrent (30);         // 30% of 400uA Max = 120uA.
        FmRadio.setTxFreqDeviation (0x81);      // 75Khz, Total Broadcast channel Bandwidth
        FmRadio.setTxPilotFreqDeviation (9);    // Use default 9% (6.75KHz) Pilot Tone Deviation.
        FmRadio.setTxPower (RfPower.get32 ());
        waitForIdle (25, true);

        for (uint8_t i = 0;i < RADIO_CAL_RETRY;i++)
        {
            // Allow several attempts to get good port matching results.
            if (calibrateAntenna ())
            {
                // QN8027 RF Port Matching OK, exit.
                TestStatus = QN8027RadioFmTestStatus_e::FM_TEST_OK;
                break;
            }

            TestStatus = QN8027RadioFmTestStatus_e::FM_TEST_VSWR;

            if (i < RADIO_CAL_RETRY - 1)
            {
                Log.infoln (String (F ("-> Retesting QN8027 RF Port Matching, Retry #%d")).c_str (), i + 1);
            }
        }

        waitForIdle (50, true);

        FmRadio.scrambleAudio (OFF);
        delay (5);

        FmRadio.clearAudioPeak ();
        delay (5);

        FmRadio.mute (true);
        delay (5);

        FmRadio.setRDSFreqDeviation (10);   // RDS Freq Deviation = 0.35KHz * Value.
        delay (1);
        FmRadio.RDS (ON);
        waitForIdle (20, true);

        FmRadio.updateSYSTEM_REG ();    // This is needed to Start FM Broadcast.
        waitForIdle (20, true);
        FmRadio.clearAudioPeak ();
        delay (1);

        Log.infoln (String (F ("-> Radio Status: %02X")).c_str (), (FmRadio.getStatus () & 0x07));
    } while (false);

    if (QN8027RadioFmTestStatus_e::FM_TEST_OK == TestStatus)
    {
        Log.infoln (String (F ("-> QN8027 Initialization Complete.")).c_str ());
    }
    else
    {
        Log.errorln (String (F ("-> QN8027 Failed Initialization.")).c_str ());
    }

    /*
      *     // DEBUG ONLY.
      *     for (int i = 0; i <= 0x1F; i++) {
      *         Log.infoln(String(F( "Radio Register %02X is: %02X")).c_str(), i, radio.read1Byte(i));
      *     }
      */

    // DEBUG_END;
}

// *********************************************************************************************
// measureAudioLevel(): Measure the Peak Audio Level. Max 675mV.
uint16_t cQN8027RadioApi::GetPeakAudioLevel (bool SkipSemaphore)
{
    // DEBUG_START;

    uint16_t mV = 0;


    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);

        waitForIdle (100, true);
        mV = FmRadio.getStatus () >> 4;
        FmRadio.clearAudioPeak ();

        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
    return mV * 45;  // Audio Peak is 45mV per count.
}

// *********************************************************************************************
// setAudioImpedance(): Set the Audio Input Impedance on the QN8027 chip.
void cQN8027RadioApi::setAudioImpedance (uint8_t value, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        waitForIdle (100, true);
        FmRadio.setAudioInpImp (value);
        waitForIdle (100, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// setAudioMute(): Control the Audio Mute on the QN8027 chip.
void cQN8027RadioApi::setAudioMute (bool value, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        waitForIdle (100, true);
        FmRadio.mute (value ? ON : OFF);
        waitForIdle (100, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// setDigitalGain(): Set the Tx Digital Gain on the QN8027 chip.
void cQN8027RadioApi::setDigitalGain (uint8_t value, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        waitForIdle (100, true);
        FmRadio.setTxDigitalGain (value);
        waitForIdle (100, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// setFrequency(): Set the Radio Frequency on the QN8027.
void cQN8027RadioApi::setFrequency (float frequency, bool Carrier, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        setRfCarrierOFF (true);
        FmRadio.setFrequency (frequency);
        setRfCarrier (Carrier, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// setMonoAudio(): Control the Audio Stereo/Mono mode on the QN8027 chip.
void cQN8027RadioApi::setMonoAudio (bool value, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        waitForIdle (100, true);
        FmRadio.MonoAudio (value ? OFF : ON);
        waitForIdle (100, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cQN8027RadioApi::setPiCode (uint16_t value, bool Carrier, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        setRfCarrierOFF (true);
        FmRadio.setPiCode (value);
        setRfCarrier (Carrier, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// ssetPreEmphasis(): Set the QN8027 chip's pre-Emphasis.
// ON = 50uS (Eur/UK/Australia), OFF = 75uS (North America/Japan).
void cQN8027RadioApi::setPreEmphasis (uint8_t value, bool Carrier, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        setRfCarrierOFF (true);
        FmRadio.setPreEmphTime50 (value);
        setRfCarrier (Carrier, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cQN8027RadioApi::setProgramServiceName (const String & value, bool Carrier, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        setRfCarrierOFF (true);
        FmRadio.sendStationName (value);
        setRfCarrier (Carrier, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cQN8027RadioApi::setPtyCode (uint8_t value, bool Carrier, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        setRfCarrierOFF (true);
        FmRadio.setPtyCode (value);
        setRfCarrier (Carrier, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cQN8027RadioApi::setRdsMessage (String & value, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        waitForIdle (100, true);
        FmRadio.sendRadioText (value);
        waitForIdle (100, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// setRfAutoOff():
// IMPORTANT: Sending RDS Messages and/or using updateUiAudioLevel() Will prevent 60S Turn Off.
// rfAutoOff = true: Turn-off RF Carrier if Audio is missing for > 60 seconds.
// rfAutoOff = false: Never turn off.
void cQN8027RadioApi::setRfAutoOff (bool value, bool Carrier, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        setRfCarrierOFF (true);
        FmRadio.radioNoAudioAutoOFF (value ? ON : OFF);
        setRfCarrier (Carrier, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// setRfCarrier(): Set the QN8027 chip's RF Carrier Output (On/Off).
void cQN8027RadioApi::setRfCarrierOFF (bool SkipSemaphore)
{
    // DEBUG_START;

    setRfCarrier (false, SkipSemaphore);

    // DEBUG_END;
}

// *********************************************************************************************
// setRfCarrier(): Set the QN8027 chip's RF Carrier Output (On/Off).
void cQN8027RadioApi::setRfCarrier (bool value, bool SkipSemaphore)
{
    // DEBUG_START;
    // DEBUG_V(String("value: ") + String(value));

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        waitForIdle (100, true);
        FmRadio.Switch (value ? ON : OFF);  // Update QN8027 Carrier.
        waitForIdle (100, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// setRfPower(): Set the QN8027 chip's RF Power Output. Max 121dBuVp.
// Note: Power is not changed until RF Carrier is toggled Off then On.
void cQN8027RadioApi::setRfPower (uint8_t value, bool Carrier, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        waitForIdle (100, true);
        FmRadio.setTxPower (value);
        waitForIdle (100, true);

        if (Carrier)
        {
            setRfCarrierOFF (true);
            setRfCarrier (Carrier, true);
        }

        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// setVgaGain(): Set the Tx Input Buffer Gain (Analog Gain) on the QN8027 chip.
void cQN8027RadioApi::setVgaGain (uint8_t value, bool SkipSemaphore)
{
    // DEBUG_START;

    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
        waitForIdle (100, true);
        FmRadio.setTxInputBufferGain (value);
        waitForIdle (100, true);
        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// waitForIdle(): Wait for QN8027 to enter Idle State. This means register command has executed.
void cQN8027RadioApi::waitForIdle (uint16_t waitMs, bool SkipSemaphore)
{
    // DEBUG_START;
    if (RadioSemaphore)
    {
        TAKE_SEMAPHORE (RadioSemaphore, SkipSemaphore);

        uint8_t stateCode1;
        uint8_t stateCode2;

        do  // once
        {
            stateCode1 = FmRadio.getStatus () & 0x07;
            // DEBUG_V(String("stateCode1: 0x") + String(stateCode1, HEX));

            // make sure the delay is a minimum of 5ms
            if (5 > waitMs)
            {
                waitMs = 5;
            }

            do
            {
                waitMs -= 5;

                delay (5);

                stateCode2 = FmRadio.getStatus () & 0x07;
                // DEBUG_V(String("stateCode2: 0x") + String(stateCode2, HEX));

                if ((stateCode2 == 0x02) || (stateCode2 == 0x05))
                {
                    // we are idle
                    break;
                }
                else if (stateCode1 != stateCode2)
                {
                    FmRadio.getStatus ();   // Clear next status byte.
                    break;
                }
            } while (5 <= waitMs);
        } while (false);

        // DEBUG_V(String("waitForIdle End Status is: 0x") + String(FmRadio.getStatus() & 0x07, HEX));

        GIVE_SEMAPHORE (RadioSemaphore, SkipSemaphore);
    }

    // DEBUG_END;
}

// *********************************************************************************************
cQN8027RadioApi QN8027RadioApi;

// *********************************************************************************************
// OEF
