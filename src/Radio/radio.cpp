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
#include "DigitalAudioGain.hpp"
#include "FrequencyAdjust.hpp"
#include "PiCode.hpp"
#include "PreEmphasis.hpp"
#include "PtyCode.hpp"
#include "RdsText.hpp"
#include "RfCarrier.hpp"
#include "RfPower.hpp"
#include "TestTone.hpp"

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
    //_ DEBUG_START;

    TestTone.poll();
    RdsText.poll();

    //_ DEBUG_END;
}

// *********************************************************************************************
void cRadio::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    AnalogAudioGain.restoreConfiguration(config);
    AudioInputImpedance.restoreConfiguration(config);
    AudioMode.restoreConfiguration(config);
    AudioMute.restoreConfiguration(config);
    DigitalAudioGain.restoreConfiguration(config);
    FrequencyAdjust.restoreConfiguration(config);
    PiCode.restoreConfiguration(config);
    PreEmphasis.restoreConfiguration(config);
    PtyCode.saveConfiguration(config);
    RfCarrier.restoreConfiguration(config);
    RfPower.restoreConfiguration(config);
    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    AnalogAudioGain.saveConfiguration(config);
    AudioInputImpedance.saveConfiguration(config);
    AudioMode.saveConfiguration(config);
    AudioMute.saveConfiguration(config);
    DigitalAudioGain.saveConfiguration(config);
    FrequencyAdjust.saveConfiguration(config);
    PiCode.saveConfiguration(config);
    PreEmphasis.saveConfiguration(config);
    PtyCode.saveConfiguration(config);
    RfCarrier.saveConfiguration(config);
    RfPower.saveConfiguration(config);

    config[F("RADIO_AUTO_FLAG")] = rfAutoFlg;             // Use radio.radioNoAudioAutoOFF(0/1) when restoring this uint8 Value.
    config[F("ANALOG_VOLUME")]          = analogVol;        // Requires custom function, not written yet.

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
