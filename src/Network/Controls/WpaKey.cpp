/*
   File: WpaKey.cpp
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

#include "WpaKey.hpp"
#include "WiFiDriver.hpp"
#include "memdebug.h"

static const PROGMEM char   WIFI_WPA_KEY            []  = "WIFI WPA KEY";
static const PROGMEM char   WIFI_WPA_KEY_STR        []  = "WIFI WPA KEY";
static const PROGMEM uint32_t PASSPHRASE_MAX_SZ         = 48;

// *********************************************************************************************
cWpaKey::cWpaKey () :   cPasswordControl (WIFI_WPA_KEY_STR,
                                          WIFI_WPA_KEY,
                                          WiFiDriver.GetDefaultWpaKey (),
                                          PASSPHRASE_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cWpaKey::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);

    bool Response = cPasswordControl::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        WiFiDriver.WiFiReset ();
    }
    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cWpaKey WpaKey;

// *********************************************************************************************
// OEF
