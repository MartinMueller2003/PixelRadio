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
cDHCP::cDHCP () : cOldControlCommon (WIFI_DHCP_FLAG)
{
    // _ DEBUG_START;

    // use DHCP by default
    DataValueStr        = "1";
    DataValue           = true;

    ActiveLabelStyle    = CSS_LABEL_STYLE_BLACK;
    InactiveLabelStyle  = CSS_LABEL_STYLE_BLACK;

    // _ DEBUG_END;
}

// *********************************************************************************************
cDHCP::~cDHCP ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cDHCP::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;
    String IpAddressTitle;

    IpAddressTitle.reserve (128);

    cOldControlCommon::AddControls (value, ControlType::Switcher, color);
    ESPUI.updateControlLabel (ControlId, WIFI_WEB_DHCP_STR.c_str ());
    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_BLACK);

    wifiStaticSettingsID = ESPUI.addControl (ControlType::Label, STATIC_NETWORK_SETTINGS.c_str (), emptyString, color, value);

    StaticIpAddress.AddControls (wifiStaticSettingsID, color);
    StaticNetmask.AddControls (wifiStaticSettingsID, color);
    StaticGatewayAddress.AddControls (wifiStaticSettingsID, color);
    StaticDnsAddress.AddControls (wifiStaticSettingsID, color);

    String      Dummy;
    String      NewVal;

    NewVal              = String (DataValue);
    DataValue           = !DataValue;
    DataValueStr        = String (DataValue);
    set (NewVal, Dummy);

    // DEBUG_END;
}

// *********************************************************************************************
uint32_t cDHCP::get ()
{
    // DEBUG_START;
    bool response = true;

    String Dummy;

    Dummy.reserve (128);

    do  // once
    {
        if (!ValidateStaticSettings (Dummy))
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
bool cDHCP::set (String & value, String & ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response       = true;
    uint32_t OldState   = get ();

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (!value.equals (F ("1")) && !value.equals (F ("0")))
        {
            // DEBUG_V("Un-Supported value");
            ResponseMessage = String (F ("DHCP Invalid set value: '")) + value + F ("'");
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }
        // DEBUG_V("Valid Value");
        uint32_t NewUseDhcp = value.equals (F ("1"));

        if (NewUseDhcp == DataValue)
        {
            // DEBUG_V("No change in value");
            break;
        }
        DataValue       = NewUseDhcp;
        DataValueStr    = String (DataValue);
        // DEBUG_V(String("DataValueStr: ") + DataValueStr);
        // DEBUG_V(String("   DataValue: ") + String(DataValue));

        ESPUI.updateControlValue (ControlId, DataValueStr);
        Log.infoln ((String (F ("WiFi Connection Set to: ")) + (DataValue ? F ("DHCP") : F ("Static IP"))).c_str ());

        SetStaticFieldsVisibility ();
        ValidateStaticSettings (ResponseMessage);

        if (OldState != get ())
        {
            WiFiDriver.WiFiReset ();
        }
        displaySaveWarning ();

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

    cOldControlCommon::restoreConfiguration (json);
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

    cOldControlCommon::saveConfiguration (json);
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
void cDHCP::TestIpSettings ()
{
    // DEBUG_START;

    String ResponseMessage;

    ResponseMessage.reserve (128);

    do  // once
    {
        // are we in DHCP mode?
        if (DataValue != 0)
        {
            // DEBUG_V("DHCP mode");
            ESPUI.print (StatusMessageId, emptyString);
            break;
        }

        // DEBUG_V("Static mode");
        if (!ValidateStaticSettings (ResponseMessage))
        {
            // DEBUG_V("Static mode but values are not valid");
            ESPUI.print (StatusMessageId, DHCP_LOCKED_STR);
            break;
        }
        // DEBUG_V("Static mode and values are valid");
        ESPUI.print (StatusMessageId, emptyString);
        WiFiDriver.WiFiReset ();
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
bool cDHCP::ValidateStaticSettings (String & ResponseMessage)
{
    // DEBUG_START;

    bool response = false;

    do  // once
    {
        if (DataValue != 0)
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

    if (!response && (DataValue == 0))
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
