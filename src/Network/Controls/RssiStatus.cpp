/*
   File: RssiStatus.cpp
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
#include "memdebug.h"
#include "RssiStatus.hpp"

static const PROGMEM char Name [] = "WIFI RSSI";

// *********************************************************************************************
cRssiStatus::cRssiStatus () :   cStatusControl (Name)
{
    // _ DEBUG_START;

    ControlStyle = cControlCommon::eCssStyle::CssStyleWhite;

    // _ DEBUG_END;
}

// *********************************************************************************************
cRssiStatus::~cRssiStatus ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
int32_t cRssiStatus::getRssi ()
{
    // DEBUG_START;

    int32_t rssi;

    rssi = int32_t (WiFi.RSSI ());

    if (rssi < -127)
    {
        rssi = -127;
    }
    else if (rssi > 20)
    {
        rssi = 20;
    }
    // DEBUG_END;

    return rssi;
}

// *********************************************************************************************
void cRssiStatus::set ()
{
    // DEBUG_START;

    String Result;

    Result.reserve (128);

    Result      = String (getRssi ());
    Result      += UNITS_DBM_STR;
    // DEBUG_V (String ("Result: ") + Result);
    cStatusControl::set (Result);

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
