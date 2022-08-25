/*
   File: ApReboot.cpp
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
#include "ApReboot.hpp"
#include "memdebug.h"

static const PROGMEM char   WIFI_AP_REBOOT_STR      []  = "AP REBOOT";
static const PROGMEM char   WIFI_REBOOT_FLAG        []  = "WIFI_REBOOT_FLAG";

// *********************************************************************************************
cApReboot::cApReboot () :   cBinaryControl (WIFI_REBOOT_FLAG, WIFI_AP_REBOOT_STR, true)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cApReboot ApReboot;

// *********************************************************************************************
// OEF
