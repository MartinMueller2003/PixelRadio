/*
  *    File: ApIpAddress.cpp
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
#include "ApIpAddress.hpp"
#include "memdebug.h"

static const PROGMEM char   WIFI_AP_IP_ADDR_STR     []  = "AP IP Address";
static const PROGMEM char   AP_IP_ADDR_STR          []  = "AP_IP_ADDR_STR";

// *********************************************************************************************
cApIpAddress::cApIpAddress () :   cIpAddressControl (AP_IP_ADDR_STR, WIFI_AP_IP_ADDR_STR, IPAddress (192, 168, 4, 1))
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cApIpAddress::~cApIpAddress ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cApIpAddress ApIpAddress;

// *********************************************************************************************
// OEF
