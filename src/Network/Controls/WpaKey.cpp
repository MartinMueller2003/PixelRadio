/*
   File: WpaKey.cpp
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
#include "WpaKey.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

const PROGMEM String WIFI_PASS_HIDE_STR         = "{PASSWORD HIDDEN}";
static const PROGMEM String WIFI_WPA_KEY_STR    = "WIFI WPA KEY";
static const PROGMEM uint32_t PASSPHRASE_MAX_SZ = 48;

// *********************************************************************************************
cWpaKey::cWpaKey () : cControlCommon ("WIFI_WPA_KEY_STR")
{
    // _ DEBUG_START;
    DataValueStr.reserve (PASSPHRASE_MAX_SZ + 2);
    DataValueStr = WiFiDriver.GetDefaultWpaKey ();
    // _ DEBUG_END;
}

// *********************************************************************************************
void cWpaKey::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, ControlType::Text, color);
    ESPUI.updateControlLabel (ControlId, WIFI_WPA_KEY_STR.c_str ());
    ESPUI.updateControlValue (ControlId, WIFI_PASS_HIDE_STR);
    ESPUI.addControl (ControlType::Max, emptyString.c_str (), String (PASSPHRASE_MAX_SZ), ControlColor::None, ControlId);

    // DEBUG_END;
}

// *********************************************************************************************
void cWpaKey::ResetToDefaults ()
{
    // DEBUG_START;

    String      value = WiFiDriver.GetDefaultWpaKey ();
    String      dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cWpaKey::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;
    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (value.length () > PASSPHRASE_MAX_SZ)
        {
            ResponseMessage     = String (F ("New WPA Passphrase Value is too long: '"))  + value + String (F ("'"));
            Response            = false;
            break;
        }

        if (value.equals (DataValueStr))
        {
            // DEBUG_V("Name did not change");
            break;
        }
        DataValueStr = value;
        Log.infoln ((String (F ("WiFi WPA Key ")) + ResponseMessage).c_str ());

        WiFiDriver.WiFiReset ();

        displaySaveWarning ();
    } while (false);

    ESPUI.updateControlValue (ControlId, WIFI_PASS_HIDE_STR);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cWpaKey WpaKey;

// *********************************************************************************************
// OEF
