/*
   File: AudioMode.cpp
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

#include "AudioMode.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static const PROGMEM String     RADIO_STEREO_FLAG       = "RADIO_STEREO_FLAG";
static const PROGMEM String     RADIO_MONO_STR          = "MONO AUDIO";
static const PROGMEM String     RADIO_STEREO_STR        = "STEREO AUDIO";
static const PROGMEM String     RADIO_AUDIO_MODE_STR    = "AUDIO MODE";

// *********************************************************************************************
cAudioMode::cAudioMode () :   cBinaryControl (RADIO_STEREO_FLAG, RADIO_AUDIO_MODE_STR, true)
{
    // _ DEBUG_START;

    OnString    = RADIO_STEREO_STR;
    OffString   = RADIO_MONO_STR;

    // _ DEBUG_END;
}

// *********************************************************************************************
bool cAudioMode::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response = cBinaryControl::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        QN8027RadioApi.setMonoAudio (!DataValue);
    }
    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cAudioMode AudioMode;

// *********************************************************************************************
// OEF
