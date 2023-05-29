/*
  *    File: RebootControl.cpp
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

#include "RebootControl.hpp"
#include "memdebug.h"

// *********************************************************************************************

static const PROGMEM char   DIAG_BOOT_MSG1_STR    []    = "WARNING: SYSTEM WILL REBOOT<br>** RELEASE NOW TO ABORT **";
static const PROGMEM char   DIAG_BOOT_MSG2_STR    []    = "WARNING: SYSTEM WILL REBOOT WHEN BUTTON IS RELEASED";
static const PROGMEM char   DIAG_BOOT_MSG3_STR    []    = "** SYSTEM REBOOTING **<br>WAIT 30 SECONDS BEFORE ACCESSING WEB PAGE.";
// static const PROGMEM char   DIAG_LOG_MSG_STR      []    = "WARNING: SERIAL CONTROLLER IS ON";
static const PROGMEM char   DIAG_LONG_PRESS_STR   []    = "Long Press (5secs)";
static const PROGMEM char   DIAG_REBOOT_STR       []    = "REBOOT SYSTEM";

static const PROGMEM char RebootWarning       [] =
        "USER REBOOT!\r\n\r\n" \
        "          ******************\r\n" \
        "          * SYSTEM REBOOT! *\r\n" \
        "          ******************\r\n\r\n";

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
void cRebootControl::Callback (Control *, int type)
{
    // DEBUG_START;
    // DEBUG_V(String("         type: ") + String(type));
    // DEBUG_V(String("ButtonEndTime: ") + String(ButtonEndTime));
    // DEBUG_V(String("  WarningSent: ") + String(WarningSent));
    // DEBUG_V(String("   RebootTime: ") + String(RebootTime));
    // DEBUG_V(String("TabId: ") + String(TabId));
    // DEBUG_V(String("color: ") + String(color));

    if (B_DOWN == type)
    {
        // DEBUG_V("Button Pressed");

        ButtonEndTime   = millis () + FiveSecondsInMs;
        WarningSent     = false;
        // Set message and color to tell user to hold for 5 seconds
        setMessage (DIAG_BOOT_MSG1_STR);
        setMessageStyle (cControlCommon::eCssStyle::CssStyleRed);
    }
    else
    {
        // DEBUG_V("Button Released");
        if (WarningSent)
        {
            // DEBUG_V("Schedule a reboot");
            setMessage (DIAG_BOOT_MSG3_STR);
            setMessageStyle (cControlCommon::eCssStyle::CssStyleRed_bw);
            ButtonEndTime = 0;
            // give time to update the UI
            RebootTime = millis () + 1000;
        }
        else
        {
            // DEBUG_V("restore button");
            ButtonEndTime = 0;
            setMessage (emptyString);
            setMessageStyle (cControlCommon::eCssStyle::CssStyleTransparent);
        }
    }

    // DEBUG_V(String("ButtonEndTime: ") + String(ButtonEndTime));
    // DEBUG_V(String("  WarningSent: ") + String(WarningSent));
    // DEBUG_V(String("   RebootTime: ") + String(RebootTime));

    // DEBUG_END;
}

// *********************************************************************************************
void cRebootControl::Poll ()
{
    // _ DEBUG_START;

    if (ButtonEndTime)
    {
        // DEBUG_V(String("ButtonEndTime: ") + String(ButtonEndTime));
        if (millis () >= ButtonEndTime)
        {
            if (!WarningSent)
            {
                // DEBUG_V("Reboot on release");
                setMessage (DIAG_BOOT_MSG2_STR);
                setMessageStyle (cControlCommon::eCssStyle::CssStyleRed_bw);
                WarningSent = true;
            }
        }
    }

    if (RebootTime && (millis () > RebootTime))
    {
        // MUST use default serial print, not serial Log.
        Serial.println (RebootWarning);
        Serial.flush ();
        delay (1000);
        ESP.restart ();
    }

    // _ DEBUG_END;
}

// *********************************************************************************************
cRebootControl RebootControl;

// *********************************************************************************************
// OEF
