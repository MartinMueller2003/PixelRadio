/*
   File: IpAddressControl.cpp
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
#include "IpAddressControl.hpp"
#include "memdebug.h"

static const PROGMEM uint32_t IpAddress_MAX_SZ = 16;

// *********************************************************************************************
cIpAddressControl::cIpAddressControl (const String & ConfigName, const String & Title) :
    cControlCommon (ConfigName, ControlType::Text, Title)
{
    // _ DEBUG_START;

    DataValueStr.reserve (IpAddress_MAX_SZ + 2);
    DataValueStr = F ("0.0.0.0");

    // _ DEBUG_END;
}

// *********************************************************************************************
cIpAddressControl::~cIpAddressControl ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
IPAddress cIpAddressControl::GetIpAddress ()
{
    // DEBUG_V (String (": ") + IpAddress.toString ());

    return IpAddress;
}

// *********************************************************************************************
void cIpAddressControl::ResetToDefaults ()
{
    // DEBUG_START;

    String value = F ("0.0.0.0");
    IpAddress = IPAddress (uint32_t (0));
    String dummy;

    set (value, dummy, true);

    // DEBUG_END;
}

// *********************************************************************************************
bool cIpAddressControl::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V (       String ("       value: ") + value);
    // DEBUG_V (       String ("DataValueStr: ") + DataValueStr);

    bool Response = true;

    IPAddress TempIp;
    TempIp.fromString (value);

    do  // once
    {
        if (!cControlCommon::set (value, ResponseMessage, ForceUpdate))
        {
            // DEBUG_V ("Failed validation");
            ESPUI.updateControlValue (ControlId, DataValueStr);
            setControlStyle (eCssStyle::CssStyleRed_bw);
            setMessage (ResponseMessage, eCssStyle::CssStyleRed_bw);
            break;
        }

        if (!ForceUpdate && (TempIp == IpAddress))
        {
            // DEBUG_V ("Address did not change");
            Log.infoln ((Title + F (": Unchanged")).c_str ());
            break;
        }
        setControlStyle (eCssStyle::CssStyleBlack_bw);
        setMessage (emptyString, eCssStyle::CssStyleTransparent);

        IpAddress       = TempIp;
        DataValueStr    = IpAddress.toString ();
    } while (false);

    // DEBUG_V (       String (" ResponseMsg: ") + ResponseMessage);
    // DEBUG_V (       String ("    Response: ") + Response);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
bool cIpAddressControl::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (       String ("       value: ") + value);
    // DEBUG_V (       String ("DataValueStr: ") + DataValueStr);

    bool Response = true;

    IPAddress TempIp;

    do  // once
    {
        if (!TempIp.fromString (value))
        {
            ResponseMessage = Title + (F (": value: '")) + value + F ("' is not valid");
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        if (!ForceUpdate && (TempIp == IPAddress (uint32_t (0))))
        {
            ResponseMessage = Title + (F (" value: '")) + value + F ("' is not valid");
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }
    } while (false);

    // DEBUG_V (       String (" ResponseMsg: ") + ResponseMessage);
    // DEBUG_V (       String ("    Response: ") + Response);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
