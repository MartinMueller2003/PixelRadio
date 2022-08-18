/*
   File: SSID.cpp
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
#include "memdebug.h"
#include "PtyCode.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "SSID.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String WIFI_SSID_STR       = "WIFI SSID";
static const PROGMEM uint32_t SSID_MAX_SZ       = 32;

// *********************************************************************************************
cSSID::cSSID () : cOldControlCommon ("WIFI_SSID_STR")
{
    // _ DEBUG_START;

    DataValueStr.reserve (SSID_MAX_SZ + 2);
    DataValueStr = WiFiDriver.GetDefaultSsid ();

    // _ DEBUG_END;
}

// *********************************************************************************************
void cSSID::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Text, color);
    ESPUI.updateControlLabel (ControlId, WIFI_SSID_STR.c_str ());
    ESPUI.addControl (ControlType::Max, emptyString.c_str (), String (SSID_MAX_SZ), ControlColor::None, ControlId);

    // DEBUG_END;
}

// *********************************************************************************************
void cSSID::ResetToDefaults ()
{
    // DEBUG_START;

    String      value = WiFiDriver.GetDefaultSsid ();
    String      dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cSSID::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (value.length () > SSID_MAX_SZ)
        {
            ResponseMessage     = String (F ("New Value is too long: \""))  + value + String (F ("\""));
            Response            = false;
            break;
        }

        if (value.equals (DataValueStr))
        {
            // DEBUG_V("Name did not change");
            Log.infoln (String (F ("WiFi SSID Unchanged")).c_str ());
            break;
        }
        DataValueStr = value;

        ESPUI.updateControlValue (ControlId, DataValueStr);
        WiFiDriver.WiFiReset ();
        Log.infoln ((String (F ("WiFi SSID Set to: ")) + DataValueStr).c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cSSID SSID;

// *********************************************************************************************
// OEF
