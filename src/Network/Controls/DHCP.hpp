#pragma once
/*
   File: DHCP.cpp
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
#include "OldControlCommon.hpp"
#include "IpAddressUi.hpp"
#include <Arduino.h>

// *********************************************************************************************
class cDHCP : public cOldControlCommon
{
public:

    cDHCP ();
    virtual ~cDHCP ();

    void        AddControls (uint16_t TabId, ControlColor color);
    uint32_t    get ();
    bool        set (String &value, String &ResponseMessage);
    void        TestIpSettings ();

    IPAddress   GetStaticIP ();
    IPAddress   GetStaticNetmask ();
    IPAddress   GetStaticGateway ();
    IPAddress   GetStaticDNS ();

    void        restoreConfiguration (JsonObject &json);
    void        saveConfiguration (JsonObject &json);

private:

    void        SetStaticFieldsVisibility ();
    bool        ValidateStaticSettings (String &ResponseMessage);

    uint16_t wifiStaticSettingsID = Control::noParent;

    cIpAddressUi StaticIp;
    cIpAddressUi StaticNetMask;
    cIpAddressUi StaticGateWay;
    cIpAddressUi StaticDns;
};      // class cDHCP

extern cDHCP DHCP;

// *********************************************************************************************
// OEF
