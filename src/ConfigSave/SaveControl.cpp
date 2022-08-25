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

static const PROGMEM char     SAVE_SETTINGS_STR       [] = "SAVE SETTINGS";
static const PROGMEM char     SAVE_SETTINGS_MSG_STR   [] = "Settings Changed, Save Required";

// *********************************************************************************************
cSaveControl::cSaveControl () :   cButtonControl (SAVE_SETTINGS_STR)
{
    // DEBUG_START;
    // DEBUG_END;
}

// *********************************************************************************************
void cSaveControl::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;
    // DEBUG_V(String("TabId: ") + String(TabId));
    // DEBUG_V(String("color: ") + String(color));

    ESPUI.addControl (ControlType::Separator, SAVE_SETTINGS_STR, emptyString, ControlColor::None, TabId);
    cButtonControl::AddControls (TabId, color);

    // DEBUG_END;
}

// *********************************************************************************************
void cSaveControl::ClearSaveNeeded ()
{
    // DEBUG_START;

    setMessage(emptyString, eCssStyle::CssStyleTransparent);

    // DEBUG_END;
}

// *********************************************************************************************
bool cSaveControl::set (const String &, String & ResponseMessage, bool)
{
    // DEBUG_START;
    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response = true;

    ResponseMessage.clear ();

    Response = true;
    ConfigSave.InitiateSaveOperation ();

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cSaveControl::SetSaveNeeded ()
{
    // DEBUG_START;

    setMessage(SAVE_SETTINGS_MSG_STR, eCssStyle::CssStyleWhite);

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
