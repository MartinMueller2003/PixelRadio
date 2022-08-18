/*
    WiFiDriverUi.cpp - Output Management class
    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
    Version: 1.1.0
    Creation: Dec-16-2021
    Revised:  Jun-13-2022
    Revision History: See PixelRadio.cpp
    Project Leader: T.Black (thomastech)
    Contributors: thomastech, MartinMueller2003

    (c) copyright T.Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
 */

#include "globals.h"
#include "DHCP.hpp"
#include "HostnameCtrl.hpp"
#include "HotspotName.hpp"
#include "LoginPassword.hpp"
#include "LoginUser.hpp"
#include "MdnsName.hpp"
#include "SSID.hpp"
#include "WPA.hpp"
#include "ApIpAddress.hpp"
#include "language.h"
#include "memdebug.h"
#include "PixelRadio.h"
#include "WiFiDriverUi.hpp"
#include <ArduinoLog.h>
#include <WiFi.h>

extern void displaySaveWarning ();
extern const PROGMEM String WIFI_PASS_HIDE_STR;

// -----------------------------------------------------------------------------
///< Start up the driver and put it into a safe mode
c_WiFiDriverUi::c_WiFiDriverUi ()
{}

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

    uint16_t TempID = Control::noParent;

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_STATUS_SEP_STR, emptyString, color, WiFiTabID);
    WiFiRssi.AddControls (WiFiTabID, color);
    WiFiStatus.AddControls (WiFiTabID, color);
    WiFiIpStatus.AddControls (WiFiTabID, color);

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_CRED_SEP_STR, emptyString, color, WiFiTabID);
    SSID.AddControls (WiFiTabID, color);
    WPA.AddControls (WiFiTabID, color);

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_ADDR_SEP_STR, emptyString.c_str (), ControlColor::None, WiFiTabID);
    DHCP.AddControls (WiFiTabID, color);

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_NAME_SEP_STR, emptyString, ControlColor::None, WiFiTabID);
    HostnameCtrl.AddControls (WiFiTabID, color);
    HotspotName.AddControls (WiFiTabID, color);

#ifdef MDNS_ENB
        // ------------------ START OF OPTIONAL MDNS SECTION ----------------------
        MdnsName.AddControls (WiFiTabID, color);
        // ------------------ END OF OPTIONAL MDNS SECTION ----------------------
#endif // ifdef MDNS_ENB

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_DEV_CRED_SEP_STR, emptyString, ControlColor::None, WiFiTabID);
    LoginUser.AddControls (WiFiTabID, color);
    LoginPassword.AddControls (WiFiTabID, color);

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_AP_IP_SEP_STR, emptyString.c_str (), ControlColor::None, WiFiTabID);
    ApIpAddress.AddControls (WiFiTabID, color);

    wifiApFallID = ESPUI.addControl (ControlType::Switcher,
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
    wifiApBootID = ESPUI.addControl (ControlType::Switcher,
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

    String NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        bool NewApFallback = (S_ACTIVE == type);

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
void c_WiFiDriverUi::CbSetApReboot (Control * sender, int type)
{
    // DEBUG_START;

    String NewValue = sender->value;

    // DEBUG_V(String("Sender ID: ") + String(sender->id));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String(" NewValue: ") + String(NewValue));

    do  // once
    {
        bool NewRebootOnWiFiFailureToConnect = (S_ACTIVE == type);

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
void c_WiFiDriverUi::UpdateStatusFields ()
{
    // DEBUG_START;

    WiFiRssi.set ();
    HomeRssi.set ();
    WiFiStatus.set ();
    HomeStatus.set ();
    WiFiIpStatus.set ();
    HomeIpStatus.set ();

    // DEBUG_END;
}
