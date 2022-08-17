#pragma once
/*
    WiFiDriverUi.hpp
    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
    Version: 1.1.0
    Creation: Dec-16-2021
    Revised:  Jun-13-2022
    Revision History: See PixelRadio.cpp
    Project Leader: T. Black (thomastech)
    Contributors: thomastech, MartinMueller2003

    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
 */

#include <ESPUI.h>

#include "DHCP.hpp"
#include "RssiStatus.hpp"
#include "SSID.hpp"
#include "WiFiStatus.hpp"
#include "WiFiIpStatus.hpp"
#include "WPA.hpp"

class c_WiFiDriverUi
{
public:

    c_WiFiDriverUi ();
    virtual ~c_WiFiDriverUi ();

    void        addControls (uint16_t _WiFiTabId, ControlColor Color);
    void        addHomeControls (uint16_t _WiFiTabId, ControlColor Color);

    void        CbSetApFallback (Control * sender, int type);
    void        CbSetApIpAddr (Control * sender, int type);
    void        CbSetApName (Control * sender, int type);
    void        CbSetApReboot (Control * sender, int type);
    void        CbSetMdnsName (Control * sender, int type);
    void        CbSetStaName (Control * sender, int type);
    void        CbSetUiLoginName (Control * sender, int type);
    void        CbSetUiLoginPassword (Control * sender, int type);
    void        CbSetWPAkey (Control * sender, int type);

    String&     getConnectionStatus () { return ConnectionStatusMessage; }
    IPAddress   getIpAddress() { return CurrentIpAddress; }

protected:

    void UpdateStatusFields ();

    #define AP_NAME_DEF_STR     "PixelRadioAP"
    #define STA_NAME_DEF_STR    "PixelRadio"
    #define CLIENT_TIMEOUT      500             // Webserver Client Timeout, in mS.
    #define AP_TIMEOUT          60
    #define MDNS_NAME_DEF_STR   "PixelRadio"    // Default MDNS and OTA WiFi Name.
    #ifdef OTA_ENB
    # define WIFI_MDNS_NAME_STR  "MDNS / OTA NAME"
    #else // ifdef OTA_ENB
    # define WIFI_MDNS_NAME_STR  "MDNS NAME"
    #endif // ifdef OTA_ENB

    // config_t           *config = nullptr;                           // Current configuration
    String StaHostname  = F (STA_NAME_DEF_STR);
    String ApHostname   = F (AP_NAME_DEF_STR);
    String mdnsName     = MDNS_NAME_DEF_STR;
    IPAddress ApIp      = {
        192u, 168u, 4u, 1u
    };
    IPAddress CurrentIpAddress  = IPAddress (0, 0, 0, 0);
    IPAddress CurrentNetmask    = IPAddress (0, 0, 0, 0);
    IPAddress CurrentGateway    = IPAddress (0, 0, 0, 0);
    IPAddress CurrentDns        = IPAddress (0, 0, 0, 0);
    bool ResetWiFi              = false;
    String passphrase;
    bool ap_fallbackIsEnabled           = true;
    uint32_t ap_timeout                 = AP_TIMEOUT;           ///< How long to wait in AP mode with no connection before rebooting
    uint32_t sta_timeout                = CLIENT_TIMEOUT;       ///< Timeout when connected as client (station)
    bool RebootOnWiFiFailureToConnect   = true;
    String ConnectionStatusMessage;

private:


    #define AP_NAME_MAX_SZ    F ("18")
    #define STA_NAME_MAX_SZ   F ("18")
    #define MDNS_NAME_MAX_SZ  F ("18")
    #define USER_NM_MAX_SZ    F ("10")
    #define USER_PW_MAX_SZ    F ("10")

    uint16_t HomeTabID = Control::noParent;

    uint16_t wifiStaticSettingsID = Control::noParent;

    uint16_t wifiApBootID               = Control::noParent;
    uint16_t wifiApFallID               = Control::noParent;
    uint16_t wifiApID                   = Control::noParent;
    uint16_t wifiApIpID                 = Control::noParent;
    uint16_t wifiApNameID               = Control::noParent;
    uint16_t wifiDevPwMsgID             = Control::noParent;
    uint16_t wifiDevUserID              = Control::noParent;
    uint16_t wifiDevUserMsgID           = Control::noParent;
    uint16_t wifiDevPwID                = Control::noParent;
    uint16_t wifiStaticDnsID            = Control::noParent;
    uint16_t wifiStaticSubnetID         = Control::noParent;
    uint16_t wifiStaticGatewayID        = Control::noParent;
    uint16_t wifiStaticIpID             = Control::noParent;

    uint16_t wifiMdnsNameID     = Control::noParent;
    uint16_t wifiNetID          = Control::noParent;
    uint16_t wifiStaNameID      = Control::noParent;
    uint16_t wifiWpaKeyID       = Control::noParent;

    cRssiStatus WiFiRssi;
    cRssiStatus HomeRssi;
    cWiFiStatus WiFiStatus;
    cWiFiStatus HomeStatus;
    cWiFiIpStatus WiFiIpStatus;
    cWiFiIpStatus HomeIpStatus;
};
