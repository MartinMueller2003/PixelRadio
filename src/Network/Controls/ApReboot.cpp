/*
   File: ApReboot.cpp
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
#include "ApReboot.hpp"
#include "memdebug.h"

extern void displaySaveWarning ();

static const PROGMEM String     ENABLED_STR             = "Enabled";
static const PROGMEM String     DISABLED_STR            = "Disabled";
static const PROGMEM String     WIFI_AP_REBOOT_STR      = "AP REBOOT";

// *********************************************************************************************
cApReboot::cApReboot () : cOldControlCommon ("WIFI_REBOOT_FLAG")
{
    // _ DEBUG_START;

    DataValueStr        = "1";
    DataValue           = true;

    ActiveLabelStyle    = CSS_LABEL_STYLE_BLACK;
    InactiveLabelStyle  = CSS_LABEL_STYLE_BLACK;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cApReboot::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Switcher, color);
    ESPUI.updateControlLabel (ControlId, WIFI_AP_REBOOT_STR.c_str ());
    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_BLACK);

    // DEBUG_END;
}

// *********************************************************************************************
bool cApReboot::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V (       String ("       value: ") + value);
    // DEBUG_V (       String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (       String ("   DataValue: ") + String (DataValue));

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (DataValueStr.equals (value))
        {
            // DEBUG_V ("Ignore duplicate setting");
            ResponseMessage = DataValue ? ENABLED_STR : DISABLED_STR;
            break;
        }

        if (value.equals (F ("0")))
        {
            DataValue = 0;
        }
        else if (value.equals (F ("1")))
        {
            DataValue = 1;
        }
        else
        {
            ResponseMessage = String (F ("AP Fallback Invalid Value: ")) + value;
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }
        DataValueStr    = value;
        ResponseMessage = DataValue ? ENABLED_STR : DISABLED_STR;
        Response        = true;

        ESPUI.updateControlValue (ControlId, DataValueStr);
        ESPUI.print (StatusMessageId, ResponseMessage);

        Log.infoln (String (F ("AP Fallback Set to: %s.")).c_str (), ResponseMessage.c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_V (       String ("       value: ") + value);
    // DEBUG_V (       String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (       String ("   DataValue: ") + String (DataValue));
    // DEBUG_V (       String ("    Response: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cApReboot ApReboot;

// *********************************************************************************************
// OEF
