/*
   File: ApIpAddress.cpp
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
#include "ApIpAddress.hpp"
#include "memdebug.h"
#include "PtyCode.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String WIFI_AP_IP_ADDR_STR = "AP IP Address";
static const PROGMEM uint32_t IpAddress_MAX_SZ  = 16;
static const PROGMEM IPAddress DefaultIpAddress = IPAddress (192, 168, 4, 1);

// *********************************************************************************************
cApIpAddress::cApIpAddress () : cOldControlCommon ("AP_IP_ADDR_STR")
{
    // _ DEBUG_START;

    DataValueStr.reserve (IpAddress_MAX_SZ + 2);
    DataValueStr        = DefaultIpAddress.toString ();
    IpAddress           = IPAddress (uint32_t (0));
    DataValue           = IpAddress;
    ActiveLabelStyle    = CSS_LABEL_STYLE_BLACK;
    InactiveLabelStyle  = CSS_LABEL_STYLE_BLACK;

    // _ DEBUG_END;
}

// *********************************************************************************************
cApIpAddress::~cApIpAddress ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cApIpAddress::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Text, color);
    ESPUI.updateControlLabel (ControlId, WIFI_AP_IP_ADDR_STR.c_str ());
    ESPUI.addControl (ControlType::Max, emptyString.c_str (), String (IpAddress_MAX_SZ), ControlColor::None, ControlId);

    // DEBUG_END;
}

// *********************************************************************************************
void cApIpAddress::ResetToDefaults ()
{
    // DEBUG_START;

    String      value = DefaultIpAddress.toString ();
    String      dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cApIpAddress::set (String & value, String & ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V ( String ("       value: ") + value);
    // DEBUG_V ( String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V ( String ("   DataValue: ") + String (DataValue));
    // DEBUG_V ( String ("   IpAddress: ") + IpAddress.toString ());

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    IPAddress TempIp;

    do  // once
    {
        if (!TempIp.fromString (value))
        {
            ResponseMessage = WIFI_AP_IP_ADDR_STR + (F (" value: '")) + value + String (F ("' is not valid"));
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        if (TempIp == IPAddress (uint32_t (0)))
        {
            ResponseMessage = WIFI_AP_IP_ADDR_STR + (F (" value: '")) + value + String (F ("' is not valid"));
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        if (TempIp == IpAddress)
        {
            // DEBUG_V ("Address did not change");
            Log.infoln ((WIFI_AP_IP_ADDR_STR + F (" Unchanged")).c_str ());
            break;
        }
        IpAddress       = TempIp;
        DataValueStr    = IpAddress.toString ();
        DataValue       = IpAddress;

        ESPUI.updateControlValue (ControlId, DataValueStr);
        Log.infoln ((WIFI_AP_IP_ADDR_STR + F (" Set to: ") + DataValueStr).c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_V ( String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V ( String ("   DataValue: ") + String (DataValue));
    // DEBUG_V ( String ("   IpAddress: ") + IpAddress.toString ());

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cApIpAddress ApIpAddress;

// *********************************************************************************************
// OEF
