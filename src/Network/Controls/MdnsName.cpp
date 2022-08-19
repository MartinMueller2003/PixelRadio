/*
   File: MdnsName.cpp
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
#include "MdnsName.hpp"
#include "WiFiDriver.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String     MDNS_NAME_DEF_STR       = "PixelRadio";
static const PROGMEM String     MDNS_NAME_STR           = "MDNS_NAME_STR";
static const PROGMEM uint32_t MDNS_NAME_MAX_SZ          = 18;
#ifdef OTA_ENB
    static const PROGMEM String WIFI_MDNS_NAME_STR = "MDNS / OTA NAME";
#else // ifdef OTA_ENB
    static const PROGMEM String WIFI_MDNS_NAME_STR = "MDNS NAME";
#endif // ifdef OTA_ENB

// *********************************************************************************************
cMdnsName::cMdnsName () :   cControlCommon (MDNS_NAME_STR,
                                            ControlType::Text,
                                            WIFI_MDNS_NAME_STR,
                                            MDNS_NAME_DEF_STR,
                                            MDNS_NAME_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMdnsName::~cMdnsName ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cMdnsName::set (const String & value, String & ResponseMessage, bool ForceUpdate)
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
cMdnsName MdnsName;

// *********************************************************************************************
// OEF
