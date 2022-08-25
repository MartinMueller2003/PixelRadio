/*
   File: RdsReset.cpp
   Project: PixelRdsReset, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <vector>

#include "RdsReset.hpp"
#include "PiCode.hpp"
#include "ProgramServiceName.hpp"
#include "PtyCode.hpp"
#include "memdebug.h"

// *********************************************************************************************

const String PROGMEM    RDS_RESET_SEP_STR   = "LOCAL RDS RESET";
const String PROGMEM    RDS_RESET_STR       = "RESET RDS SETTINGS";

// *********************************************************************************************
cRdsReset::cRdsReset () :   cButtonControl (RDS_RESET_STR)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cRdsReset::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    // DEBUG_V (String ("TabId: ") + String (TabId))
    // DEBUG_V (String ("color: ") + String (color))

    ESPUI.addControl (ControlType::Separator, RDS_RESET_SEP_STR.c_str (), emptyString, ControlColor::None, TabId);
    cButtonControl::AddControls (TabId, color);

    // DEBUG_END;
}

// *********************************************************************************************
bool cRdsReset::set (const String &, String & ResponseMessage, bool)
{
    // DEBUG_START;

    PiCode.ResetToDefaults ();
    PtyCode.ResetToDefaults ();
    ProgramServiceName.ResetToDefaults ();

    ResponseMessage = String (F ("Reset RDS Settings to defaults."));

    // DEBUG_END;

    return true;
}

// *********************************************************************************************
cRdsReset RdsReset;

// *********************************************************************************************
// OEF
