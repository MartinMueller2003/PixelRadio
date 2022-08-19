/*
   File: HostnameCtrl.cpp
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

#include "HostnameCtrl.hpp"
#include "memdebug.h"
#include "WiFiDriver.hpp"

static const PROGMEM String     WIFI_WEBSRV_NAME_STR    = "WEBSERVER NAME";
static const PROGMEM String     STA_NAME_DEF_STR        = "PixelRadio";
static const PROGMEM String     STA_NAME_STR            = "STA_NAME_STR";
static const PROGMEM uint32_t STA_NAME_MAX_SZ           = 18;

// *********************************************************************************************
cHostnameCtrl::cHostnameCtrl () :   cControlCommon (STA_NAME_STR, ControlType::Text, WIFI_WEBSRV_NAME_STR, STA_NAME_DEF_STR, STA_NAME_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cHostnameCtrl::~cHostnameCtrl ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cHostnameCtrl::ResetToDefaults ()
{
    // DEBUG_START;

    String      value = STA_NAME_DEF_STR;
    String      dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cHostnameCtrl::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    DEBUG_START;
        DEBUG_V (       String ("       value: ") + value);
        DEBUG_V (       String ("DataValueStr: ") + DataValueStr);

    bool Response = cControlCommon::set (value, ResponseMessage, ForceUpdate);

    do  // once
    {
        if (!Response)
        {
            DEBUG_V ("New data is not valid");
            break;
        }

        if (value.equals (DataValueStr))
        {
            DEBUG_V ("Name did not change");
            break;
        }
        DEBUG_V ("New Name Accepted");
        WiFiDriver.WiFiReset ();
    } while (false);

    DEBUG_END;

    return Response;
}

// *********************************************************************************************
cHostnameCtrl HostnameCtrl;

// *********************************************************************************************
// OEF
