/*
   File: radio.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <Wire.h>
#include "PixelRadio.h"
#include "globals.h"
#include "language.h"
#include "radio.hpp"
#include "memdebug.h"

// *********************************************************************************************
void cRadio::begin()
{
    DEBUG_START;

    RadioSemaphore = xSemaphoreCreateBinary();

    fmRadioTestCode = Radio.initRadioChip(); // If QN8027 fails we will warn user on UI homeTab.
    Log.infoln(String(F("FM Radio RDS/RBDS Started.")).c_str());

    DEBUG_END;
}

// *********************************************************************************************
// calibrateAntenna(): Calibrate the QN8027 Antenna Interface. On exit, true if Calibration OK.
// This is an undocumented feature that was found during PixelRadio project development.
// This routine is called once during startup(). Takes about 2 Secs to execute.
bool cRadio::calibrateAntenna(void)
{
    DEBUG_START;

    bool successFlg = true;
    uint8_t regVal1;
    uint8_t regVal2;

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    FmRadio.setFrequency(108.0F); // High end of FM tuning range.
    waitForIdle(50);
    FmRadio.Switch(OFF);
    waitForIdle(15);
    FmRadio.Switch(ON);
    waitForIdle(50);

    FmRadio.reCalibrate();
    waitForIdle(120);

    regVal1 = FmRadio.read1Byte(ANT_REG);

    FmRadio.setFrequency(85.0F); // Low end of FM tuning range.
    waitForIdle(50);
    FmRadio.Switch(OFF);
    waitForIdle(15);
    FmRadio.Switch(ON);
    waitForIdle(50);

    FmRadio.reCalibrate();
    waitForIdle(120);

    regVal2 = FmRadio.read1Byte(ANT_REG);

    Log.verboseln(String(F("-> QN8027 RF Port Test: Low RF Range= 0x%02X, High RF Range= 0x%02X")).c_str(), regVal1, regVal2);

    if (regVal1 == 0x00 && regVal2 == 0x00) {
        successFlg = false;
        Log.errorln("-> QN8027 RF Port Calibration Failed, Possible Oscillator Failure.");
    }
    else if ((regVal1 <= 0x01) || (regVal1 >= 0x1f) || (regVal2 <= 0x01) || (regVal2 >= 0x1f)) {
        successFlg = false;
        Log.errorln("-> QN8027 RF Port has Poor Calibration, RF Tuning Range Impaired.");
    }
    else {
        successFlg = true;
        Log.infoln("-> QN8027 RF Port Matching OK, Calibration Successful.");
    }

    /*
       radio.setFrequency(103.0F); // Recalibrate at middle FM range.
       waitForIdle(50);
       radio.Switch(OFF);
       waitForIdle(15);
       radio.Switch(ON);
       waitForIdle(50);
     */

    FmRadio.reCalibrate();
    waitForIdle(120);
    xSemaphoreGive(RadioSemaphore);

    setRfCarrier();
    waitForIdle(50);

    DEBUG_END;
    return successFlg;
}

// *********************************************************************************************
//
// checkRadioIsPresent(): Check to see if QN8027 FM Radio Chip is installed. Return true if Ok.
bool cRadio::checkRadioIsPresent(void) 
{
    DEBUG_START;

    bool response = false;
    Wire.beginTransmission(QN8027_I2C_ADDR);

    if (Wire.endTransmission(true) == 0) 
    { // Receive 0 = success (ACK response)
        response = true;
    }

    DEBUG_END;
    return false;
}

// ************************************************************************************************
// getAudioGain(): Compute the Audio Gain.
// Radio Audio Gain in dB = ((Analog Input Gain Step + 1) * 3) - (Input Impedance Step * 3)
// Please note that the formula has been modified based on real-world measurements.
// The official formula uses 6dB Impedance steps vs the revised use of 3dB.
int8_t cRadio::getAudioGain(void)
{
    DEBUG_START;

    int8_t vgaGain;
    int8_t impedance;
    int8_t audioGain;

    if (vgaGainStr.equals(VGA_GAIN0_STR))
    {
        vgaGain = 0;
    }
    else if (vgaGainStr.equals(VGA_GAIN1_STR))
    {
        vgaGain = 1;
    }
    else if (vgaGainStr.equals(VGA_GAIN2_STR))
    {
        vgaGain = 2;
    }
    else if (vgaGainStr.equals(VGA_GAIN3_STR))
    {
        vgaGain = 3;
    }
    else if (vgaGainStr.equals(VGA_GAIN4_STR))
    {
        vgaGain = 4;
    }
    else if (vgaGainStr.equals(VGA_GAIN5_STR))
    {
        vgaGain = 5;
    }
    else 
    {
        vgaGain = 0;
        Log.errorln("getAudioGain: Undefined vgaGainStr!");
    }

    if (inpImpedStr.equals(INP_IMP05K_STR))
    {
        impedance = 0;
    }
    else if (inpImpedStr.equals(INP_IMP10K_STR))
    {
        impedance = 1;
    }
    else if (inpImpedStr.equals(INP_IMP20K_STR))
    {
        impedance = 2;
    }
    else if (inpImpedStr.equals(INP_IMP40K_STR))
    {
        impedance = 3;
    }
    else 
    {
        impedance = 0;
        Log.errorln(F("displayAudioGain: Undefined inpImpedStr!"));
    }

    audioGain = ((vgaGain + 1) * 3) - (impedance * 3);

    DEBUG_END;
    return audioGain;
}

// *********************************************************************************************
// initRadioChip(); Initialize the QN8027 FM Radio Chip. Returns Test Result Code.
uint8_t cRadio::initRadioChip(void) 
{
    DEBUG_START;

    uint8_t testCode = FM_TEST_OK;
    uint8_t regVal;

    Log.infoln(String(F("Initializing QN8027 FM Radio Chip ...")).c_str());

    if (checkRadioIsPresent()) 
    {
        Log.verboseln(String(F("-> QN8027 is Present")).c_str());
    }
    else 
    {
        Log.errorln(String(F("-> QN8027 is Missing")).c_str());
        return FM_TEST_FAIL;
    }

    regVal = FmRadio.read1Byte(CID1_REG);

    if ((regVal > 0x00) && ((regVal & 0x0C) == 0x00)) 
    {
        successFlg = true;
        Log.verboseln(String(F("-> CID1 Chip Family ID: 0x%02X")).c_str(), regVal);
    }
    else 
    {
        Log.errorln(String(F("-> Incorrect CID1 Chip Family ID: 0x%02X")).c_str(), regVal);
        // return FM_TEST_FAIL;
    }

    regVal = FmRadio.read1Byte(CID2_REG);

    if ((regVal & 0xF0) == 0x40) 
    {
        successFlg = true;
        Log.verboseln(String(F("-> CID2 Chip Version: 0x%02X")).c_str(), regVal);
    }
    else 
    {
        Log.errorln(String(F("-> Incorrect CID2 Version: 0x%02X")).c_str(), regVal);
    }

    FmRadio.reset();
    delay(30);

    FmRadio.setClockSource(0x00); // XTAL on pins 1 & 2.
    //    delay(5);
    FmRadio.setCrystalFreq(12);

    //    delay(5);
    FmRadio.setCrystalCurrent(30); // 30% of 400uA Max = 120uA.
    //    delay(5);

    // Log.infoln(String(F("Radio XTal Status is: %02X")).c_str(), radio.getStatus());

    FmRadio.setTxFreqDeviation(0x81); // 75Khz, Total Broadcast channel Bandwidth
    //    delay(10);

    // Log.infoln(String(F("Radio FreqDev Status is: %02X")).c_str(), radio.getStatus());

    FmRadio.setTxPilotFreqDeviation(9); // Use default 9% (6.75KHz) Pilot Tone Deviation.
    //    delay(10);

    // Log.infoln(String(F("Radio TxPilotDeviation Status is: %02X")).c_str(), radio.getStatus());

    setRfPower();
    waitForIdle(25);

    // Log.infoln(String(F("Radio Tx Power Status is: %02X")).c_str(), radio.getStatus());

    for (uint8_t i = 0; i < RADIO_CAL_RETRY; i++) 
    { // Allow several attempts to get good port matching results.
        if (calibrateAntenna()) 
        {                   // QN8027 RF Port Matching OK, exit.
            testCode = FM_TEST_OK;
            break;
        }
        else 
        {
            testCode = FM_TEST_VSWR; // Report High VSWR.

            if (i < RADIO_CAL_RETRY - 1) 
            {
                Log.infoln(String(F( "-> Retesting QN8027 RF Port Matching, Retry #%d")).c_str(), i + 1);
            }
        }
    }
    waitForIdle(50);

    // Log.infoln(String(F("Calibrate Antenna Status is: %02X")).c_str(), radio.getStatus());

    setPreEmphasis();
    setVgaGain(); // Tx Input Buffer Gain.
    setDigitalGain();
    setAudioImpedance();
    waitForIdle(10);

    // Log.infoln(String(F("Radio Audio Configuration Status: %02X")).c_str(), radio.getStatus());

    FmRadio.MonoAudio(!stereoEnbFlg);
    delay(5);

    // Log.infoln(String(F("Radio Mono Audio Status is: %02X")).c_str(), radio.getStatus());

    FmRadio.scrambleAudio(OFF);
    delay(5);

    // Log.infoln(String(F("Radio Privacy Status is: %02X")).c_str(), radio.getStatus());

    FmRadio.clearAudioPeak();
    delay(5);

    // Log.infoln(String(F("Radio Clear Peak Status is: %02X")).c_str(), radio.getStatus());

    FmRadio.mute(muteFlg);
    delay(5);

    // Log.infoln(String(F("Radio Clear Peak Status is: %02X")).c_str(), radio.getStatus());

    setRfAutoOff();

    // Log.infoln(String(F("Radio RF Auto Shutoff Status is: %02X")).c_str(), radio.getStatus());

    FmRadio.Switch(uint8_t(rfCarrierFlg));
    waitForIdle(50);

    // Log.infoln(String(F("Radio Switch Status is: %02X")).c_str(), radio.getStatus());

    FmRadio.setFrequency((float(fmFreqX10)) / 10.0f);
    waitForIdle(50);
    Log.infoln(String(F( "-> Radio Frequency: %3.1f")).c_str(), FmRadio.getFrequency());

    FmRadio.setRDSFreqDeviation(10); // RDS Freq Deviation = 0.35KHz * Value.
    delay(1);
    FmRadio.RDS(ON);
    waitForIdle(20);

    // Log.infoln(String(F("Radio RF Carrier Status is: %02X")).c_str(), radio.getStatus());

    FmRadio.updateSYSTEM_REG(); // This is needed to Start FM Broadcast.
    waitForIdle(20);
    FmRadio.clearAudioPeak();
    delay(1);
    Log.infoln(String(F("-> Radio Status: %02X")).c_str(), (FmRadio.getStatus() & 0x07));

    FmRadio.setPiCode(PiCode);
    FmRadio.setPtyCode(PtyCode);

    if (successFlg) 
    {
        Log.infoln(String(F("-> QN8027 Initialization Complete.")).c_str());
    }
    else 
    {
        Log.errorln(String(F("-> QN8027 Failed Initialization.")).c_str());
    }

    /*
        // DEBUG ONLY.
        for (int i = 0; i <= 0x1F; i++) {
            Log.infoln(String(F( "Radio Register %02X is: %02X")).c_str(), i, radio.read1Byte(i));
        }
     */

    DEBUG_END;
    return testCode;
}

// *********************************************************************************************
// measureAudioLevel(): Measure the Peak Audio Level. Max 675mV.
uint16_t cRadio::measureAudioLevel(void) 
{
    DEBUG_START;

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    uint16_t mV = FmRadio.getStatus() >> 4;
    mV = mV * 45; // Audio Peak is 45mV per count.
    FmRadio.clearAudioPeak();
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
    return mV;
}

// *********************************************************************************************
void cRadio::restoreConfiguration(JsonObject & config)
{
    DEBUG_START;

    ReadFromJSON(rfCarrierFlg,      config, F("RADIO_RF_CARR_FLAG"));
    ReadFromJSON(stereoEnbFlg,      config, F("RADIO_STEREO_FLAG"));
    ReadFromJSON(preEmphasisStr,    config, F("RADIO_PRE_EMPH_STR"));
    ReadFromJSON(rfPowerStr,        config, F("RADIO_POWER_STR"));
    ReadFromJSON(vgaGainStr,        config, F("ANALOG_GAIN_STR"));
    ReadFromJSON(digitalGainStr,    config, F("DIGITAL_GAIN_STR"));
    ReadFromJSON(inpImpedStr,       config, F("INPUT_IMPED_STR"));

    DEBUG_END;
}

// *********************************************************************************************
void cRadio::saveConfiguration(JsonObject & config)
{
    DEBUG_START;

    config[F("RADIO_FM_FREQ")]          = fmFreqX10;      // Use radio.setFrequency(MHZ) when restoring this uint16 value.
    config[F("RADIO_MUTE_FLAG")]        = muteFlg;        // Use radio.mute(0/1) when restoring this uint8 value. 1=MuteOn
    config[F("RADIO_AUTO_FLAG")]        = rfAutoFlg;      // Use radio.radioNoAudioAutoOFF(0/1) when restoring this uint8 Value.
    config[F("RADIO_RF_CARR_FLAG")]     = rfCarrierFlg;   // Use radio.RDS(0/1) when restoring this uint8 value. 1=CarrierOn.
    config[F("RADIO_STEREO_FLAG")]      = stereoEnbFlg;   // Use radio.MonoAudio(0/1) when restoring this uint8 value. 0=Stereo.
    config[F("RADIO_PRE_EMPH_STR")]     = preEmphasisStr; // Use radio.setPreEmphTime50(0/1), uint8 value. Not working?
    config[F("RADIO_POWER_STR")]        = rfPowerStr;     // Use radio.setTxPower(20-75) when restoring this uint8 value.

    config[F("ANALOG_VOLUME")]          = analogVol;        // Requires custom function, not written yet.
    config[F("ANALOG_GAIN_STR")]        = vgaGainStr;       // Use radio.setTxInputBufferGain(0-5) when restoring this Int value.
    
    config[F("RDS_PI_CODE")]            = PiCode; // Use radio.setPiCode() when restoring this hex value.
    config[F("RDS_PTY_CODE")]           = PtyCode;
    config[F("RDS_PROG_SERV_STR")]      = ProgramServiceName;

    config[F("DIGITAL_GAIN_STR")]       = digitalGainStr;   // Use radio.setTxDigitalGain(0/1/2) when restoring this Int value.
    config[F("INPUT_IMPED_STR")]        = inpImpedStr;      // Use radio.setAudioInpImp(5/10/20/40) when restoring this Int value.

    DEBUG_END;
}

// *********************************************************************************************
// setAudioImpedance(): Set the Audio Input Impedance on the QN8027 chip.
void cRadio::setAudioImpedance(void)
{
    DEBUG_START;

    uint8_t impedVal = 0;

    if (inpImpedStr.equals(INP_IMP05K_STR))
    {
        impedVal = 5;
    }
    else if (inpImpedStr.equals(INP_IMP10K_STR))
    {
        impedVal = 10;
    }
    else if (inpImpedStr.equals(INP_IMP20K_STR))
    {
        impedVal = 20;
    }
    else if (inpImpedStr.equals(INP_IMP40K_STR))
    {
        impedVal = 40;
    }

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    delay(5);
    FmRadio.setAudioInpImp(impedVal);
    delay(5);
    xSemaphoreGive(RadioSemaphore);

    DEBUG_V(String("impedVal: ") + String(impedVal));
    
    DEBUG_END;
}

// *********************************************************************************************
// setAudioMute(): Control the Audio Mute on the QN8027 chip.
void cRadio::setAudioMute(void)
{
    DEBUG_START;

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    delay(5);
    FmRadio.mute(muteFlg ? ON : OFF);
    delay(5);
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// setDigitalGain(): Set the Tx Digital Gain on the QN8027 chip.
void cRadio::setDigitalGain(void)
{
    DEBUG_START;

    uint8_t gainVal = 0;

    // Log.verboseln(String(F( "-> Digital Audio Gain Set to: %s.")).c_str(), digitalGainStr.c_str());

    if (digitalGainStr.equals(DIG_GAIN0_STR))
    {
        gainVal = 0; // 0dB.
    }
    else if (digitalGainStr.equals(DIG_GAIN1_STR))
    {
        gainVal = 1; // 1dB.
    }
    else if (digitalGainStr.equals(DIG_GAIN2_STR))
    {
        gainVal = 2; // 2dB.
    }
    else 
    {
        Log.errorln(String(F("setDigitalGain: Invalid Value, Using Default")).c_str());
        digitalGainStr = DIG_GAIN_DEF_STR;
        gainVal        = 0;
    }

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    delay(5);
    FmRadio.setTxDigitalGain(gainVal);
    delay(5);
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// setFrequency(): Set the Radio Frequency on the QN8027.
void cRadio::setFrequency(void)
{
    DEBUG_START;

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    waitForIdle(5);
    FmRadio.Switch(OFF); // Turn Off Carrier.
    waitForIdle(5);
    FmRadio.setFrequency((float(fmFreqX10)) / 10.0f);
    waitForIdle(25);
    FmRadio.Switch(rfCarrierFlg ? ON : OFF); // Restore Carrier to user's state.
    waitForIdle(25);
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// setMonoAudio(): Control the Audio Stereo/Mono mode on the QN8027 chip.
void cRadio::setMonoAudio(void)
{
    DEBUG_START;

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    delay(5);
    FmRadio.MonoAudio(stereoEnbFlg ? OFF : ON);
    delay(5);
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// ssetPreEmphasis(): Set the QN8027 chip's pre-Emphasis.
// ON = 50uS (Eur/UK/Australia), OFF = 75uS (North America/Japan).
void cRadio::setPreEmphasis(void)
{
    DEBUG_START;

    uint8_t emphVal;

    // Log.verboseln(String(F( "-> Pre-Emphasis Set to: %s.")).c_str(), preEmphasisStr.c_str());

    if (preEmphasisStr.equals(PRE_EMPH_EUR_STR))
    {
        emphVal = ON;
    }
    else if (preEmphasisStr.equals(PRE_EMPH_USA_STR))
    {
        emphVal = OFF;
    }
    else 
    {
        Log.errorln("setPreEmphasis: Invalid Value, Using Default");
        preEmphasisStr = PRE_EMPH_USA_STR;
        emphVal        = OFF;
    }

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    delay(1);
    FmRadio.Switch(OFF); // Turn Off Carrier.
    delay(1);
    FmRadio.setPreEmphTime50(emphVal);
    waitForIdle(10);
    FmRadio.Switch(rfCarrierFlg ? ON : OFF); // Restore Carrier to user's state.
    waitForIdle(25);
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// setRfAutoOff():
// IMPORTANT: Sending RDS Messages and/or using updateUiAudioLevel() Will prevent 60S Turn Off.
// rfAutoOff = true: Turn-off RF Carrier if Audio is missing for > 60 seconds.
// rfAutoOff = false: Never turn off.
void cRadio::setRfAutoOff(void)
{
    DEBUG_START;

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    FmRadio.Switch(OFF); // Turn Off Carrier.
    delay(5);
    FmRadio.radioNoAudioAutoOFF(rfAutoFlg ? ON : OFF );
    delay(5);
    FmRadio.Switch(rfCarrierFlg ? ON : OFF ); // Restore Carrier to user's state.
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// setRfCarrier(): Set the QN8027 chip's RF Carrier Output (On/Off).
void cRadio::setRfCarrier(void)
{
    DEBUG_START;

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    waitForIdle(10);
    FmRadio.Switch(rfCarrierFlg ? ON : OFF ); // Update QN8027 Carrier.
    waitForIdle(25);
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// setRfPower(): Set the QN8027 chip's RF Power Output. Max 121dBuVp.
// Note: Power is not changed until RF Carrier is toggled Off then On.
void cRadio::setRfPower(void)
{
    DEBUG_START;

    uint8_t pwrVal = RF_HIGH_POWER;

    // Log.verboseln(String(F( "-> RF Power Set to: %s")).c_str(), rfPowerStr.c_str());

    if (rfPowerStr.equals(RF_PWR_LOW_STR))
    {
        pwrVal = RF_LOW_POWER;
    }
    else if (rfPowerStr.equals(RF_PWR_MED_STR))
    {
        pwrVal = RF_MED_POWER;
    }
    else if (rfPowerStr.equals(RF_PWR_HIGH_STR))
    {
        pwrVal = RF_HIGH_POWER;
    }
    else 
    {
        Log.errorln(String(F("setRfPower: Invalid Value, Using High Power")).c_str());
        rfPowerStr = RF_PWR_HIGH_STR;
        pwrVal     = RF_HIGH_POWER;
    }

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    waitForIdle(10);
    FmRadio.setTxPower(pwrVal);
    waitForIdle(10);

    if (rfCarrierFlg)
    {
        FmRadio.Switch(OFF); // Turn Off Carrier.
        waitForIdle(10);
        FmRadio.Switch(ON); // Update QN8027 Carrier.
        waitForIdle(25);
    }
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// setVgaGain(): Set the Tx Input Buffer Gain (Analog Gain) on the QN8027 chip.
void cRadio::setVgaGain(void)
{
    DEBUG_START;
    uint8_t gainVal = 0;

    if (vgaGainStr.equals(VGA_GAIN0_STR))
    {
        gainVal = 0x00;
    }
    else if (vgaGainStr.equals(VGA_GAIN1_STR))
    {
        gainVal = 0x01;
    }
    else if (vgaGainStr.equals(VGA_GAIN2_STR))
    {
        gainVal = 0x02;
    }
    else if (vgaGainStr.equals(VGA_GAIN3_STR))
    {
        gainVal = 0x03;
    }
    else if (vgaGainStr.equals(VGA_GAIN4_STR))
    {
        gainVal = 0x04;
    }
    else if (vgaGainStr.equals(VGA_GAIN5_STR))
    {
        gainVal = 0x05;
    }
    else 
    {
        Log.errorln(String(F("setVgaGain: Invalid Value, Using Default")).c_str());
        vgaGainStr = VGA_GAIN3_STR;
        gainVal    = 0x03;
    }

    xSemaphoreTake(RadioSemaphore, portMAX_DELAY);
    delay(5);
    FmRadio.setTxInputBufferGain(gainVal);
    delay(5);
    xSemaphoreGive(RadioSemaphore);

    DEBUG_END;
}

// *********************************************************************************************
// updateOnAirSign(): Turn on the "On Air" LED Sign if RF Carrier is present.
void cRadio::updateOnAirSign(void)
{
    DEBUG_START;
#ifdef OldWay
new way: call GPIO and tell it to update the output and the UI.

    if (rfCarrierFlg)
    {
        digitalWrite(ON_AIR_PIN, SIGN_ON);
    }
    else 
    {
        digitalWrite(ON_AIR_PIN, SIGN_OFF);
    }
#endif // def OldWay

    DEBUG_END;
}

// *********************************************************************************************
// waitForIdle(): Wait for QN8027 to enter Idle State. This means register command has executed.
void cRadio::waitForIdle(uint16_t waitMs) 
{
    DEBUG_START;
#ifdef OldWay
    uint8_t stateCode1;
    uint8_t stateCode2;

    do // once
    {
        stateCode1 = FmRadio.getStatus() & 0x07;
        DEBUG_V(String("stateCode1: 0x") + String(stateCode1, HEX));

        // make sure the delay is a minimum of 5ms
        if(5 > waitMs) {waitMs = 5;}

        do
        {
            waitMs -= 5;

            delay(5);

            stateCode2 = FmRadio.getStatus() & 0x07;
            DEBUG_V(String("stateCode2: 0x") + String(stateCode2, HEX));

            if ((stateCode2 == 0x02) || (stateCode2 == 0x05))
            {
                break;
            }
            else if  (stateCode1 != stateCode2) 
            {
                FmRadio.getStatus(); // Clear next status byte.
                break;
            }
        } while (5 <= waitMs);

    } while (false);

    DEBUG_V(String("waitForIdle End Status is: 0x") + String(FmRadio.getStatus() & 0x07, HEX));
#endif // def OldWay
    DEBUG_END;
}


#ifdef OldWay
// *********************************************************************************************
// updateRadioSettings(): Update Any Radio Setting that has changed by the Web UI.
// This routine must be placed in main loop();
// QN8027 specific settings must not be changed in the Web UI Callbacks due to ESP32 core
// limitations. So instead the callbacks set a flag that tells this routine to perform the action.
void cRadio::updateRadioSettings(void)
{
    DEBUG_START;

    if (newAutoRfFlg) 
    {
        newAutoRfFlg = false;
        setRfAutoOff();
    }

    if (newAudioModeFlg) 
    {
        newAudioModeFlg = false;
        setMonoAudio();
    }

    if (newPreEmphFlg) 
    {
    }

    DEBUG_END;
}
#endif // def OldWay

// *********************************************************************************************
cRadio  Radio;

// *********************************************************************************************
// OEF
