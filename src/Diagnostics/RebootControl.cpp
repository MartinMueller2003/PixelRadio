/*
   File: RebootControl.cpp
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

#include "RebootControl.hpp"
#include "memdebug.h"

// *********************************************************************************************

// static const PROGMEM char   DIAG_BOOT_MSG1_STR    []    = "WARNING: SYSTEM WILL REBOOT<br>** RELEASE NOW TO ABORT **";
// static const PROGMEM char   DIAG_BOOT_MSG2_STR    []    = "** SYSTEM REBOOTING **<br>WAIT 30 SECONDS BEFORE ACCESSING WEB PAGE.";
// static const PROGMEM char   DIAG_LOG_MSG_STR      []    = "WARNING: SERIAL CONTROLLER IS ON";
static const PROGMEM char   DIAG_LONG_PRESS_STR   []    = "Long Press (5secs)";
static const PROGMEM char   DIAG_REBOOT_STR       []    = "REBOOT SYSTEM";

// *********************************************************************************************
cRebootControl::cRebootControl () :   cButtonControl (DIAG_REBOOT_STR)
{}

// *********************************************************************************************
void cRebootControl::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;
    // DEBUG_V(String("TabId: ") + String(TabId));
    // DEBUG_V(String("color: ") + String(color));

    cButtonControl::AddControls (TabId, color);
    setControlPanelStyle (ePanelStyle::PanelStyle135_black);
    setControl (DIAG_LONG_PRESS_STR, eCssStyle::CssStyleWhite_grey);

    // DEBUG_END;
}

// *********************************************************************************************
void cRebootControl::Callback (Control * sender, int type)
{
    // DEBUG_START;
    // DEBUG_V(String("TabId: ") + String(TabId));
    // DEBUG_V(String("color: ") + String(color));

    // if button pressed
        // record time
        // Set message and color to tell user to hold for 5 seconds

    // if button released
        // if time > min hold time
            // schedule a reboot
        // else
            // restore button

    // DEBUG_END;
}

// *********************************************************************************************
bool cRebootControl::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (   String ("       value: ") + value);
    // DEBUG_V (   String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (   String ("   DataValue: ") + String (DataValue));

    bool Response = cButtonControl::set (value, ResponseMessage, ForceUpdate);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cRebootControl RebootControl;

// *********************************************************************************************
// OEF
