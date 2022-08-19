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
#include "AudioMode.hpp"
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String     RADIO_STEREO_FLAG               = "RADIO_STEREO_FLAG";
static const PROGMEM char       RADIO_MONO_STR        []        = "MONO AUDIO";
static const PROGMEM char       RADIO_STEREO_STR      []        = "STEREO AUDIO";
static const PROGMEM String     RADIO_AUDIO_MODE_STR            = "AUDIO MODE";

// *********************************************************************************************
cAudioMode::cAudioMode () : cOldControlCommon (RADIO_STEREO_FLAG)
{
    // _ DEBUG_START;

    DataValueStr        = "1";
    DataValue           = true;

    ActiveLabelStyle    = CSS_LABEL_STYLE_BLACK;
    InactiveLabelStyle  = CSS_LABEL_STYLE_BLACK;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cAudioMode::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Switcher, color);
    ESPUI.updateControlLabel (ControlId, RADIO_AUDIO_MODE_STR.c_str ());
    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_BLACK);

    // DEBUG_V();


    // DEBUG_END;
}

// *********************************************************************************************
bool cAudioMode::set (String & value, String & ResponseMessage)
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
            ResponseMessage = String (F ("Radio Audio Mode Invalid Value: ")) + value;
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }
        DataValueStr    = value;
        ResponseMessage = DataValue ? F (RADIO_STEREO_STR) : F (RADIO_MONO_STR);
        Response        = true;

        QN8027RadioApi.setMonoAudio (DataValue);
        ESPUI.updateControlValue (ControlId, DataValueStr);
        ESPUI.print (StatusMessageId, ResponseMessage);

        Log.infoln (String (F ("Radio Audio Mode Set to: %s.")).c_str (), ResponseMessage.c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cAudioMode AudioMode;

// *********************************************************************************************
// OEF
