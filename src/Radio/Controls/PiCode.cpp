/*
  *    File: PiCode.cpp
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

#include "PiCode.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "memdebug.h"

static const PROGMEM char   RDS_PI_CODE_STR []  = "PI CODE";
static const PROGMEM char   RDS_PI_CODE     []  = "RDS_PI_CODE";

static const PROGMEM uint32_t   RDS_PI_CODE_DEF = uint32_t (0x6400);    // Default RDS PI Code, 16-bit hex value, 0x00ff - 0xffff.
static const PROGMEM uint32_t   RDS_PI_CODE_MAX = uint32_t (0xffff);    // Maximum PI Code Value (hex).
static const PROGMEM uint32_t   RDS_PI_CODE_MIN = uint32_t (0x00ff);    // Minumum PI Code Value (hex).

// *********************************************************************************************
cPiCode::cPiCode () :   cControlCommon (
        RDS_PI_CODE,
        ControlType::Text,
        RDS_PI_CODE_STR,
        String (RDS_PI_CODE_DEF), 10)
{
    // _ DEBUG_START;

    SetDataValueStr (String (F ("0x")) + String (RDS_PI_CODE_DEF, HEX));

    // _ DEBUG_END;
}

// *********************************************************************************************
void cPiCode::ResetToDefaults ()
{
    // DEBUG_START;

    String  value = String (F ("0x")) + String (RDS_PI_CODE_DEF, HEX);
    String  dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cPiCode::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V (String ("         value: ") + value);

    bool Response = cControlCommon::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        QN8027RadioApi.setPiCode (uint16_t (StringToNumber (get ())), RfCarrier.get ());
    }

    // DEBUG_V (       String ("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V (       String ("       Response: ") + String (Response));

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
uint32_t cPiCode::StringToNumber (const String & value)
{
    // DEBUG_START;

    // DEBUG_V (String ("value: ") + value);
    uint32_t Response = value.toInt ();

    if (0 == value.indexOf ("0x"))
    {
        char * p = nullptr;
        Response = uint32_t (strtol (value.substring (2).c_str (), & p, 16));
    }

    // DEBUG_V (String ("Response: ") + String (Response, HEX));

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
bool cPiCode::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = true;

    // DEBUG_V (       String ("         value: ") + value);
    // DEBUG_V (       String ("           get: ") + get ());


    // DEBUG_V (       String ("OldPiCodeValue: ") + String (OldPiCodeValue));

    do  // once
    {
        uint32_t NewPiCodeValue = StringToNumber (value);
        // DEBUG_V (String ("NewPiCodeValue: ") + String (NewPiCodeValue));

        if ((NewPiCodeValue < RDS_PI_CODE_MIN) ||
            (NewPiCodeValue > RDS_PI_CODE_MAX))
        {
            // DEBUG_V ("Value is out of range");
            ResponseMessage = GetTitle () + (F (": BAD_VALUE: Value Out of Range: ")) + value;
            Response        = false;
            break;
        }

        // DEBUG_V ("Value has been accepted");

        SetDataValueStr (String (F ("0x")) + String (NewPiCodeValue, HEX));
    } while (false);

    // DEBUG_V (       String ("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V (       String ("       Response: ") + String (Response));

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cPiCode PiCode;

// *********************************************************************************************
// OEF
