#pragma once
/*
   File: WiFiIpStatus.cpp
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
#include "ControlCommon.hpp"
#include <Arduino.h>

// *********************************************************************************************
class cWiFiIpStatus : public cControlCommon
{
public:

    cWiFiIpStatus ();
    virtual ~cWiFiIpStatus ()    {}

    void        restoreConfiguration (JsonObject &)     {}
    void        saveConfiguration (JsonObject &)        {}
    void        AddControls (uint16_t Tab, ControlColor color);
    bool        set (String &, String &)                {return true;}
    void        set ();
};      // class cWiFiIpStatus

// *********************************************************************************************
// OEF
