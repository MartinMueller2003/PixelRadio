/*
  *    File: SystemRunTime.cpp
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

#include "SystemRunTime.hpp"
#include "memdebug.h"

const String PROGMEM DIAG_RUN_TIME_STR = "UPTIME";

// *********************************************************************************************
cSystemRunTime::cSystemRunTime () :   cStatusControl (DIAG_RUN_TIME_STR, emptyString)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cSystemRunTime::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cStatusControl::AddControls (TabId, color);
    setControlPanelStyle (ePanelStyle::PanelStyle135_black);

    // DEBUG_END;
}

// *********************************************************************************************
void cSystemRunTime::Poll ()
{
    // _ DEBUG_START;

    uint32_t Now = millis ();

    if (Now >= NextReadingTimeMs)
    {
        // forward one second
        NextReadingTimeMs += MeasurementIntervalMs;

        uint8_t seconds = (Now / 1000) % 60;
        uint8_t minutes = (Now / (60 * 1000)) % 60;
        uint8_t hours   = (Now / (60 * 60 * 1000)) % 24;
        int16_t days    = (Now / (24 * 60 * 60 * 1000));

        char timeBuff[30];
        memset (timeBuff, 0x0, sizeof (timeBuff));
        sprintf (timeBuff, "Days:%u + %02u:%02u:%02u", days, hours, minutes, seconds);
        set (String (timeBuff), true, false);
    }

    // _ DEBUG_END;
}

cSystemRunTime SystemRunTime;

// *********************************************************************************************
// OEF
