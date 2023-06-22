/*
  *     WiFiDriverUi.cpp - Output Management class
  *     Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *     Version: 1.1.0
  *     Creation: Dec-16-2021
  *     Revised:  Jun-13-2022
  *     Revision History: See PixelRadio.cpp
  *     Project Leader: T.Black (thomastech)
  *     Contributors: thomastech, MartinMueller2003
  *
  *     (c) copyright T.Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  */

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
    StatusGroup.SetTitle(WIFI_STATUS_SEP_STR);
    StatusGroup.AddControls (WiFiTabID, color);
    WiFiRssi.AddControls (StatusGroup.ControlId, color);
    WiFiStatus.AddControls (StatusGroup.ControlId, color);
    WiFiIpStatus.AddControls (StatusGroup.ControlId, color);

    // -----------------------------------------------------------------------------
    CredGroup.SetTitle(WIFI_CRED_SEP_STR);
    CredGroup.AddControls (WiFiTabID, color);
    SSID.AddControls (CredGroup.ControlId, color);
    WpaKey.AddControls (CredGroup.ControlId, color);

    // -----------------------------------------------------------------------------
    ESPUI.addControl (ControlType::Separator, WIFI_ADDR_SEP_STR, emptyString.c_str (), ControlColor::None, WiFiTabID);
    DHCP.AddControls (WiFiTabID, color);

    // -----------------------------------------------------------------------------
    NameGroup.SetTitle(WIFI_NAME_SEP_STR);
    NameGroup.AddControls (WiFiTabID, color);
    HostnameCtrl.AddControls (NameGroup.ControlId, color);
    HotspotName.AddControls (NameGroup.ControlId, color);

    #ifdef MDNS_ENB
        // ------------------ START OF OPTIONAL MDNS SECTION ----------------------
        MdnsName.AddControls (NameGroup.ControlId, color);
        // ------------------ END OF OPTIONAL MDNS SECTION ----------------------
    #endif // ifdef MDNS_ENB

    // -----------------------------------------------------------------------------
    LoginGroup.SetTitle(WIFI_AP_IP_SEP_STR);
    LoginGroup.AddControls (WiFiTabID, color);
    LoginUser.AddControls (LoginGroup.ControlId, color);
    LoginPassword.AddControls (LoginGroup.ControlId, color);

    // -----------------------------------------------------------------------------
    ApControlGroup.SetTitle(WIFI_AP_IP_SEP_STR);
    ApControlGroup.AddControls (WiFiTabID, color);
    ApIpAddress.AddControls (ApControlGroup.ControlId, color);
    ApFallback.AddControls (ApControlGroup.ControlId, color);
    ApReboot.AddControls (ApControlGroup.ControlId, color);

    UpdateStatusFields ();

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriverUi::addHomeControls (uint16_t HomeTabID, ControlColor color)
{
    // DEBUG_START;

    HomeStatusGroup.SetTitle(HOME_SEP_WIFI_STR);
    HomeStatusGroup.AddControls (HomeTabID, color);
    HomeRssi.AddControls (HomeStatusGroup.ControlId, color);
    HomeStatus.AddControls (HomeStatusGroup.ControlId, color);
    HomeIpStatus.AddControls (HomeStatusGroup.ControlId, color);

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
