/*
   File: NumberControl.cpp
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

#include "NumberControl.hpp"
#include "memdebug.h"

// *********************************************************************************************
cNumberControl::cNumberControl (
    const String    & ConfigName,
    const String    & Title,
    uint32_t        DefaultValue,
    uint32_t        _MinValue,
    uint32_t        _MaxValue) :
    MinValue (_MinValue),
    MaxValue (_MaxValue),
    cControlCommon (
        ConfigName,
        ControlType::Number,
        Title,
        String (DefaultValue),
        10)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
uint32_t cNumberControl::StringToNumber (const String & value)
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
bool cNumberControl::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = true;

    // DEBUG_V (       String ("         value: ") + value);
    // DEBUG_V (       String ("           get: ") + get ());

    uint32_t OldNumberControlValue = StringToNumber (get ());
    // DEBUG_V (       String ("OldNumberControlValue: ") + String (OldNumberControlValue));

    do  // once
    {
        uint32_t NewNumberControlValue = StringToNumber (value);
        // DEBUG_V (String ("NewNumberControlValue: ") + String (NewNumberControlValue));

        if (NewNumberControlValue == OldNumberControlValue)
        {
            // DEBUG_V ("No Change. Value is OK");
            break;
        }

        if ((NewNumberControlValue < MinValue) ||
            (NewNumberControlValue > MaxValue))
        {
            // DEBUG_V ("Value is out of range");
            ResponseMessage = GetTitle () + (F (": BAD_VALUE: Value Out of Range: ")) + value;
            Response        = false;
            break;
        }
        // DEBUG_V ("Value has been accepted");

        DataValueStr = String (NewNumberControlValue);
    } while (false);

    // DEBUG_V (       String ("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V (       String ("       Response: ") + String (Response));

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
