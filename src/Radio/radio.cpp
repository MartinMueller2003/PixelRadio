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
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

#include "AnalogAudioGain.hpp"
#include "AudioInputImpedance.hpp"
#include "AudioMode.hpp"
#include "AudioMute.hpp"
#include "FrequencyAdjust.hpp"
#include "RdsText.hpp"
#include "RfCarrier.hpp"
#include "TestTone.hpp"
#include "PreEmphasis.hpp"

// *********************************************************************************************
void cRadio::begin()
{
    // DEBUG_START;

    QN8027RadioApi.begin(); // If QN8027 fails we will warn user on UI homeTab.
    Log.infoln(String(F("FM Radio RDS/RBDS Started.")).c_str());

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::Poll()
{
    /// DEBUG_START;

    TestTone.poll();
    RdsText.poll();

    /// DEBUG_END;
}

// *********************************************************************************************
void cRadio::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    ReadFromJSON(rfPowerStr,        config, F("RADIO_POWER_STR"));
    ReadFromJSON(digitalGainStr,    config, F("DIGITAL_GAIN_STR"));

    AudioInputImpedance.restoreConfiguration(config);
    AudioMute.restoreConfiguration(config);
    AudioMode.restoreConfiguration(config);
    RfCarrier.restoreConfiguration(config);
    FrequencyAdjust.restoreConfiguration(config);
    PreEmphasis.restoreConfiguration(config);
    AnalogAudioGain.restoreConfiguration(config);

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    AudioInputImpedance.saveConfiguration(config);
    AudioMute.saveConfiguration(config);
    AudioMode.saveConfiguration(config);
    RfCarrier.saveConfiguration(config);
    FrequencyAdjust.saveConfiguration(config);
    PreEmphasis.saveConfiguration(config);
    AnalogAudioGain.saveConfiguration(config);

    config[F("RADIO_AUTO_FLAG")]        = rfAutoFlg;      // Use radio.radioNoAudioAutoOFF(0/1) when restoring this uint8 Value.
    config[F("RADIO_POWER_STR")]        = rfPowerStr;     // Use radio.setTxPower(20-75) when restoring this uint8 value.

    config[F("ANALOG_VOLUME")]          = analogVol;        // Requires custom function, not written yet.
    
    config[F("RDS_PI_CODE")]            = PiCode; // Use radio.setPiCode() when restoring this hex value.
    config[F("RDS_PTY_CODE")]           = PtyCode;
    config[F("RDS_PROG_SERV_STR")]      = ProgramServiceName;

    config[F("DIGITAL_GAIN_STR")]       = digitalGainStr;   // Use radio.setTxDigitalGain(0/1/2) when restoring this Int value.

    // DEBUG_END;
}

// *********************************************************************************************
// setDigitalGain(): Set the Tx Digital Gain on the QN8027 chip.
void cRadio::setDigitalGain(void)
{
    // DEBUG_START;

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
#ifdef OldWay

    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        delay(5);
        FmRadio.setTxDigitalGain(gainVal);
        delay(5);
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setPiCode(uint16_t value)
{
    // DEBUG_START;

    PiCode = value;
    setPiCode();
    String Response = String(F("0x")) + String(PiCode, HEX);
    ESPUI.print(rdsPiID, Response);

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setPiCode()
{
    // DEBUG_START;

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.setPiCode(PiCode);
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setProgramServiceName()
{
    // DEBUG_START;
#ifdef OldWay

    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.sendStationName(ProgramServiceName);
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }

#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setPtyCode()
{
    // DEBUG_START;

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.setPtyCode(PtyCode);
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
// setRfAutoOff():
// IMPORTANT: Sending RDS Messages and/or using updateUiAudioLevel() Will prevent 60S Turn Off.
// rfAutoOff = true: Turn-off RF Carrier if Audio is missing for > 60 seconds.
// rfAutoOff = false: Never turn off.
void cRadio::setRfAutoOff(void)
{
    // DEBUG_START;

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.radioNoAudioAutoOFF(rfAutoFlg ? ON : OFF );
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
// setRfPower(): Set the QN8027 chip's RF Power Output. Max 121dBuVp.
// Note: Power is not changed until RF Carrier is toggled Off then On.
void cRadio::setRfPower(void)
{
    // DEBUG_START;

#ifdef OldWay
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

    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        waitForIdle(10);
        FmRadio.setTxPower(pwrVal);
        waitForIdle(10);

        if (rfCarrierFlg)
        {
            setRfCarrier(OFF);
            setRfCarrier();
        }
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
// updateOnAirSign(): Turn on the "On Air" LED Sign if RF Carrier is present.
void cRadio::updateOnAirSign(void)
{
    // DEBUG_START;
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

    // DEBUG_END;
}

// *********************************************************************************************
// waitForIdle(): Wait for QN8027 to enter Idle State. This means register command has executed.
void cRadio::waitForIdle(uint16_t waitMs) 
{
    // DEBUG_START;
#ifdef OldWay
    uint8_t stateCode1;
    uint8_t stateCode2;

    do // once
    {
        stateCode1 = FmRadio.getStatus() & 0x07;
        // DEBUG_V(String("stateCode1: 0x") + String(stateCode1, HEX));

        // make sure the delay is a minimum of 5ms
        if(5 > waitMs) {waitMs = 5;}

        do
        {
            waitMs -= 5;

            delay(5);

            stateCode2 = FmRadio.getStatus() & 0x07;
            // DEBUG_V(String("stateCode2: 0x") + String(stateCode2, HEX));

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

    // DEBUG_V(String("waitForIdle End Status is: 0x") + String(FmRadio.getStatus() & 0x07, HEX));
#endif // def OldWay
    // DEBUG_END;
}

#ifdef OldWay
// *********************************************************************************************
// updateRadioSettings(): Update Any Radio Setting that has changed by the Web UI.
// This routine must be placed in main loop();
// QN8027 specific settings must not be changed in the Web UI Callbacks due to ESP32 core
// limitations. So instead the callbacks set a flag that tells this routine to perform the action.
void cRadio::updateRadioSettings(void)
{
    // DEBUG_START;

    if (newAutoRfFlg) 
    {
        newAutoRfFlg = false;
        setRfAutoOff();
    }

    // DEBUG_END;
}
#endif // def OldWay

// *********************************************************************************************
cRadio  Radio;

// *********************************************************************************************
// OEF
