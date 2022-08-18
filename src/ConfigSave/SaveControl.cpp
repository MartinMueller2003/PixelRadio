/*
   File: SaveControl.cpp
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
#include "ConfigSave.hpp"
#include "memdebug.h"
#include "SaveControl.hpp"

// *********************************************************************************************

static const PROGMEM String     SAVE_SETTINGS_STR       = "SAVE SETTINGS";
static const PROGMEM String     SAVE_SETTINGS_MSG_STR   = "[ Settings Changed, Save Required ]";

// *********************************************************************************************
cSaveControl::cSaveControl () : cOldControlCommon (emptyString)
{
    // DEBUG_START;

    DataValueStr        = "0";
    DataValue           = false;

    ActiveLabelStyle    = CSS_LABEL_STYLE_RED;
    InactiveLabelStyle  = CSS_LABEL_STYLE_BLACK;

    // DEBUG_END;
}

// *********************************************************************************************
void cSaveControl::AddControls (uint16_t TabId, ControlColor _color)
{
    // DEBUG_START;
    // DEBUG_V(String("TabId: ") + String(TabId));
    // DEBUG_V(String("color: ") + String(color));

    color = _color;

    ESPUI.addControl (ControlType::Separator, SAVE_SETTINGS_STR.c_str (), emptyString, ControlColor::None, TabId);

    cOldControlCommon::AddControls (TabId, ControlType::Button, color);
    ESPUI.updateControlLabel (ControlId, SAVE_SETTINGS_STR.c_str ());
    ESPUI.updateControlValue (ControlId, SAVE_SETTINGS_STR);
    ESPUI.      setElementStyle (       StatusMessageId, CSS_LABEL_STYLE_WHITE);
    ESPUI.      setElementStyle (       ControlId,
        String (F ("background-color: grey; color: white; margin-top: .1rem; margin-bottom: .1rem;")));

    // DEBUG_END;
}

// *********************************************************************************************
void cSaveControl::ClearSaveNeeded ()
{
    // DEBUG_START;

    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_BLACK);
    ESPUI.updateControlValue (StatusMessageId, emptyString);
    ESPUI.updateControlLabel (StatusMessageId, emptyString.c_str ());

    // DEBUG_END;
}

// *********************************************************************************************
bool cSaveControl::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;
    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response = true;

    ResponseMessage.clear ();

    if (!IgnoreFirstSet)
    {
        Response = true;
        ConfigSave.InitiateSaveOperation ();
    }
    else
    {
        IgnoreFirstSet = false;
    }
    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cSaveControl::SetSaveNeeded ()
{
    // DEBUG_START;

    ESPUI.updateControlValue (StatusMessageId, SAVE_SETTINGS_MSG_STR);
    ESPUI.updateControlLabel (StatusMessageId, SAVE_SETTINGS_MSG_STR.c_str ());
    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_WHITE);

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
