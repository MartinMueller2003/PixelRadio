/*
   File: PiCode.cpp
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
#include "language.h"
#include "PiCode.hpp"
#include "RfCarrier.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static const PROGMEM String RDS_PI_CODE_STR     = "PI CODE";
static const PROGMEM char RDS_PI_CODE           [] = "RDS_PI_CODE";

static const PROGMEM uint16_t RDS_PI_CODE_DEF   = uint32_t(0x6400);         // Default RDS PI Code, 16-bit hex value, 0x00ff - 0xffff.
static const PROGMEM uint32_t RDS_PI_CODE_MAX   = uint32_t(0xffff);         // Maximum PI Code Value (hex).
static const PROGMEM uint32_t RDS_PI_CODE_MIN   = uint32_t(0x00ff);         // Minumum PI Code Value (hex).

// *********************************************************************************************
cPiCode::cPiCode() : cControlCommon(String(RDS_PI_CODE))
{
    //_ DEBUG_START;

    DataValue = RDS_PI_CODE_DEF;
    DataValueStr = String(F("0x")) + String(DataValue, HEX);

    //_ DEBUG_END;
}

// *********************************************************************************************
void cPiCode::AddControls (uint16_t value)
{
    // DEBUG_START;

    do // once
    {
        cControlCommon::AddControls(value, 
                                    ControlType::Text, 
                                    ControlColor::Alizarin);
        ESPUI.updateControlLabel(ControlId, RDS_PI_CODE_STR.c_str());

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
bool cPiCode::set(String & value, String & ResponseMessage)
{
    // DEBUG_START;

    bool Response = true;
    ResponseMessage.reserve(128);
    ResponseMessage.clear();

    uint32_t NewPiCodeValue = 0;

    do // once
    {
        value.trim();
        NewPiCodeValue = strtol(value.c_str(), NULL, HEX);

        if(NewPiCodeValue == DataValue)
        {
            // DEBUG_V("No Change. Value is OK");
            break;
        }
        // DEBUG_V(String("NewPiCodeValue: ") + String(NewPiCodeValue, HEX));

        if ((NewPiCodeValue < RDS_PI_CODE_MIN) || 
            (NewPiCodeValue > RDS_PI_CODE_MAX))
        {
            ResponseMessage = String(F("Value Out of Range: ")) + value;
            Response = false;
            break;
        }
        // DEBUG_V("Value has been accepted");

        DataValue = NewPiCodeValue;
        DataValueStr = String(F("0x")) + String(DataValue, HEX);
        ESPUI.print(ControlId, DataValueStr);

        QN8027RadioApi.setPiCode(DataValue, RfCarrier.get());
        Log.infoln(String(F("RDS PI Code Set to: \"%s\"")).c_str(), DataValueStr);

        displaySaveWarning();

    } while (false);

    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V(String("       Response: ") + String(Response));

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cPiCode PiCode;

// *********************************************************************************************
// OEF
