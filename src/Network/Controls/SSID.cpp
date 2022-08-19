/*
   File: SSID.cpp
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
#include "PtyCode.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "SSID.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String     WIFI_SSID       = "WIFI SSID";
static const PROGMEM String     WIFI_SSID_STR   = "WIFI_SSID_STR";
static const PROGMEM uint32_t SSID_MAX_SZ       = 32;

// *********************************************************************************************
cSSID::cSSID () :   cControlCommon (WIFI_SSID_STR,
                                    ControlType::Text,
                                    WIFI_SSID,
                                    WiFiDriver.GetDefaultSsid (),
                                    SSID_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cSSID::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);

    bool Response = cControlCommon::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        WiFiDriver.WiFiReset ();
    }
    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cSSID SSID;

// *********************************************************************************************
// OEF
