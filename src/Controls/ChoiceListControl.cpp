/*
   File: ChoiceListControl.cpp
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
#include "ChoiceListControl.hpp"
#include "memdebug.h"

// *********************************************************************************************
cChoiceListControl::cChoiceListControl (const String &ConfigName, const String &Title, ChoiceList_t &_ChoiceList) :
    ChoiceList (_ChoiceList),
    cControlCommon (ConfigName, ControlType::Select, Title)
{
    // _ DEBUG_START;
    setControlStyle (CssStyleBlack_bw);
    // _ DEBUG_END;
}

// *********************************************************************************************
void cChoiceListControl::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, color);
    setControlStyle (CssStyleBlack_bw);

    for (auto &CurrentOption : ChoiceList)
    {
        ESPUI.addControl (ControlType::Option,
            CurrentOption.first.c_str (),
            CurrentOption.first,
            ControlColor::None,
            ControlId);
    }
    ESPUI.updateControlValue (ControlId, DataValueStr);

    // DEBUG_END;
}

// *********************************************************************************************
uint32_t cChoiceListControl::get32 ()
{
    // DEBUG_START;

    uint32_t Response;

    Response = atoi (ChoiceList[get ()].c_str ());

    // DEBUG_V (String ("Response: ") + String (Response));
    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
bool cChoiceListControl::validate (const String &value, String &ResponseMessage)
{
    // DEBUG_START;
    // DEBUG_V (String ("value: ") + value);

    bool Response = true;

    do  // once
    {
        if (DataValueStr.equals (value))
        {
            // DEBUG_V ("Ignore duplicate setting");
            Response = false;
            break;
        }

        if (ChoiceList.end () == ChoiceList.find (value))
        {
            ResponseMessage = String (Title + F (": Set: BAD VALUE: '")) + value + F ("'");
            Log.errorln (ResponseMessage.c_str ());
            Response = false;
            break;
        }
        DataValueStr = value;
    } while (false);

    // DEBUG_V (       String ("Response: ") + String (Response));
    // DEBUG_V (       String (" Message: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
