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

class c_WiFiDriverUi
{
public:
                c_WiFiDriverUi ();
    virtual     ~c_WiFiDriverUi ();

    void        addControls(uint16_t _WiFiTabId);
    void        addHomeControls(uint16_t _WiFiTabId);

    void        CbSetApFallback(Control * sender, int type);
    void        CbSetApIpAddr(Control * sender, int type);
    void        CbSetApName(Control * sender, int type);
    void        CbSetApReboot(Control * sender, int type);
    void        CbSetDhcp(Control * sender, int type);
    void        CbSetDns(Control * sender, int type);
    void        CbSetMdnsName(Control * sender, int type);
    void        CbSetSSID(Control * sender, int type);
    void        CbSetStaName(Control * sender, int type);
    void        CbSetStaticGateway(Control * sender, int type);
    void        CbSetStaticIp(Control * sender, int type);
    void        CbSetStaticNetmask(Control * sender, int type);
    void        CbSetUiLoginName(Control * sender, int type);
    void        CbSetUiLoginPassword(Control * sender, int type);
    void        CbSetWPAkey(Control * sender, int type);

    bool        ValidateStaticSettings();

protected:
    void        UpdateStatusFields();
    int8_t      getRSSI         ();

    #define AP_NAME_DEF_STR     "PixelRadioAP"
    #define STA_NAME_DEF_STR    "PixelRadio"
    #define CLIENT_TIMEOUT      500   // Webserver Client Timeout, in mS.
    #define AP_TIMEOUT          60
    #define MDNS_NAME_DEF_STR   "PixelRadio" // Default MDNS and OTA WiFi Name.
    #ifdef OTA_ENB
    #define WIFI_MDNS_NAME_STR  "MDNS / OTA NAME"
    #else // ifdef OTA_ENB
    #define WIFI_MDNS_NAME_STR  "MDNS NAME"
    #endif // ifdef OTA_ENB

    // config_t           *config = nullptr;                           // Current configuration
    String      StaHostname             = F(STA_NAME_DEF_STR);
    String      ApHostname              = F(AP_NAME_DEF_STR);
    String      mdnsName                = MDNS_NAME_DEF_STR;
    IPAddress   ApIp                    = { 192u, 168u, 4u, 1u };
    IPAddress   CurrentIpAddress        = IPAddress (0, 0, 0, 0);
    IPAddress   CurrentNetmask          = IPAddress (0, 0, 0, 0);
    IPAddress   CurrentGateway          = IPAddress (0, 0, 0, 0);
    IPAddress   CurrentDns              = IPAddress (0, 0, 0, 0);
    bool        ResetWiFi               = false;
    String      ssid;
    String      passphrase;
    IPAddress   staticIp                = IPAddress ((uint32_t)0);
    IPAddress   staticNetmask           = IPAddress ((uint32_t)0);
    IPAddress   staticGateway           = IPAddress ((uint32_t)0);
    IPAddress   staticDnsIp             = IPAddress ((uint32_t)0);
    bool        UseDhcp                 = true;
    bool        ap_fallbackIsEnabled    = true;
    uint32_t    ap_timeout              = AP_TIMEOUT;      ///< How long to wait in AP mode with no connection before rebooting
    uint32_t    sta_timeout             = CLIENT_TIMEOUT;  ///< Timeout when connected as client (station)
    bool        RebootOnWiFiFailureToConnect = true;
    String      ConnectionStatusMessage;

private:
    void        SetStaticFieldsVisibility();

    #define AP_NAME_MAX_SZ    F("18")
    #define STA_NAME_MAX_SZ   F("18")
    #define MDNS_NAME_MAX_SZ  F("18")
    #define SSID_MAX_SZ       F("32")   // Maximum permitted SSID Size according to standards.
    #define USER_NM_MAX_SZ    F("10")
    #define USER_PW_MAX_SZ    F("10")
    #define PASSPHRASE_MAX_SZ F("48")

    uint16_t    HomeTabID               = Control::noParent;
    uint16_t    WiFiTabID               = Control::noParent;

    uint16_t    wifiStaticSettingsID    = Control::noParent;
    uint16_t    wifiStatusRssiID        = Control::noParent;
    uint16_t    wifiStatusStaApID       = Control::noParent;
    uint16_t    wifiStatusIpAddrID      = Control::noParent;

    uint16_t    wifiApBootID            = Control::noParent;
    uint16_t    wifiApFallID            = Control::noParent;
    uint16_t    wifiApID                = Control::noParent;
    uint16_t    wifiApIpID              = Control::noParent;
    uint16_t    wifiApNameID            = Control::noParent;
    uint16_t    wifiDevPwMsgID          = Control::noParent;
    uint16_t    wifiDevUserID           = Control::noParent;
    uint16_t    wifiDevUserMsgID        = Control::noParent;
    uint16_t    wifiDevPwID             = Control::noParent;
    uint16_t    wifiDhcpID              = Control::noParent;
    uint16_t    wifiDhcpMsgID           = Control::noParent;
    uint16_t    wifiStaticDnsID         = Control::noParent;
    uint16_t    wifiStaticSubnetID      = Control::noParent;
    uint16_t    wifiStaticGatewayID     = Control::noParent;
    uint16_t    wifiStaticIpID          = Control::noParent;

    uint16_t    wifiMdnsNameID          = Control::noParent;
    uint16_t    wifiNetID               = Control::noParent;
    uint16_t    wifiSsidID              = Control::noParent;
    uint16_t    wifiStaNameID           = Control::noParent;
    uint16_t    wifiWpaKeyID            = Control::noParent;

    uint16_t    homeRssiID              = Control::noParent;
    uint16_t    homeStaID               = Control::noParent;
    uint16_t    homeStaMsgID            = Control::noParent;

}; // c_WiFiDriverUi
