/*
    WiFiDriverUi.cpp - Output Management class
    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
    Version: 1.1.0
    Creation: Dec-16-2021
    Revised:  Jun-13-2022
    Revision History: See PixelRadio.cpp
    Project Leader: T. Black (thomastech)
    Contributors: thomastech, MartinMueller2003

    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
*/

#include <WiFi.h>
#include <ArduinoLog.h>
#include "WiFiDriverUi.hpp"
#include "memdebug.h"
#include "language.h"
#include "PixelRadio.h"
#include "globals.h"

extern void displaySaveWarning();

//-----------------------------------------------------------------------------
///< Start up the driver and put it into a safe mode
c_WiFiDriverUi::c_WiFiDriverUi ()
{
} // c_WiFiDriverUi

//-----------------------------------------------------------------------------
///< deallocate any resources and put the output channels into a safe state
c_WiFiDriverUi::~c_WiFiDriverUi()
{
    // DEBUG_START;

    // DEBUG_END;

} // ~c_WiFiDriverUi

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::addControls(uint16_t _WiFiTabID)
{
    // DEBUG_START;

    uint16_t TempID = Control::noParent;
    do // once
    {
        if(Control::noParent != WiFiTabID)
        {
            // DEBUG_V("Cant reinit the UI");
            break;
        }
        WiFiTabID = _WiFiTabID;

        //-----------------------------------------------------------------------------
        TempID = ESPUI.addControl(ControlType::Separator, WIFI_STATUS_SEP_STR, emptyString, ControlColor::None, WiFiTabID);

        wifiStatusRssiID = ESPUI.addControl(ControlType::Label, 
                                      WIFI_RSSI_STR, 
                                      "0", 
                                      ControlColor::Carrot, 
                                      WiFiTabID);
        ESPUI.setElementStyle(wifiStatusRssiID, F("max-width: 25%;"));
        ESPUI.setPanelStyle(wifiStatusRssiID,   F("font-size: 1.25em;"));

        wifiStatusStaApID = ESPUI.addControl(ControlType::Label, 
                                     WIFI_STA_STATUS_STR, 
                                     emptyString,
                                     ControlColor::Carrot, 
                                     WiFiTabID);
        ESPUI.setElementStyle(wifiStatusStaApID, F("max-width: 80%;"));
        ESPUI.setPanelStyle(wifiStatusStaApID,   F("font-size: 1.25em;"));


        wifiStatusIpAddrID = ESPUI.addControl(ControlType::Label, 
                                        "IP_ADDR", 
                                        emptyString, 
                                        ControlColor::Carrot, 
                                        WiFiTabID);
        ESPUI.setElementStyle(wifiStatusIpAddrID, F("max-width: 45%;"));
        ESPUI.setPanelStyle(wifiStatusIpAddrID,   F("font-size: 1.25em;"));

        //-----------------------------------------------------------------------------
        TempID = ESPUI.addControl(ControlType::Separator, WIFI_CRED_SEP_STR, emptyString, ControlColor::None, WiFiTabID);

        wifiSsidID = ESPUI.addControl(  ControlType::Text, 
                                        WIFI_SSID_STR, 
                                        ssid, 
                                        ControlColor::Carrot, 
                                        WiFiTabID,
                                        [](Control *sender, int type, void *parm)
                                        {
                                            if (nullptr != parm)
                                            {
                                                reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetSSID(sender, type);
                                            }
                                        },
                                        this);
        TempID = ESPUI.addControl(ControlType::Max, emptyString.c_str(), SSID_MAX_SZ, ControlColor::None, wifiSsidID);

        wifiWpaKeyID = ESPUI.addControl(    ControlType::Text,
                                            WIFI_WPA_KEY_STR, 
                                            F(WIFI_PASS_HIDE_STR), 
                                            ControlColor::Carrot, 
                                            WiFiTabID,
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetWPAkey(sender, type);
                                                }
                                            },
                                            this);
        TempID = ESPUI.addControl(ControlType::Max, emptyString.c_str(), PASSPHRASE_MAX_SZ, ControlColor::None, wifiWpaKeyID);

        //-----------------------------------------------------------------------------
        TempID = ESPUI.addControl(ControlType::Separator, WIFI_ADDR_SEP_STR, emptyString.c_str(), ControlColor::None, WiFiTabID);

        wifiDhcpID = ESPUI.addControl(  ControlType::Switcher,
                                        WIFI_WEB_DHCP_STR, 
                                        UseDhcp ? "1" : "0",
                                        ControlColor::Carrot,
                                        WiFiTabID,
                                        [](Control *sender, int type, void *parm)
                                        {
                                            if (nullptr != parm)
                                            {
                                                reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetDhcp(sender, type);
                                            }
                                        },
                                        this);
        wifiDhcpMsgID = ESPUI.addControl(ControlType::Label, emptyString.c_str(), emptyString, ControlColor::Carrot, wifiDhcpID);
        ESPUI.setElementStyle(wifiDhcpMsgID, CSS_LABEL_STYLE_BLACK);

        wifiStaticSettingsID = ESPUI.addControl(ControlType::Label, "STATIC NETWORK SETTINGS", emptyString, ControlColor::Carrot, WiFiTabID);

        TempID = ESPUI.addControl(ControlType::Label, emptyString.c_str(), F("IP Address"), ControlColor::Carrot, wifiStaticSettingsID);
        ESPUI.setElementStyle(TempID, CSS_LABEL_STYLE_BLACK);
        wifiStaticIpID = ESPUI.addControl(  ControlType::Text, 
                                            "", 
                                            staticIp.toString(), 
                                            ControlColor::Carrot, 
                                            wifiStaticSettingsID,
                                            [](Control *sender, int type, void *parm)
                                        {
                                            if (nullptr != parm)
                                            {
                                                reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetStaticIp(sender, type);
                                            }
                                        },
                                            this);

        TempID = ESPUI.addControl(ControlType::Label, emptyString.c_str(), F("Subnet Mask"), ControlColor::Carrot, wifiStaticSettingsID);
        ESPUI.setElementStyle(TempID, CSS_LABEL_STYLE_BLACK);
        wifiStaticSubnetID = ESPUI.addControl(  ControlType::Text,
                                                WIFI_SUBNET_STR,
                                                staticNetmask.toString(),
                                                ControlColor::Carrot,
                                                wifiStaticSettingsID,
                                                [](Control *sender, int type, void *parm)
                                                {
                                                    if (nullptr != parm)
                                                    {
                                                        reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetStaticNetmask(sender, type);
                                                    }
                                                },
                                                this);

        TempID = ESPUI.addControl(ControlType::Label, emptyString.c_str(), F("Gateway IP Address"), ControlColor::Carrot, wifiStaticSettingsID);
        ESPUI.setElementStyle(TempID, CSS_LABEL_STYLE_BLACK);
        wifiStaticGatewayID = ESPUI.addControl( ControlType::Text, 
                                                WIFI_GATEWAY_STR, 
                                                staticGateway.toString(), 
                                                ControlColor::Carrot, 
                                                wifiStaticSettingsID,
                                                [](Control *sender, int type, void *parm)
                                                {
                                                    if (nullptr != parm)
                                                    {
                                                        reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetStaticGateway(sender, type);
                                                    }
                                                },
                                                this);

        TempID = ESPUI.addControl(ControlType::Label, emptyString.c_str(), F("DNS IP Address"), ControlColor::Carrot, wifiStaticSettingsID);
        ESPUI.setElementStyle(TempID, CSS_LABEL_STYLE_BLACK);
        wifiStaticDnsID = ESPUI.addControl( ControlType::Text,
                                            WIFI_DNS_STR,
                                            staticDnsIp.toString(),
                                            ControlColor::Carrot,
                                            wifiStaticSettingsID,
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetDns(sender, type);
                                                }
                                            },
                                            this);

        //-----------------------------------------------------------------------------
        ESPUI.addControl(ControlType::Separator, WIFI_NAME_SEP_STR, emptyString, ControlColor::None, WiFiTabID);

        wifiStaNameID = ESPUI.addControl(   ControlType::Text, 
                                            WIFI_WEBSRV_NAME_STR, 
                                            StaHostname, 
                                            ControlColor::Carrot, 
                                            WiFiTabID, 
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetStaName(sender, type);
                                                }
                                            },
                                            this);
        ESPUI.addControl(ControlType::Max, emptyString.c_str(), STA_NAME_MAX_SZ, ControlColor::None, wifiStaNameID);

        wifiApNameID = ESPUI.addControl(    ControlType::Text, 
                                            WIFI_AP_NAME_STR, 
                                            ApHostname, 
                                            ControlColor::Carrot, 
                                            WiFiTabID, 
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetApName(sender, type);
                                                }
                                            },
                                            this);
        ESPUI.addControl(ControlType::Max, emptyString.c_str(), AP_NAME_MAX_SZ, ControlColor::None, wifiApNameID);

        #ifdef MDNS_ENB
        // ------------------ START OF OPTIONAL MDNS SECTION ----------------------
        wifiMdnsNameID = ESPUI.addControl(  ControlType::Text, 
                                            WIFI_MDNS_NAME_STR, 
                                            mdnsName, 
                                            ControlColor::Carrot, 
                                            WiFiTabID, 
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetMdnsName(sender, type);
                                                }
                                            },
                                            this);
        ESPUI.addControl(ControlType::Max, emptyString.c_str(), MDNS_NAME_MAX_SZ, ControlColor::None, wifiMdnsNameID);
        // ------------------ END OF OPTIONAL MDNS SECTION ----------------------
        #endif // ifdef MDNS_ENB

        //-----------------------------------------------------------------------------
        ESPUI.addControl(ControlType::Separator, WIFI_DEV_CRED_SEP_STR, emptyString, ControlColor::None, WiFiTabID);

        wifiDevUserID = ESPUI.addControl(   ControlType::Text,
                                            WIFI_DEV_USER_NM_STR,
                                            userNameStr,
                                            ControlColor::Carrot,
                                            WiFiTabID,
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetUiLoginName(sender, type);
                                                }
                                            },
                                            this);
        ESPUI.addControl(ControlType::Max, emptyString.c_str(), USER_NM_MAX_SZ, ControlColor::None, wifiDevUserID);
        wifiDevUserMsgID = ESPUI.addControl(ControlType::Label, emptyString.c_str(), F(WIFI_BLANK_MSG_STR), ControlColor::Alizarin, wifiDevUserID);
        ESPUI.setElementStyle(wifiDevUserMsgID, CSS_LABEL_STYLE_MAROON);

        wifiDevPwID = ESPUI.addControl( ControlType::Text,
                                        WIFI_DEV_USER_PW_STR,
                                        userPassStr,
                                        ControlColor::Carrot,
                                        WiFiTabID,
                                        [](Control *sender, int type, void *parm)
                                        {
                                            if (nullptr != parm)
                                            {
                                                reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetUiLoginPassword(sender, type);
                                            }
                                        },
                                        this);
        ESPUI.addControl(ControlType::Max, emptyString.c_str(), USER_PW_MAX_SZ, ControlColor::None, wifiDevPwID);
        wifiDevPwMsgID = ESPUI.addControl(ControlType::Label, emptyString.c_str(), F(WIFI_BLANK_MSG_STR), ControlColor::Alizarin, wifiDevPwID);
        ESPUI.setElementStyle(wifiDevPwMsgID,   CSS_LABEL_STYLE_MAROON);

        //-----------------------------------------------------------------------------
        ESPUI.addControl(ControlType::Separator, WIFI_AP_IP_SEP_STR, emptyString.c_str(), ControlColor::None, WiFiTabID);

        wifiApIpID = ESPUI.addControl(  ControlType::Text, 
                                        WIFI_AP_IP_ADDR_STR, 
                                        ApIp.toString(), 
                                        ControlColor::Carrot, 
                                        WiFiTabID, 
                                        [](Control *sender, int type, void *parm)
                                        {
                                            if (nullptr != parm)
                                            {
                                                reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetApIpAddr(sender, type);
                                            }
                                        },
                                        this);

        wifiApFallID = ESPUI.addControl(    ControlType::Switcher, 
                                            WIFI_AP_FALLBK_STR, 
                                            (ap_fallbackIsEnabled) ? "1" : "0", 
                                            ControlColor::Carrot, 
                                            WiFiTabID,
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetApFallback(sender, type);
                                                }
                                            },
                                            this);

        // ESPUI.addControl(ControlType::Separator, WIFI_BOOT_SEP_STR, "", ControlColor::None, wifiTab);
        wifiApBootID = ESPUI.addControl(    ControlType::Switcher,
                                            WIFI_AP_REBOOT_STR,
                                            (RebootOnWiFiFailureToConnect) ? "1" : "0",
                                            ControlColor::Carrot,
                                            WiFiTabID,
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetApReboot(sender, type);
                                                }
                                            },
                                            this);
    } while (false);

    ValidateStaticSettings();
    SetStaticFieldsVisibility();
    UpdateStatusFields();

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::addHomeControls(uint16_t _HomeTabID)
{
    // DEBUG_START;

    do // once
    {
        if(HomeTabID == _HomeTabID)
        {
            // DEBUG_V("Already created the UI controls");
            break;
        }
        HomeTabID = _HomeTabID;
        // DEBUG_V(String("HomeTabID: ") + String(HomeTabID));

        ESPUI.addControl(ControlType::Separator, HOME_SEP_WIFI_STR, emptyString, ControlColor::None, HomeTabID);
        // DEBUG_V();

        homeRssiID = ESPUI.addControl(ControlType::Label, HOME_WIFI_STR, emptyString, ControlColor::Peterriver, HomeTabID);
        ESPUI.setPanelStyle(homeRssiID,         "font-size: 1.25em;");
        ESPUI.setElementStyle(homeRssiID,       "max-width: 30%;");

        homeStaID = ESPUI.addControl(ControlType::Label, HOME_STA_STR, emptyString, ControlColor::Peterriver, HomeTabID);
        ESPUI.setPanelStyle(homeStaID,          "font-size: 1.15em;");
        ESPUI.setElementStyle(homeStaID,        "max-width: 65%;");

        homeStaMsgID = ESPUI.addControl(ControlType::Label, "IP_ADDR", emptyString, ControlColor::Peterriver, HomeTabID);
        ESPUI.setElementStyle(homeStaMsgID, F("max-width: 45%;"));
        ESPUI.setPanelStyle(homeStaMsgID,   F("font-size: 1.25em;"));

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApFallback(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        bool NewApFallback = (S_ACTIVE == type);
        if(NewApFallback == ap_fallbackIsEnabled)
        {
            // DEBUG_V("No change in value");
            break;
        }

        ap_fallbackIsEnabled = NewApFallback;
        Log.infoln((String(F("AP Fallback Set to: ")) + (ap_fallbackIsEnabled ? F("On") : F("Off"))).c_str());
        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApIpAddr(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(ApIp.toString()))
        {
            // DEBUG_V("IP did not change");
            break;
        }

        ApIp.fromString(NewValue);
        ESPUI.updateControlValue(sender, ApIp.toString());

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApName(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.isEmpty())
        {
            Log.warningln(String(F("-> AP hostname cannot be empty.")).c_str());
            ESPUI.updateControlValue(sender, ApHostname);
            break;
        }

        if(NewValue.equals(ApHostname))
        {
            // DEBUG_V("Name did not change");
            break;
        }

        ApHostname = NewValue;
        Log.infoln((String(F("Webserver (AP) Name Set to: \"")) + ApHostname + F("\"")).c_str());
        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApReboot(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        bool NewRebootOnWiFiFailureToConnect = (S_ACTIVE == type);
        if(NewRebootOnWiFiFailureToConnect == RebootOnWiFiFailureToConnect)
        {
            // DEBUG_V("No change in value");
            break;
        }

        RebootOnWiFiFailureToConnect = NewRebootOnWiFiFailureToConnect;
        Log.infoln((String(F("AP Reboot Set to: ")) + (NewRebootOnWiFiFailureToConnect ? F("On") : F("Off"))).c_str());
        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetDhcp(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        bool NewUseDhcp = (S_ACTIVE == type);
        if(NewUseDhcp == UseDhcp)
        {
            // DEBUG_V("No change in value");
            break;
        }

        UseDhcp = NewUseDhcp;
        Log.infoln((String(F("WiFi Connection Set to: ")) + (UseDhcp ? F("DHCP") : F("Static IP"))).c_str());

        SetStaticFieldsVisibility();
        ValidateStaticSettings();

        displaySaveWarning();
        
        // For some reason the initial load of the UI does not work properly
        ESPUI.jsonDom(0);

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetDns(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(staticDnsIp.toString()))
        {
            // DEBUG_V("DNS did not change");
            break;
        }

        staticDnsIp.fromString(NewValue);
        ESPUI.updateControlValue(sender, staticDnsIp.toString());
        ValidateStaticSettings();

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetMdnsName(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(mdnsName))
        {
            // DEBUG_V("Name did not change");
            break;
        }

        mdnsName = NewValue;
        Log.infoln((String("MDNS/OTA Name Set to: '") + mdnsName + "'").c_str());

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetSSID(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(ssid))
        {
            // DEBUG_V("Name did not change");
            break;
        }

        ssid = NewValue;
        Log.infoln((String(F("WiFi SSID Set to: \"")) + ssid + "\"").c_str());
        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaticGateway(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(staticGateway.toString()))
        {
            // DEBUG_V("Gateway did not change");
            break;
        }

        staticGateway.fromString(NewValue);
        ESPUI.updateControlValue(sender, staticGateway.toString());
        ValidateStaticSettings();

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaticIp(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(staticIp.toString()))
        {
            // DEBUG_V("IP did not change");
            break;
        }

        staticIp.fromString(NewValue);
        ESPUI.updateControlValue(sender, staticIp.toString());
        ValidateStaticSettings();

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaticNetmask(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(staticNetmask.toString()))
        {
            // DEBUG_V("IP did not change");
            break;
        }

        staticNetmask.fromString(NewValue);
        ESPUI.updateControlValue(sender, staticNetmask.toString());

        ValidateStaticSettings();

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaName(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.isEmpty())
        {
            Log.warningln(String(F("-> STA hostname cannot be empty.")).c_str());
            ESPUI.updateControlValue(sender, StaHostname);
            break;
        }

        if(NewValue.equals(StaHostname))
        {
            // DEBUG_V("Name did not change");
            break;
        }

        StaHostname = NewValue;
        Log.infoln((String(F("Webserver (STA) Name Set to: \"")) + StaHostname + F("\"")).c_str());
        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetUiLoginName(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(userNameStr))
        {
            // DEBUG_V("UserName did not change");
            break;
        }

        userNameStr = NewValue;
        Log.infoln((String(F("Device User Name Set to: '")) + userNameStr + "'").c_str());

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetUiLoginPassword(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(userPassStr))
        {
            // DEBUG_V("User Password did not change");
            break;
        }

        userPassStr = NewValue;
        Log.infoln((String(F("Device Password Set to: '")) + userPassStr + "'").c_str());

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetWPAkey(Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;
    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(passphrase))
        {
            // DEBUG_V("passphrase did not change");
            break;
        }

        passphrase = NewValue;
        ESPUI.updateControlValue(sender, F(WIFI_PASS_HIDE_STR));

        displaySaveWarning();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
// getRSSI(): Get the RSSI value.
//            Note: AP Mode will always return 0 since it doesn't receive a sgnal from a router.
int8_t c_WiFiDriverUi::getRSSI(void)
{
    int16_t rssi;

    rssi = WiFi.RSSI();

    if (rssi < -127) {
        rssi = -127;
    }
    else if (rssi > 20) {
        rssi = 20;
    }

    return int8_t(rssi);
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::SetStaticFieldsVisibility()
{
    // DEBUG_START;

    ESPUI.updateVisibility(wifiStaticSettingsID,      !UseDhcp);
//    ESPUI.updateVisibility(wifiStaticSubnetID,  !UseDhcp);
//    ESPUI.updateVisibility(wifiStaticGatewayID, !UseDhcp);
//    ESPUI.updateVisibility(wifiStaticDnsID,     !UseDhcp);

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::UpdateStatusFields()
{
    // DEBUG_START;

    ESPUI.updateControlValue(wifiStatusRssiID,   String(getRSSI()) + UNITS_DBM_STR);
    ESPUI.updateControlValue(wifiStatusStaApID,  ConnectionStatusMessage);
    ESPUI.updateControlValue(wifiStatusIpAddrID, String(CurrentIpAddress.toString()));

    ESPUI.updateControlValue(homeRssiID,   String(getRSSI()) + UNITS_DBM_STR);
    ESPUI.updateControlValue(homeStaID,    ConnectionStatusMessage);
    ESPUI.updateControlValue(homeStaMsgID, CurrentIpAddress.toString());

    // DEBUG_END;
}

//-----------------------------------------------------------------------------
bool c_WiFiDriverUi::ValidateStaticSettings()
{
    // DEBUG_START;

    bool response = false;

    do // once
    {
        if(UseDhcp)
        {
            // DEBUG_V("Dont check the rest of the settings if they dont matter");
            break;
        }

        if (staticIp == IPAddress((uint32_t)0))
        {
            Log.warningln("WiFi: Static IP Address is empty. Using DHCP.");
            break;
        }

        if (staticNetmask == IPAddress((uint32_t)0))
        {
            Log.warningln("WiFi: Static Netmask is empty. Using DHCP.");
            break;
        }

        if (staticGateway == IPAddress((uint32_t)0))
        {
            Log.warningln("WiFi: Static Gateway Address is empty. Using DHCP.");
            break;
        }

        if (staticDnsIp == IPAddress((uint32_t)0))
        {
            Log.warningln("WiFi: Static DNS IP Address is empty. Using DHCP.");
            break;
        }

        // DEBUG_V("All settings are valid. We can use Static settings");
        response = true;

    } while (false);

    if(response || UseDhcp)
    {
        ESPUI.print(wifiDhcpMsgID, emptyString);
    }
    else
    {
        ESPUI.print(wifiDhcpMsgID, String(F(DHCP_LOCKED_STR)));
    }

    // DEBUG_END;
    return response;
}
