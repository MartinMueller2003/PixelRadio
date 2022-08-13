/*
   File: ProgramServiceName.cpp
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
#include "ProgramServiceName.hpp"
#include "PtyCode.hpp"
#include "RfCarrier.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static const PROGMEM String RDS_PSN_DEF_STR       = "PixeyFM";
static const PROGMEM String RDS_PROG_SERV_STR     = "RDS_PROG_SERV_STR";
static const PROGMEM String RDS_PROG_SERV_NM_STR  = "PROGRAM SERVICE NAME<br>Station Name (4-8 Chars)";

// *********************************************************************************************
cProgramServiceName::cProgramServiceName() : cControlCommon(RDS_PROG_SERV_STR)
{
    //_ DEBUG_START;
    DataValueStr.reserve(10);
    DataValueStr = RDS_PSN_DEF_STR;
    //_ DEBUG_END;
}

// *********************************************************************************************
void cProgramServiceName::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls(value, ControlType::Text, color);
    ESPUI.updateControlLabel(ControlId, RDS_PROG_SERV_NM_STR.c_str());
    ESPUI.addControl(ControlType::Min, emptyString.c_str(), "4", ControlColor::None, ControlId);
    ESPUI.addControl(ControlType::Max, emptyString.c_str(), "8", ControlColor::None, ControlId);

    // DEBUG_END;
}

// *********************************************************************************************
bool cProgramServiceName::set(String & value, String & ResponseMessage)
{
    // DEBUG_START;

    bool Response = true;
    ResponseMessage.reserve(128);
    ResponseMessage.clear();

    do // once
    {
        if(DataValueStr.equals(value))
        {
            // DEBUG_V("Skip duplicate name");
            break;
        }

        ResponseMessage = String(F("Program Service Name: '")) + value;
        if(value.length() < 4)
        {
            ResponseMessage += F("' is too short.");
            Log.infoln(ResponseMessage.c_str());
            Response = false;
            break;
        }

        if(value.length() > 8)
        {
            ResponseMessage += F("' is too long.");
            Log.infoln(ResponseMessage.c_str());
            Response = false;
            break;
        }

        // DEBUG_V();
        DataValueStr = value;

        ResponseMessage += F("' Accepted");

        ESPUI.updateControlValue(ControlId, DataValueStr);
        QN8027RadioApi.setProgramServiceName(DataValueStr, RfCarrier.get());
        PtyCode.setPtyCodeOptionValues();
        
        displaySaveWarning();
        Log.infoln(ResponseMessage.c_str());
        ResponseMessage.clear();
        
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cProgramServiceName ProgramServiceName;

// *********************************************************************************************
// OEF
