/*
  *    File: ProgramServiceName.cpp
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

#include "ProgramServiceName.hpp"
#include "PtyCode.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "memdebug.h"

static const PROGMEM char   RDS_PSN_DEF_STR         []  = "PixeyFM";
static const PROGMEM char   RDS_PROG_SERV_STR       []  = "RDS_PROG_SERV_STR";
static const PROGMEM char   RDS_PROG_SERV_NM_STR    []  = "PROGRAM SERVICE NAME<br>Station Name (4-8 Chars)";

// *********************************************************************************************
cProgramServiceName::cProgramServiceName () :   cControlCommon (
        RDS_PROG_SERV_STR,
        ControlType::Text,
        RDS_PROG_SERV_NM_STR,
        RDS_PSN_DEF_STR,
        8)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cProgramServiceName::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, color);
    ESPUI.addControl (ControlType::Min, emptyString.c_str (), "4", ControlColor::None, ControlId);

    // DEBUG_END;
}

// *********************************************************************************************
bool cProgramServiceName::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cControlCommon::set (value, ResponseMessage, SkipLogOutput, ForceUpdate);

    if (Response)
    {
        QN8027RadioApi.setProgramServiceName (GetDataValueStr (), RfCarrier.get ());
    }

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
bool cProgramServiceName::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    bool Response = true;

    do  // once
    {
        ResponseMessage = GetTitle () + F (": '") + value;

        if (value.length () < 4)
        {
            ResponseMessage += F ("' is too short. BAD_VALUE");
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        if (value.length () > 8)
        {
            ResponseMessage += F ("' is too long. BAD_VALUE");
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        // DEBUG_V ("Accepted");
        SetDataValueStr (value);
        ResponseMessage += F ("' Accepted");
    } while (false);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cProgramServiceName ProgramServiceName;

// *********************************************************************************************
// OEF
