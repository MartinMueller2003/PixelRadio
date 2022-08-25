/*
   File: DigitalAudioGain.cpp
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
#include <map>

#include "AudioGain.hpp"
#include "DigitalAudioGain.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static ChoiceListVector_t MapOfGainValues
{
    {"0 dB",  "0"},
    {"1 dB",  "1"},
    {"2 dB",  "2"},
};

static const PROGMEM char   RADIO_DIG_AUDIO_STR     []  = "DIGITAL AUDIO GAIN";
static const PROGMEM char   DIG_GAIN_DEF_STR        []  = "0 dB";
static const PROGMEM char   DIGITAL_GAIN_STR        []  = "DIGITAL_GAIN_STR";

// *********************************************************************************************
cDigitalAudioGain::cDigitalAudioGain () :   cChoiceListControl (DIGITAL_GAIN_STR, RADIO_DIG_AUDIO_STR, DIG_GAIN_DEF_STR, & MapOfGainValues)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cDigitalAudioGain::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cChoiceListControl::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        // DEBUG_V ("Update the radio")
        // DEBUG_V(String("get32: ") + String(get32()));
        QN8027RadioApi.setDigitalGain (get32 ());
        AudioGain.set ();
    }
    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cDigitalAudioGain DigitalAudioGain;

// *********************************************************************************************
// OEF
