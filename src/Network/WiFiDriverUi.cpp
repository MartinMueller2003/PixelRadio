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

extern void displaySaveWarning ();
extern const PROGMEM String WIFI_PASS_HIDE_STR;

// -----------------------------------------------------------------------------
///< Start up the driver and put it into a safe mode
c_WiFiDriverUi::c_WiFiDriverUi ()
{
}

// -----------------------------------------------------------------------------
///< deallocate any resources and put the output channels into a safe state
c_WiFiDriverUi::~c_WiFiDriverUi ()
{
    // DEBUG_START;

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::addControls (uint16_t WiFiTabID, ControlColor color)
{
    // DEBUG_START;

    uint16_t  TempID = Control::noParent;

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_STATUS_SEP_STR, emptyString, color, WiFiTabID);
    WiFiRssi.AddControls (WiFiTabID, color);
    WiFiStatus.AddControls (WiFiTabID, color);
    WiFiIpStatus.AddControls (WiFiTabID, color);

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_CRED_SEP_STR, emptyString, color, WiFiTabID);
    SSID.AddControls(WiFiTabID, color);
    WPA.AddControls(WiFiTabID, color);

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_ADDR_SEP_STR, emptyString.c_str (), ControlColor::None, WiFiTabID);
    DHCP.AddControls(WiFiTabID, color);

    // -----------------------------------------------------------------------------
    ESPUI.addControl(ControlType::Separator, WIFI_NAME_SEP_STR, emptyString, ControlColor::None, WiFiTabID);

    wifiStaNameID = ESPUI.addControl(
        ControlType::Text,
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

    wifiApNameID = ESPUI.addControl(
        ControlType::Text,
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
        wifiMdnsNameID = ESPUI.addControl (
                ControlType::Text,
                WIFI_MDNS_NAME_STR,
                mdnsName,
                ControlColor::Carrot,
                WiFiTabID,
                [] (Control * sender, int type, void * parm)
                {
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_WiFiDriverUi *> (parm)->CbSetMdnsName (sender, type);
                    }
                },
                this);
        ESPUI.  addControl (ControlType::Max, emptyString.c_str (), MDNS_NAME_MAX_SZ, ControlColor::None, wifiMdnsNameID);
        // ------------------ END OF OPTIONAL MDNS SECTION ----------------------
        #endif // ifdef MDNS_ENB

        // -----------------------------------------------------------------------------
        ESPUI.  addControl (ControlType::Separator, WIFI_DEV_CRED_SEP_STR, emptyString, ControlColor::None, WiFiTabID);

        wifiDevUserID = ESPUI.addControl (
                ControlType::Text,
                WIFI_DEV_USER_NM_STR,
                userNameStr,
                ControlColor::Carrot,
                WiFiTabID,
                [] (Control * sender, int type, void * parm)
                {
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_WiFiDriverUi *> (parm)->CbSetUiLoginName (sender, type);
                    }
                },
                this);
        ESPUI.  addControl (ControlType::Max, emptyString.c_str (), USER_NM_MAX_SZ, ControlColor::None, wifiDevUserID);
        wifiDevUserMsgID = ESPUI.addControl (
                ControlType::Label,
                emptyString.c_str (),
                F (WIFI_BLANK_MSG_STR),
                ControlColor::Alizarin,
                wifiDevUserID);
        ESPUI.setElementStyle (wifiDevUserMsgID, CSS_LABEL_STYLE_MAROON);

        wifiDevPwID = ESPUI.addControl (
                ControlType::Text,
                WIFI_DEV_USER_PW_STR,
                userPassStr,
                ControlColor::Carrot,
                WiFiTabID,
                [] (Control * sender, int type, void * parm)
                {
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_WiFiDriverUi *> (parm)->CbSetUiLoginPassword (sender, type);
                    }
                },
                this);
        ESPUI.addControl (ControlType::Max, emptyString.c_str (), USER_PW_MAX_SZ, ControlColor::None, wifiDevPwID);
        wifiDevPwMsgID = ESPUI.addControl (
                ControlType::Label,
                emptyString.c_str (),
                F (WIFI_BLANK_MSG_STR),
                ControlColor::Alizarin,
                wifiDevPwID);
        ESPUI.setElementStyle (wifiDevPwMsgID, CSS_LABEL_STYLE_MAROON);

        // -----------------------------------------------------------------------------
        ESPUI.addControl (ControlType::Separator, WIFI_AP_IP_SEP_STR, emptyString.c_str (), ControlColor::None, WiFiTabID);

        wifiApIpID = ESPUI.addControl (
                ControlType::Text,
                WIFI_AP_IP_ADDR_STR,
                ApIp.toString (),
                ControlColor::Carrot,
                WiFiTabID,
                [] (Control * sender, int type, void * parm)
                {
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_WiFiDriverUi *> (parm)->CbSetApIpAddr (sender, type);
                    }
                },
                this);

        wifiApFallID = ESPUI.addControl (
                ControlType::Switcher,
                WIFI_AP_FALLBK_STR,
                (ap_fallbackIsEnabled) ? "1" : "0",
                ControlColor::Carrot,
                WiFiTabID,
                [] (Control * sender, int type, void * parm)
                {
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_WiFiDriverUi *> (parm)->CbSetApFallback (sender, type);
                    }
                },
                this);

        // ESPUI.addControl(ControlType::Separator, WIFI_BOOT_SEP_STR, "", ControlColor::None, wifiTab);
        wifiApBootID = ESPUI.addControl (
                ControlType::Switcher,
                WIFI_AP_REBOOT_STR,
                (RebootOnWiFiFailureToConnect) ? "1" : "0",
                ControlColor::Carrot,
                WiFiTabID,
                [] (Control * sender, int type, void * parm)
                {
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_WiFiDriverUi *> (parm)->CbSetApReboot (sender, type);
                    }
                },
                this);

    UpdateStatusFields ();

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::addHomeControls (uint16_t HomeTabID, ControlColor color)
{
    // DEBUG_START;

    ESPUI.addControl (ControlType::Separator, HOME_SEP_WIFI_STR, emptyString, color, HomeTabID);
    HomeRssi.AddControls (HomeTabID, color);
    HomeStatus.AddControls (HomeTabID, color);
    HomeIpStatus.AddControls (HomeTabID, color);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApFallback (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        bool  NewApFallback = (S_ACTIVE == type);

        if (NewApFallback == ap_fallbackIsEnabled)
        {
            // DEBUG_V("No change in value");
            break;
        }
        ap_fallbackIsEnabled = NewApFallback;
        Log.infoln ((String (F ("AP Fallback Set to: ")) + (ap_fallbackIsEnabled ? F ("On") : F ("Off"))).c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApIpAddr (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        if (NewValue.equals (ApIp.toString ()))
        {
            // DEBUG_V("IP did not change");
            break;
        }
        ApIp.fromString (NewValue);
        ESPUI.updateControlValue (sender, ApIp.toString ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApName (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        if (NewValue.isEmpty ())
        {
            Log.warningln (String (F ("-> AP hostname cannot be empty.")).c_str ());
            ESPUI.updateControlValue (sender, ApHostname);
            break;
        }

        if (NewValue.equals (ApHostname))
        {
            // DEBUG_V("Name did not change");
            break;
        }
        ApHostname = NewValue;
        Log.infoln ((String (F ("Webserver (AP) Name Set to: \"")) + ApHostname + F ("\"")).c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetApReboot (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        bool  NewRebootOnWiFiFailureToConnect = (S_ACTIVE == type);

        if (NewRebootOnWiFiFailureToConnect == RebootOnWiFiFailureToConnect)
        {
            // DEBUG_V("No change in value");
            break;
        }
        RebootOnWiFiFailureToConnect = NewRebootOnWiFiFailureToConnect;
        Log.infoln ((String (F ("AP Reboot Set to: ")) + (NewRebootOnWiFiFailureToConnect ? F ("On") : F ("Off"))).c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetMdnsName (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        if (NewValue.equals (mdnsName))
        {
            // DEBUG_V("Name did not change");
            break;
        }
        mdnsName = NewValue;
        Log.infoln ((String ("MDNS/OTA Name Set to: '") + mdnsName + "'").c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetStaName (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        if (NewValue.isEmpty ())
        {
            Log.warningln (String (F ("-> STA hostname cannot be empty.")).c_str ());
            ESPUI.updateControlValue (sender, StaHostname);
            break;
        }

        if (NewValue.equals (StaHostname))
        {
            // DEBUG_V("Name did not change");
            break;
        }
        StaHostname = NewValue;
        Log.infoln ((String (F ("Webserver (STA) Name Set to: \"")) + StaHostname + F ("\"")).c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetUiLoginName (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        if (NewValue.equals (userNameStr))
        {
            // DEBUG_V("UserName did not change");
            break;
        }
        userNameStr = NewValue;
        Log.infoln ((String (F ("Device User Name Set to: '")) + userNameStr + "'").c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetUiLoginPassword (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        if (NewValue.equals (userPassStr))
        {
            // DEBUG_V("User Password did not change");
            break;
        }
        userPassStr = NewValue;
        Log.infoln ((String (F ("Device Password Set to: '")) + userPassStr + "'").c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::CbSetWPAkey (Control * sender, int type)
{
    // DEBUG_START;

    String  NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        if (NewValue.equals (passphrase))
        {
            // DEBUG_V("passphrase did not change");
            break;
        }
        passphrase = NewValue;
        ESPUI.updateControlValue (sender, WIFI_PASS_HIDE_STR);

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::UpdateStatusFields ()
{
    // DEBUG_START;

    WiFiRssi.set ();
    HomeRssi.set ();
    WiFiStatus.set ();
    HomeStatus.set ();
    WiFiIpStatus.set();
    HomeIpStatus.set();

    // DEBUG_END;
}

