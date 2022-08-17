/*
   File: AudioMute.cpp
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
#include "AudioMute.hpp"
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <map>

static const PROGMEM String     RADIO_MUTE_FLAG = "RADIO_MUTE_FLAG";
static const PROGMEM String     ADJUST_MUTE_STR = "AUDIO MUTE";

// *********************************************************************************************
cAudioMute::cAudioMute () : cControlCommon (ADJUST_MUTE_STR)
{
    // _ DEBUG_START;

    DataValueStr        = "0";
    DataValue           = false;

    ActiveLabelStyle    = CSS_LABEL_STYLE_WHITE;
    InactiveLabelStyle  = CSS_LABEL_STYLE_WHITE;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cAudioMute::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, ControlType::Switcher, color);
    ESPUI.updateControlLabel (ControlId, ADJUST_MUTE_STR.c_str ());
    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_WHITE);

    // DEBUG_END;
}

// *********************************************************************************************
bool cAudioMute::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (DataValueStr.equals (value))
        {
            // DEBUG_V("Ignore duplicate setting");
            break;
        }

        if (value.equals (F ("0")))
        {
            DataValue = 0;
        }
        else if (value.equals (F ("1")))
        {
            DataValue = 1;
        }
        else
        {
            ResponseMessage = String (F ("Radio Audio Mute Invalid Value: ")) + value;
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }
        DataValueStr    = value;
        ResponseMessage = DataValue ? F ("Muted") : F ("Unmuted");
        Response        = true;

        QN8027RadioApi.setAudioMute (!value);
        ESPUI.updateControlValue (ControlId, DataValueStr);
        ESPUI.print (StatusMessageId, ResponseMessage);
        ESPUI.setElementStyle (ControlId, DataValue ? String (F ("background: red;")) : String (F ("background: #bebebe;")));

        Log.infoln (String (F ("Audio Mute Set to: %s.")).c_str (), ResponseMessage.c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cAudioMute AudioMute;

// *********************************************************************************************
// OEF
