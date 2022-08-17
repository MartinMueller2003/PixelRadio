/*
   File: WiFiStatus.cpp
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
#include "memdebug.h"
#include "WiFiDriver.hpp"
#include "WiFiStatus.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String WIFI_STA_STATUS_STR = "WiFi STA/AP STATUS";

// *********************************************************************************************
cWiFiStatus::cWiFiStatus () : cControlCommon (emptyString)
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cWiFiStatus::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, ControlType::Label, color);
    ESPUI.updateControlLabel (ControlId, WIFI_STA_STATUS_STR.c_str ());
    ESPUI.updateControlValue (ControlId, F ("0"));
    ESPUI.setElementStyle (ControlId, F ("max-width: 80%;"));
    ESPUI.setPanelStyle (ControlId, F ("font-size: 1.25em;"));
    set ();

    // DEBUG_END;
}

// *********************************************************************************************
void cWiFiStatus::set ()
{
    // DEBUG_START;

    ESPUI.print (ControlId, WiFiDriver.getConnectionStatus ());

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
