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
#include "memdebug.h"
#include "PiCode.hpp"
#include "ProgramServiceName.hpp"
#include "PtyCode.hpp"
#include "RdsReset.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <vector>

// *********************************************************************************************

#define RDS_RESET_SEP_STR     "LOCAL RDS RESET"
#define RDS_RESET_STR         "RESET RDS SETTINGS"

// *********************************************************************************************
cRdsReset::cRdsReset () : cOldControlCommon (emptyString)
{
    // _ DEBUG_START;

    DataValue           = 0;
    DataValueStr        = "0";

    // _ DEBUG_END;
}

// *********************************************************************************************
void cRdsReset::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    // DEBUG_V (String ("value: ") + String (value))
    // DEBUG_V (String ("color: ") + String (color))

    ESPUI.addControl (ControlType::Separator, RDS_RESET_SEP_STR, emptyString, ControlColor::None, value);

    cOldControlCommon::AddControls (value, ControlType::Button, color);
    ESPUI.updateControlValue (ControlId, RDS_RESET_STR);
    ESPUI.updateControlLabel (ControlId, RDS_RESET_STR);
    ESPUI.setPanelStyle (ControlId, String (F ("font-size: 1.35em;")));
    ESPUI.setElementStyle (ControlId, String (F ("color: white;")));

    // DEBUG_END;
}

// *********************************************************************************************
bool cRdsReset::set (String & value, String & ResponseMessage)
{
    // DEBUG_START;

    // do something here
    // DEBUG_V (String ("value: ") + String (value));

    if (DataValue)
    {
        PiCode.ResetToDefaults ();
        PtyCode.ResetToDefaults ();
        ProgramServiceName.ResetToDefaults ();

        Log.infoln (String (F ("Reset RDS Settings to defaults.")).c_str ());
    }
    ++DataValue;

    // DEBUG_END;

    return true;
}

// *********************************************************************************************
cRdsReset RdsReset;

// *********************************************************************************************
// OEF
