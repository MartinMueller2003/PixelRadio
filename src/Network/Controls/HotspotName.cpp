/*
   File: HotspotName.cpp
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
#include "HotspotName.hpp"
#include "memdebug.h"
#include "WiFiDriver.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String     WIFI_AP_NAME_STR        = "AP (HOTSPOT) NAME";
static const PROGMEM String     AP_NAME_DEF_STR         = "PixelRadioAP";
static const PROGMEM uint32_t AP_NAME_MAX_SZ            = 18;

// *********************************************************************************************
cHotspotName::cHotspotName () : cOldControlCommon ("AP_NAME_STR")
{
    // _ DEBUG_START;

    DataValueStr.reserve (AP_NAME_MAX_SZ + 2);
    DataValueStr = AP_NAME_DEF_STR;

    // _ DEBUG_END;
}

// *********************************************************************************************
cHotspotName::~cHotspotName ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cHotspotName::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Text, color);
    ESPUI.updateControlLabel (ControlId, WIFI_AP_NAME_STR.c_str ());
    ESPUI.addControl (ControlType::Max, emptyString.c_str (), String (AP_NAME_MAX_SZ), ControlColor::None, ControlId);

    // DEBUG_END;
}

// *********************************************************************************************
void cHotspotName::ResetToDefaults ()
{
    // DEBUG_START;

    String      value = AP_NAME_DEF_STR;
    String      dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cHotspotName::set (String & value, String & ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V ( String ("       value: ") + value);
    // DEBUG_V ( String ("DataValueStr: ") + DataValueStr);

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (value.isEmpty ())
        {
            ResponseMessage = String (F ("-> AP Hotspot cannot be empty."));
            Log.warningln (ResponseMessage.c_str ());
            ESPUI.updateControlValue (ControlId, DataValueStr);
            break;
        }

        if (value.length () > AP_NAME_MAX_SZ)
        {
            ResponseMessage = String (F ("-> AP Hotspot cannot be longer than ")) + String (AP_NAME_MAX_SZ) + F (" characters.");
            Log.warningln (ResponseMessage.c_str ());
            ESPUI.updateControlValue (ControlId, DataValueStr);
            break;
        }

        if (value.equals (DataValueStr))
        {
            // DEBUG_V ("Name did not change");
            break;
        }
        DataValueStr = value;
        ESPUI.updateControlValue (ControlId, DataValueStr);
        Log.infoln ((String (F ("Webserver (AP) Name Set to: '")) + DataValueStr + F ("'")).c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cHotspotName HotspotName;

// *********************************************************************************************
// OEF
