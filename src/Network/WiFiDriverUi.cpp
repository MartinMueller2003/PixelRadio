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
#include "WpaKey.hpp"
#include "ApFallback.hpp"
#include "ApIpAddress.hpp"
#include "ApReboot.hpp"
#include "language.h"
#include "memdebug.h"
#include "PixelRadio.h"
#include "WiFiDriverUi.hpp"
#include <ArduinoLog.h>
#include <WiFi.h>

extern void displaySaveWarning ();
static const PROGMEM char WIFI_PASS_HIDE_STR [] = "PASSWORD HIDDEN";

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
    WpaKey.AddControls (WiFiTabID, color);

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
    ApFallback.AddControls (WiFiTabID, color);
    ApReboot.AddControls (WiFiTabID, color);

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
