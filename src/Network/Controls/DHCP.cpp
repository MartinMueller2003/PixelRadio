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
#include <ArduinoLog.h>
#include "DHCP.hpp"
#include "WiFiDriver.hpp"
#include "memdebug.h"

static const PROGMEM String     STATIC_NETWORK_SETTINGS = "STATIC NETWORK SETTINGS";
static const PROGMEM String     WIFI_DHCP_FLAG = "WIFI_DHCP_FLAG";
static const PROGMEM String     WIFI_WEB_DHCP_STR = "WEBSERVER DHCP";
static const PROGMEM String     DHCP_LOCKED_STR = "INCOMPLETE STATIC WIFI SETTINGS.<br>DHCP settings will be used.";
static const PROGMEM String     WIFI_IP_ADDR_STR = "WIFI_IP_ADDR_STR";
static const PROGMEM String     WIFI_SUBNET_STR = "WIFI_SUBNET_STR";
static const PROGMEM String     WIFI_GATEWAY_STR = "WIFI_GATEWAY_STR";
static const PROGMEM String     WIFI_DNS_STR = "WIFI_DNS_STR";

// *********************************************************************************************
cDHCP::cDHCP () : cControlCommon (WIFI_DHCP_FLAG)
{
    // _ DEBUG_START;

    // use DHCP by default
    DataValueStr        = "1";
    DataValue           = true;

    ActiveLabelStyle    = CSS_LABEL_STYLE_BLACK;
    InactiveLabelStyle  = CSS_LABEL_STYLE_BLACK;

    StaticIp.SetConfigName(WIFI_IP_ADDR_STR);
    StaticNetMask.SetConfigName(WIFI_SUBNET_STR);
    StaticGateWay.SetConfigName(WIFI_GATEWAY_STR);
    StaticDns.SetConfigName(WIFI_DNS_STR);

    // _ DEBUG_END;
}

// *********************************************************************************************
void cDHCP::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;
    String IpAddressTitle;
    IpAddressTitle.reserve(128);

    cControlCommon::AddControls (value, ControlType::Switcher, color);
    ESPUI.updateControlLabel (ControlId, WIFI_WEB_DHCP_STR.c_str ());
    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_BLACK);

    wifiStaticSettingsID = ESPUI.addControl (ControlType::Label, STATIC_NETWORK_SETTINGS.c_str(), emptyString, color, value);

    IpAddressTitle = F("IP Address");
    StaticIp.AddControls(wifiStaticSettingsID, color, IpAddressTitle);
    IpAddressTitle = F("Subnet Mask");
    StaticNetMask.AddControls(wifiStaticSettingsID, color, IpAddressTitle);
    IpAddressTitle = F("Gateway IP Address");
    StaticGateWay.AddControls(wifiStaticSettingsID, color, IpAddressTitle);
    IpAddressTitle = F("DNS IP Address");
    StaticDns.AddControls(wifiStaticSettingsID, color, IpAddressTitle);

    String Dummy;
    String NewVal;
    NewVal = String(DataValue);
    DataValue = !DataValue;
    DataValueStr = String(DataValue);
    set(NewVal, Dummy);

    // DEBUG_END;
}

// *********************************************************************************************
uint32_t cDHCP::get()
{
    // DEBUG_START;
    bool response = true;

    String Dummy;
    Dummy.reserve(128);

    do // once
    {
        if(!ValidateStaticSettings(Dummy))
        {
            // DEBUG_V("IP Settings wont support static settings");
            break;
        }

        response = (0 != DataValue);

    } while (false);

    // DEBUG_END;
    return response;
}

// *********************************************************************************************
bool cDHCP::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool  Response = true;
    uint32_t OldState = get();

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if(!value.equals(F("1")) && !value.equals(F("0")))
        {
            // DEBUG_V("Un-Supported value");
            ResponseMessage = String(F("DHCP Invalid set value: '")) + value + F("'");
            Log.infoln(ResponseMessage.c_str());
            Response = false;
            break;
        }

        // DEBUG_V("Valid Value");
        uint32_t NewUseDhcp = value.equals(F("1"));

        if (NewUseDhcp == DataValue)
        {
            // DEBUG_V("No change in value");
            break;
        }

        DataValue = NewUseDhcp;
        DataValueStr = String(DataValue);
        // DEBUG_V(String("DataValueStr: ") + DataValueStr);
        // DEBUG_V(String("   DataValue: ") + String(DataValue));

        ESPUI.updateControlValue(ControlId, DataValueStr);
        Log.infoln((String(F("WiFi Connection Set to: ")) + (DataValue ? F("DHCP") : F("Static IP"))).c_str());

        SetStaticFieldsVisibility ();
        ValidateStaticSettings(ResponseMessage);
        if(OldState != get())
        {
            WiFiDriver.WiFiReset();
        }

        displaySaveWarning ();

        // For some reason the initial load of the UI does not work properly
        ESPUI.jsonDom (0);

    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cDHCP::restoreConfiguration (JsonObject &json)
{
    // DEBUG_START;

    cControlCommon::restoreConfiguration(json);
    StaticIp.restoreConfiguration(json);
    StaticNetMask.restoreConfiguration(json);
    StaticGateWay.restoreConfiguration(json);
    StaticDns.restoreConfiguration(json);

    // DEBUG_END;
}

// *********************************************************************************************
void cDHCP::saveConfiguration (JsonObject &json)
{
    // DEBUG_START;

    cControlCommon::saveConfiguration(json);
    StaticIp.saveConfiguration(json);
    StaticNetMask.saveConfiguration(json);
    StaticGateWay.saveConfiguration(json);
    StaticDns.saveConfiguration(json);

    // DEBUG_END;
}

// *********************************************************************************************
void cDHCP::SetStaticFieldsVisibility ()
{
    // DEBUG_START;

    ESPUI.updateVisibility (wifiStaticSettingsID, DataValue == 0);

    // DEBUG_END;
}

// *********************************************************************************************
void cDHCP::TestIpSettings()
{
    // DEBUG_START;

    String ResponseMessage;
    ResponseMessage.reserve(128);

    do // once
    {
        // are we in DHCP mode?
        if(DataValue != 0)
        {
            // DEBUG_V("DHCP mode");
            ESPUI.print(StatusMessageId, emptyString);
            break;
        }
        // DEBUG_V("Static mode");
        if(!ValidateStaticSettings(ResponseMessage))
        {
            // DEBUG_V("Static mode but values are not valid");
            ESPUI.print(StatusMessageId, DHCP_LOCKED_STR);
            break;
        }

        // DEBUG_V("Static mode and values are valid");
        ESPUI.print(StatusMessageId, emptyString);
        WiFiDriver.WiFiReset();

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
bool cDHCP::ValidateStaticSettings (String & ResponseMessage)
{
    // DEBUG_START;

    bool  response = false;

    do  // once
    {
        if (DataValue != 0)
        {
            // DEBUG_V("Dont check the rest of the settings if they dont matter");
            response = true;
            break;
        }

        if (StaticIp.GetIpAddress() == IPAddress ((uint32_t)0))
        {
            Log.warningln ("WiFi: Static IP Address is empty. Using DHCP.");
            break;
        }

        if (StaticNetMask.GetIpAddress() == IPAddress ((uint32_t)0))
        {
            Log.warningln ("WiFi: Static Netmask is empty. Using DHCP.");
            break;
        }

        if (StaticGateWay.GetIpAddress() == IPAddress ((uint32_t)0))
        {
            Log.warningln ("WiFi: Static Gateway Address is empty. Using DHCP.");
            break;
        }

        if (StaticDns.GetIpAddress() == IPAddress ((uint32_t)0))
        {
            Log.warningln ("WiFi: Static DNS IP Address is empty. Using DHCP.");
            break;
        }
        // DEBUG_V("All settings are valid. We can use Static settings");
        response = true;
    } while (false);

    if (!response && (DataValue == 0))
    {
        ResponseMessage = DHCP_LOCKED_STR;
    }

    // DEBUG_END;
    return response;
}

// *********************************************************************************************
cDHCP  DHCP;

// *********************************************************************************************
// OEF
