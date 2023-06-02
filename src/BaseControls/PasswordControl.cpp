/*
  *    File: PasswordControl.cpp
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
#include "PasswordControl.hpp"
#include "memdebug.h"

// *********************************************************************************************
cPasswordControl::cPasswordControl (const String & ConfigName, const String & Title, const String & DefaultValue, uint32_t MaxDataLength) :
    cControlCommonMsg (ConfigName, ControlType::Password, Title, DefaultValue, MaxDataLength)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cPasswordControl::~cPasswordControl ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cPasswordControl::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V (       String ("         value: ") + value);
    // DEBUG_V (       String ("  DataValueStr: ") + DataValueStr);
    // DEBUG_V (       String ("PasswordString: ") + PasswordString);

    bool Response = cControlCommonMsg::set (value, ResponseMessage, SkipLogOutput, ForceUpdate);
    setControlStyle (eCssStyle::CssStyleBlack_bw);

    // DEBUG_V (       String ("  DataValueStr: ") + DataValueStr);
    // DEBUG_V (       String ("   ResponseMsg: ") + ResponseMessage);
    // DEBUG_V (       String ("      Response: ") + Response);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
