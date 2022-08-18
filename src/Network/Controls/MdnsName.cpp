/*
   File: MdnsName.cpp
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
#include "MdnsName.hpp"
#include "WiFiDriver.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String MDNS_NAME_DEF_STR   = "PixelRadio";
static const PROGMEM uint32_t MDNS_NAME_MAX_SZ  = 18;
#ifdef OTA_ENB
    static const PROGMEM String WIFI_MDNS_NAME_STR = "MDNS / OTA NAME";
#else // ifdef OTA_ENB
    static const PROGMEM String WIFI_MDNS_NAME_STR = "MDNS NAME";
#endif // ifdef OTA_ENB

// *********************************************************************************************
cMdnsName::cMdnsName () : cOldControlCommon ("MDNS_NAME_STR")
{
    // _ DEBUG_START;

    DataValueStr.reserve (MDNS_NAME_MAX_SZ + 2);
    DataValueStr = MDNS_NAME_DEF_STR;

    // _ DEBUG_END;
}

// *********************************************************************************************
cMdnsName::~cMdnsName ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cMdnsName::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Text, color);
    ESPUI.updateControlLabel (ControlId, WIFI_MDNS_NAME_STR.c_str ());
    ESPUI.addControl (ControlType::Max, emptyString.c_str (), String (MDNS_NAME_MAX_SZ), ControlColor::None, ControlId);

    // DEBUG_END;
}

// *********************************************************************************************
void cMdnsName::ResetToDefaults ()
{
    // DEBUG_START;

    String      value = MDNS_NAME_DEF_STR;
    String      dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cMdnsName::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (value.length () > MDNS_NAME_MAX_SZ)
        {
            ResponseMessage     = String (F ("MDNS ? OTA New Value is too long: \""))  + value + String (F ("\""));
            Response            = false;
            break;
        }

        if (value.equals (DataValueStr))
        {
            // DEBUG_V("Name did not change");
            Log.infoln (String (F ("MDNS / OTA Name Unchanged")).c_str ());
            break;
        }
        DataValueStr = value;

        ESPUI.updateControlValue (ControlId, DataValueStr);
        WiFiDriver.WiFiReset ();
        Log.infoln ((String (F ("MDNS / OTA Name Set to: ")) + DataValueStr).c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cMdnsName MdnsName;

// *********************************************************************************************
// OEF
