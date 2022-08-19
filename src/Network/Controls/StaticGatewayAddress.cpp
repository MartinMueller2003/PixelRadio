/*
   File: StaticGatewayAddress.cpp
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
#include "StaticGatewayAddress.hpp"
#include "DHCP.hpp"
#include "memdebug.h"
static const PROGMEM String WIFI_GATEWAY_STR = "WIFI_GATEWAY_STR";

static const PROGMEM String WIFI_IP_ADDR = "Gateway IP Address";

// *********************************************************************************************
cStaticGatewayAddress::cStaticGatewayAddress () :   cIpAddressControl (WIFI_GATEWAY_STR, WIFI_IP_ADDR)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cStaticGatewayAddress::~cStaticGatewayAddress ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cStaticGatewayAddress::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    DEBUG_START;

    DEBUG_V (String ("       value: ") + value);

    bool Response = true;

    do  // once
    {
        Response = cIpAddressControl::set (value, ResponseMessage, ForceUpdate);

        if (!Booting)
        {
            DHCP.TestIpSettings ();
        }
    } while (false);

    DEBUG_V (String ("IpAddress: ") + GetIpAddress ());

    DEBUG_END;

    return Response;
}

cStaticGatewayAddress StaticGatewayAddress;

// *********************************************************************************************
// OEF
