/*
   File: HotspotName.cpp
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

#include "HotspotName.hpp"
#include "WiFiDriver.hpp"
#include "memdebug.h"

static const PROGMEM String     WIFI_AP_NAME_STR        = "AP (HOTSPOT) NAME";
static const PROGMEM String     AP_NAME_STR             = "AP_NAME_STR";
static const PROGMEM String     AP_NAME_DEF_STR         = "PixelRadioAP";
static const PROGMEM uint32_t AP_NAME_MAX_SZ            = 18;

// *********************************************************************************************
cHotspotName::cHotspotName () :   cControlCommon (AP_NAME_STR, ControlType::Text, WIFI_AP_NAME_STR, AP_NAME_DEF_STR, AP_NAME_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cHotspotName::~cHotspotName ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cHotspotName HotspotName;

// *********************************************************************************************
// OEF
