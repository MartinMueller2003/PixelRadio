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
#include "../memdebug.h"
#include "../language.h"
#include "../PixelRadio.h"
#include "../globals.h"

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
void c_WiFiDriverUi::addControls(uint16_t _HomeTabId, uint16_t _WiFiTabID)
{
    DEBUG_START;

    do // once
    {
        if(Control::noParent != HomeTabID)
        {
            DEBUG_V("Cant reinit the UI");
            break;
        }

        HomeTabID = _HomeTabId;
        WiFiTabID = _WiFiTabID;

        ESPUI.addControl(ControlType::Separator, WIFI_STATUS_SEP_STR, emptyString, ControlColor::None, WiFiTabID);

#ifdef OldWay
        tempStr    = getRSSI();
        tempStr   += UNITS_DBM_STR;
#endif // def OldWay

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

        ESPUI.addControl(ControlType::Separator, WIFI_CRED_SEP_STR, emptyString, ControlColor::None, WiFiTabID);

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

        ESPUI.addControl(ControlType::Separator, WIFI_ADDR_SEP_STR, "", ControlColor::None, WiFiTabID);

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
        wifiDhcpMsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", emptyString, ControlColor::Carrot, wifiDhcpID);
        ESPUI.setElementStyle(wifiDhcpMsgID, CSS_LABEL_STYLE_BLACK);

        wifiStaticIpID = ESPUI.addControl(    ControlType::Text, 
                                        WIFI_IP_ADDR_STR, 
                                        staticIp.toString(), 
                                        ControlColor::Carrot, 
                                        WiFiTabID,
                                        [](Control *sender, int type, void *parm)
                                        {
                                            if (nullptr != parm)
                                            {
                                                reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetStaticIp(sender, type);
                                            }
                                        },
                                        this);

        wifiStaticGatewayID = ESPUI.addControl(   ControlType::Text, 
                                            WIFI_GATEWAY_STR, 
                                            staticGateway.toString(), 
                                            ControlColor::Carrot, 
                                            WiFiTabID,
                                            [](Control *sender, int type, void *parm)
                                            {
                                                if (nullptr != parm)
                                                {
                                                    reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetStaticGateway(sender, type);
                                                }
                                            },
                                            this);

        wifiStaticSubnetID = ESPUI.addControl(   ControlType::Text,
                                        WIFI_SUBNET_STR,
                                        staticNetmask.toString(),
                                        ControlColor::Carrot,
                                        WiFiTabID,
                                        [](Control *sender, int type, void *parm)
                                        {
                                            if (nullptr != parm)
                                            {
                                                reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetStaticNetmask(sender, type);
                                            }
                                        },
                                        this);

        wifiStaticDnsID = ESPUI.addControl(   ControlType::Text,
                                        WIFI_DNS_STR,
                                        staticDnsIp.toString(),
                                        ControlColor::Carrot,
                                        WiFiTabID,
                                        [](Control *sender, int type, void *parm)
                                        {
                                            if (nullptr != parm)
                                            {
                                                reinterpret_cast<c_WiFiDriverUi *>(parm)->CbSetDns(sender, type);
                                            }
                                        },
                                        this);

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
    wifiDevUserMsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", F("WIFI_BLANK_MSG_STR"), ControlColor::Alizarin, wifiDevUserID);


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
    wifiDevPwMsgID = ESPUI.addControl(ControlType::Label, "MSG_AREA", WIFI_BLANK_MSG_STR, ControlColor::Alizarin, wifiDevPwID);

#ifdef OldWay
        ESPUI.setPanelStyle(homeRssiID,         "font-size: 1.25em;");
        ESPUI.setPanelStyle(homeStaID,          "font-size: 1.15em;");
        ESPUI.setPanelStyle(homeTextMsgID,      "font-size: 1.15em;");
        ESPUI.setElementStyle(homeRssiID,       "max-width: 30%;");
        ESPUI.setElementStyle(homeStaID,        "max-width: 65%;");
        ESPUI.setElementStyle(homeStaMsgID,     CSS_LABEL_STYLE_BLACK);
        ESPUI.setElementStyle(homeTextMsgID,    CSS_LABEL_STYLE_WHITE);

        ESPUI.setElementStyle(wifiDevUserMsgID, CSS_LABEL_STYLE_MAROON);
        ESPUI.setElementStyle(wifiDevPwMsgID,   CSS_LABEL_STYLE_MAROON);
        ESPUI.setElementStyle(wifiSaveMsgID,    CSS_LABEL_STYLE_MAROON);
#endif // def OldWay

        SetStaticFieldsVisibility();

    } while (false);

    ValidateStaticSettings();

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApName(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(StaHostname))
        {
            DEBUG_V("Name did not change");
            break;
        }

        StaHostname = NewValue;

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetDhcp(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        bool NewUseDhcp = (S_ACTIVE == type);
        if(NewUseDhcp == UseDhcp)
        {
            DEBUG_V("No change in value");
            break;
        }

        UseDhcp = NewUseDhcp;

        SetStaticFieldsVisibility();

        displaySaveWarning();

        // For some reason the initial load of the UI does not work properly
        ESPUI.jsonDom(0);

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetDns(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(staticDnsIp.toString()))
        {
            DEBUG_V("DNS did not change");
            break;
        }

        staticDnsIp.fromString(NewValue);
        ESPUI.updateControlValue(sender, staticDnsIp.toString());
        ValidateStaticSettings();

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetSSID(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(ssid))
        {
            DEBUG_V("Name did not change");
            break;
        }

        ssid = NewValue;

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaticGateway(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(staticGateway.toString()))
        {
            DEBUG_V("Gateway did not change");
            break;
        }

        staticGateway.fromString(NewValue);
        ESPUI.updateControlValue(sender, staticGateway.toString());
        ValidateStaticSettings();

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaticIp(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(staticIp.toString()))
        {
            DEBUG_V("IP did not change");
            break;
        }

        staticIp.fromString(NewValue);
        ESPUI.updateControlValue(sender, staticIp.toString());
        ValidateStaticSettings();

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaticNetmask(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(staticNetmask.toString()))
        {
            DEBUG_V("IP did not change");
            break;
        }

        staticNetmask.fromString(NewValue);
        ESPUI.updateControlValue(sender, staticNetmask.toString());

        ValidateStaticSettings();

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaName(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(StaHostname))
        {
            DEBUG_V("Name did not change");
            break;
        }

        StaHostname = NewValue;

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetUiLoginName(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(userNameStr))
        {
            DEBUG_V("UserName did not change");
            break;
        }

        userNameStr = NewValue;
        Log.infoln((String(F("Device User Name Set to: '")) + userNameStr + "'").c_str());

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetUiLoginPassword(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(userPassStr))
        {
            DEBUG_V("User Password did not change");
            break;
        }

        userPassStr = NewValue;
        Log.infoln((String(F("Device Password Set to: '")) + userPassStr + "'").c_str());

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetWPAkey(Control * sender, int type)
{
    DEBUG_START;

    String NewValue = sender->value;
    DEBUG_V(String("Sender ID: ") + String(sender->id));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String(" NewValue: ") + String(NewValue));

    do // once
    {
        if(NewValue.equals(passphrase))
        {
            DEBUG_V("passphrase did not change");
            break;
        }

        passphrase = NewValue;
        ESPUI.updateControlValue(sender, F(WIFI_PASS_HIDE_STR));

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::SetStaticFieldsVisibility()
{
    DEBUG_START;

    ESPUI.updateVisibility(wifiStaticIpID,      !UseDhcp);
    ESPUI.updateVisibility(wifiStaticSubnetID,  !UseDhcp);
    ESPUI.updateVisibility(wifiStaticGatewayID, !UseDhcp);
    ESPUI.updateVisibility(wifiStaticDnsID,     !UseDhcp);

    DEBUG_END;
}

//-----------------------------------------------------------------------------
void c_WiFiDriverUi::UpdateStatusFields()
{
    DEBUG_START;

    ESPUI.updateControlValue(wifiStatusRssiID,   String(WiFi.RSSI()));
    ESPUI.updateControlValue(wifiStatusStaApID,  ConnectionStatusMessage);
    ESPUI.updateControlValue(wifiStatusIpAddrID, String(CurrentIpAddress.toString()));

    DEBUG_END;
}

//-----------------------------------------------------------------------------
bool c_WiFiDriverUi::ValidateStaticSettings()
{
    DEBUG_START;

    bool response = false;

    do // once
    {
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

        response = true;

    } while (false);

    if(response)
    {
        ESPUI.print(wifiDhcpMsgID, emptyString);
    }
    else
    {
        ESPUI.print(wifiDhcpMsgID, String(F(DHCP_LOCKED_STR)));
    }

    DEBUG_END;
    return response;
}
