#pragma once
/*
  *     WiFiDriverUi.hpp
  *     Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *     Version: 1.1.0
  *     Creation: Dec-16-2021
  *     Revised:  Jun-13-2022
  *     Revision History: See PixelRadio.cpp
  *     Project Leader: T. Black (thomastech)
  *     Contributors: thomastech, MartinMueller2003
  *
  *     (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  */

#include <ESPUI.h>
#include "RssiStatus.hpp"
#include "WiFiIpStatus.hpp"
#include "WiFiStatus.hpp"

class c_WiFiDriverUi
{
public:

    c_WiFiDriverUi ();
    virtual~c_WiFiDriverUi ();

    void    addControls (uint16_t _WiFiTabId, ControlColor Color);
    void    addHomeControls (uint16_t _WiFiTabId, ControlColor Color);

    String      &getConnectionStatus () {return ConnectionStatusMessage;}
    IPAddress   getIpAddress ()         {return CurrentIpAddress;}

protected:

    void UpdateStatusFields ();

    #define CLIENT_TIMEOUT  500 // Webserver Client Timeout, in mS.
    #define AP_TIMEOUT      60

    IPAddress       CurrentIpAddress    = IPAddress (0, 0, 0, 0);
    IPAddress       CurrentNetmask      = IPAddress (0, 0, 0, 0);
    IPAddress       CurrentGateway      = IPAddress (0, 0, 0, 0);
    IPAddress       CurrentDns          = IPAddress (0, 0, 0, 0);
    bool            ResetWiFi           = false;
    uint32_t        ap_timeout          = AP_TIMEOUT;       ///< How long to wait in AP mode with no connection before rebooting
    uint32_t        sta_timeout         = CLIENT_TIMEOUT;   ///< Timeout when connected as client (station)
    String          ConnectionStatusMessage;

private:
    uint16_t        wifiNetID = Control::noParent;

    cRssiStatus     WiFiRssi;
    cRssiStatus     HomeRssi;
    cWiFiStatus     WiFiStatus;
    cWiFiStatus     HomeStatus;
    cWiFiIpStatus   WiFiIpStatus;
    cWiFiIpStatus   HomeIpStatus;
};  // class c_WiFiDriverUi
