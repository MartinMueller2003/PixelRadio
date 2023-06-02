/*
  *    File: AudioMute.cpp
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
#include <map>

#include "AudioMute.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static const PROGMEM char   RADIO_MUTE_FLAG []  = "RADIO_MUTE_FLAG";
static const PROGMEM char   ADJUST_MUTE_STR []  = "AUDIO MUTE";

// *********************************************************************************************
cAudioMute::cAudioMute () :   cBinaryControl (RADIO_MUTE_FLAG, ADJUST_MUTE_STR, true)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cAudioMute::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    setOffMessage (F ("Muted"), eCssStyle::CssStyleWhite);
    setOnMessage (F ("Unmuted"), eCssStyle::CssStyleWhite);

    cBinaryControl::AddControls (TabId, color);

    addInputCondition ( F ("unmuted"),  true);
    addInputCondition ( F ("muted"),    false);
    addInputCondition ( F ("unmute"),   true);
    addInputCondition ( F ("mute"),     false);

    // DEBUG_END;
}

// *********************************************************************************************
bool cAudioMute::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response = cBinaryControl::set (value, ResponseMessage, SkipLogOutput, ForceUpdate);

    if (Response)
    {
        QN8027RadioApi.setAudioMute (!value);
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cAudioMute AudioMute;

// *********************************************************************************************
// OEF
