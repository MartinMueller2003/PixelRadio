/*
   File: GpioCommon.cpp
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

#include "GpioCommon.hpp"
#include "memdebug.h"

static ChoiceListVector_t ListOfOptions
{
    {"DIGITAL INPUT = FLOAT",       String("0, ") + String(INPUT)},
    {"DIGITAL INPUT = PULLUP",      String("0, ") + String(INPUT_PULLUP)},
    {"DIGITAL INPUT = PULLDOWN",    String("0, ") + String(INPUT_PULLDOWN)},
    {"DIGITAL OUTPUT = LOW",        String("0, ") + String(OUTPUT)},
    {"DIGITAL OUTPUT = HIGH",       String("1, ") + String(OUTPUT)},
};

static std::map<String, String> CommandTranslation =
{
    {"read", "read"},
    {"outhigh", "DIGITAL OUTPUT = HIGH"},
    {"outlow", "DIGITAL OUTPUT = LOW"},
};

// *********************************************************************************************
cGpioCommon::cGpioCommon (const String & ConfigName, gpio_num_t _pinId) :
    pinId(_pinId),
    cChoiceListControl (
            ConfigName,
            String(F("GPIO PIN ")) + String(_pinId),
            String("DIGITAL INPUT = PULLDOWN"),
            & ListOfOptions)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cGpioCommon::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cChoiceListControl::set (value, ResponseMessage, ForceUpdate);

    do // once
    {
        // read : outhigh : outlow

        if(String(F("read")).equals(get()))
        {
            // DEBUG_V("Read pin");
            ResponseMessage = String(digitalRead(pinId));
            Response = true;
            break;
        }

        if (!Response)
        {
            // DEBUG_V("Nothing to do");
            break;
        }

        String Action = ListOfOptions[getIndex()].second;
        // DEBUG_V(String("  Action: ") + Action);

        uint32_t position = Action.indexOf(",");
        pinMode (pinId, Action.substring(position+2).toInt());
        digitalWrite (pinId, Action.substring(0,position).toInt());

        // DEBUG_V(String("position: ") + String(position));
        // DEBUG_V(String("PinValue: ") + Action.substring(0,position));
        // DEBUG_V(String(" PinMode: ") + Action.substring(position+2));

    } while(false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
bool cGpioCommon::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = true;
    String temp = value;
    temp.toLowerCase();

    if(CommandTranslation.end() != CommandTranslation.find(temp))
    {
        // DEBUG_V ("Found a match for the value");
        DataValueStr = CommandTranslation[value];
    }
    else
    {
        Response = cChoiceListControl::validate(value, ResponseMessage, ForceUpdate);
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
// OEF
