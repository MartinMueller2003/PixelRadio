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
#include <Arduino.h>
#include "ControlCommon.hpp"
#include "IpAddressUi.hpp"

// *********************************************************************************************
class cDHCP : public cControlCommon
{
public:

    cDHCP ();
    virtual ~cDHCP () {}

    void        AddControls (uint16_t TabId, ControlColor color);
    uint32_t    get();
    bool        set (String &value, String &ResponseMessage);
    void        TestIpSettings();

    IPAddress GetStaticIP() { return StaticIp.GetIpAddress(); }
    IPAddress GetStaticNetmask() { return StaticNetMask.GetIpAddress(); }
    IPAddress GetStaticGateway() { return StaticGateWay.GetIpAddress(); }
    IPAddress GetStaticDNS() { return StaticDns.GetIpAddress(); }

    void        restoreConfiguration (JsonObject &json);
    void        saveConfiguration (JsonObject &json);

  private:
    void        SetStaticFieldsVisibility ();
    bool        ValidateStaticSettings (String & ResponseMessage);

    uint16_t  wifiStaticSettingsID  = Control::noParent;
    
  cIpAddressUi StaticIp;
  cIpAddressUi StaticNetMask;
  cIpAddressUi StaticGateWay;
  cIpAddressUi StaticDns;
  
};

extern cDHCP  DHCP;

// *********************************************************************************************
// OEF
