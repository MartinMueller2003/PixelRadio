/*
  *    File: IpAddressControl.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include "IpAddressControl.hpp"
#include "memdebug.h"

static const PROGMEM uint32_t IpAddress_MAX_SZ = 16;

// *********************************************************************************************
cIpAddressControl::cIpAddressControl (const String & ConfigName, const String & Title, const IPAddress DefaultValue) :
    cControlCommonMsg (ConfigName, ControlType::Text, Title, DefaultValue.toString (), IpAddress_MAX_SZ)
{
    // _ DEBUG_START;

    IpAddress = DefaultValue;

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
bool cIpAddressControl::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V (       String ("       value: ") + value);
    // DEBUG_V (       String ("DataValueStr: ") + DataValueStr);

    bool Response = true;

    IPAddress TempIp;
    TempIp.fromString (value);

    do  // once
    {
        if (!cControlCommon::set (value, ResponseMessage, SkipLogOutput, ForceUpdate))
        {
            // DEBUG_V ("Failed validation");
            ESPUI.updateControlValue (ControlId, GetDataValueStr ());
            setControlStyle (eCssStyle::CssStyleRed_bw);
            setMessage (ResponseMessage, eCssStyle::CssStyleRed_bw);
            break;
        }

        setControl (TempIp.toString (), eCssStyle::CssStyleBlack_bw);
        setMessage (emptyString, eCssStyle::CssStyleTransparent);

        if (!SkipLogOutput && !ForceUpdate && (TempIp == IpAddress))
        {
            // DEBUG_V ("Address did not change");
            Log.infoln ((GetTitle () + F (": Unchanged")).c_str ());
            break;
        }

        IpAddress = TempIp;
        SetDataValueStr (IpAddress.toString ());
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
            ResponseMessage = GetTitle () + (F (": value: '")) + value + F ("' is not valid");
            // Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        if (!ForceUpdate && (TempIp == IPAddress (uint32_t (0))))
        {
            ResponseMessage = GetTitle () + (F (" value: '")) + value + F ("' is not valid");
            // Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        SetDataValueStr (TempIp.toString ());
    } while (false);

    // DEBUG_V (       String (" ResponseMsg: ") + ResponseMessage);
    // DEBUG_V (       String ("    Response: ") + Response);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
