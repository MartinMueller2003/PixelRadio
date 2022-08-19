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
#include "StaticIpAddress.hpp"
#include "StaticGatewayAddress.hpp"
#include "StaticNetmask.hpp"
#include "StaticDnsAddress.hpp"
#include "memdebug.h"

static const PROGMEM String     STATIC_NETWORK_SETTINGS = "STATIC NETWORK SETTINGS";
static const PROGMEM String     WIFI_DHCP_FLAG          = "WIFI_DHCP_FLAG";
static const PROGMEM String     WIFI_WEB_DHCP_STR       = "WEBSERVER DHCP";
static const PROGMEM String     DHCP_LOCKED_STR         = "INCOMPLETE STATIC WIFI SETTINGS.<br>DHCP settings will be used.";

// *********************************************************************************************
cDHCP::cDHCP () :   cBinaryControl (WIFI_DHCP_FLAG, WIFI_WEB_DHCP_STR, true)
{
    // _ DEBUG_START;

    // use DHCP by default

    // _ DEBUG_END;
}

// *********************************************************************************************
cDHCP::~cDHCP ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cDHCP::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cBinaryControl::AddControls (TabId, color);

    wifiStaticSettingsID = ESPUI.addControl (ControlType::Label, STATIC_NETWORK_SETTINGS.c_str (), emptyString, color, TabId);

    StaticIpAddress.AddControls (wifiStaticSettingsID, color);
    StaticNetmask.AddControls (wifiStaticSettingsID, color);
    StaticGatewayAddress.AddControls (wifiStaticSettingsID, color);
    StaticDnsAddress.AddControls (wifiStaticSettingsID, color);

    String      Dummy;
    String      NewVal;
    NewVal = String (DataValue);
    set (NewVal, Dummy, true);

    // DEBUG_END;
}

// *********************************************************************************************
bool cDHCP::getBool ()
{
    // DEBUG_START;
    bool response = true;       // using DHCP

    String Dummy;

    Dummy.reserve (128);

    do  // once
    {
        if (!ValidateStaticSettings (Dummy))
        {
            // DEBUG_V("IP Settings wont support static settings");
            break;
        }
        response = DataValue;
    } while (false);

    // DEBUG_END;
    return response;
}

// *********************************************************************************************
bool cDHCP::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V (String ("OldDataValue: ") + String (DataValue));

    bool        OldState        = getBool ();
    bool        OldDataValue    = DataValue;
    bool        Response        = cBinaryControl::set (value, ResponseMessage, ForceUpdate);
    // DEBUG_V (       String ("   DataValue: ") + String (DataValue));
    // DEBUG_V (       String ("    OldState: ") + String (OldState));
    // DEBUG_V (       String ("    NewState: ") + String (getBool ()));

    do  // once
    {
        if (!Response)
        {
            // DEBUG_V ("New Value is not valid");
            break;
        }

        if (!ForceUpdate && (OldDataValue == DataValue))
        {
            // DEBUG_V ("No change in value");
            break;
        }
        SetStaticFieldsVisibility ();
        SetControlMessage (ResponseMessage);

        if (OldState != getBool ())
        {
            WiFiDriver.WiFiReset ();
        }
        // For some reason the initial load of the UI does not work properly
        ESPUI.jsonDom (0);
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cDHCP::restoreConfiguration (JsonObject & json)
{
    // DEBUG_START;

    cBinaryControl::restoreConfiguration (json);
    StaticIpAddress.restoreConfiguration (json);
    StaticNetmask.restoreConfiguration (json);
    StaticGatewayAddress.restoreConfiguration (json);
    StaticDnsAddress.restoreConfiguration (json);

    // DEBUG_END;
}

// *********************************************************************************************
void cDHCP::saveConfiguration (JsonObject & json)
{
    // DEBUG_START;

    cBinaryControl::saveConfiguration (json);
    StaticIpAddress.saveConfiguration (json);
    StaticNetmask.saveConfiguration (json);
    StaticGatewayAddress.saveConfiguration (json);
    StaticDnsAddress.saveConfiguration (json);

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
void cDHCP::SetControlMessage (String & ResponseMessage)
{
    // DEBUG_START;

    do  // once
    {
        // are we in DHCP mode?
        if (DataValue)
        {
            // DEBUG_V ("DHCP mode");
            // setMessage (emptyString, eCssStyle::CssStyleTransparent);
            break;
        }
        // DEBUG_V ("Static mode");

        if (!ValidateStaticSettings (ResponseMessage))
        {
            // DEBUG_V ("Static mode but values are not valid");
            setMessage (DHCP_LOCKED_STR, eCssStyle::CssStyleBlack);

            break;
        }
        // DEBUG_V ("Static mode and values are valid");
        setMessage (F ("Using Static Settings"), eCssStyle::CssStyleTransparent);
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cDHCP::TestIpSettings ()
{
    // DEBUG_START;

    String ResponseMessage;
    ResponseMessage.reserve (128);
    SetControlMessage (ResponseMessage);

    // DEBUG_END;
}

// *********************************************************************************************
bool cDHCP::ValidateStaticSettings (String & ResponseMessage)
{
    // DEBUG_START;

    bool response = false;

    do  // once
    {
        if (DataValue)
        {
            // DEBUG_V("Dont check the rest of the settings if they dont matter");
            response = true;
            break;
        }

        if (StaticIpAddress.GetIpAddress () == IPAddress ((uint32_t)0))
        {
            Log.warningln (String (F ("WiFi: Static IP Address is empty. Using DHCP.")).c_str ());
            break;
        }

        if (StaticNetmask.GetIpAddress () == IPAddress ((uint32_t)0))
        {
            Log.warningln (String (F ("WiFi: Static Netmask is empty. Using DHCP.")).c_str ());
            break;
        }

        if (StaticGatewayAddress.GetIpAddress () == IPAddress ((uint32_t)0))
        {
            Log.warningln (String (F ("WiFi: Static Gateway Address is empty. Using DHCP.")).c_str ());
            break;
        }

        if (StaticDnsAddress.GetIpAddress () == IPAddress ((uint32_t)0))
        {
            Log.warningln (String (F ("WiFi: Static DNS IP Address is empty. Using DHCP.")).c_str ());
            break;
        }
        // DEBUG_V("All settings are valid. We can use Static settings");
        response = true;
    } while (false);

    if (!response && (!DataValue))
    {
        ResponseMessage = DHCP_LOCKED_STR;
    }
    // DEBUG_END;
    return response;
}

// *********************************************************************************************
cDHCP DHCP;

// *********************************************************************************************
// OEF
