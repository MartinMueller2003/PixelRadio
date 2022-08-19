/*
   File: BinaryControl.cpp
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
#include "PixelRadio.h"
#include "BinaryControl.hpp"
#include "memdebug.h"

static const PROGMEM String     ENABLED_STR     = "Enabled";
static const PROGMEM String     DISABLED_STR    = "Disabled";

// *********************************************************************************************
cBinaryControl::cBinaryControl (const String & ConfigName, const String & _Title) :
    cControlCommon (ConfigName, ControlType::Switcher, _Title)
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cBinaryControl::restoreConfiguration (JsonObject & config)
{
    // DEBUG_START;

    if (!ConfigName.isEmpty ())
    {
        bool NewValue = DataValue;
        ReadFromJSON (NewValue, config, ConfigName);
        String  NewValueStr = String (DataValue);
        String  Response;
        cControlCommon::set (NewValueStr, Response, false);
    }
    // DEBUG_END;
}

// *********************************************************************************************
void cBinaryControl::saveConfiguration (JsonObject & config)
{
    // DEBUG_START;

    if (!ConfigName.isEmpty ())
    {
        config[ConfigName] = DataValue;
    }
    // DEBUG_END;
}

// *********************************************************************************************
bool cBinaryControl::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (       String ("       value: ") + value);
    // DEBUG_V (       String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (       String ("   DataValue: ") + String (DataValue));

    SkipSetLog = true;
    bool Response = cControlCommon::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        DataValue       = value.equals (F ("1"));
        ResponseMessage = DataValue ? ENABLED_STR : DISABLED_STR;
        ESPUI.print (MessageId, ResponseMessage);
        String LogMsg;
        LogMsg.reserve (128);
        LogMsg = Title + F (": Set To '") + ResponseMessage + F ("'");
        Log.infoln (LogMsg.c_str ());
    }
    // DEBUG_V (       String ("       value: ") + value);
    // DEBUG_V (       String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (       String ("   DataValue: ") + String (DataValue));
    // DEBUG_V (       String ("    Response: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
bool cBinaryControl::validate (const String & value, String & ResponseMessage, bool)
{
    // DEBUG_START;
    // DEBUG_V (String ("value: ") + value);

    bool Response = true;

    if (!value.equals (F ("0")) && !value.equals (F ("1")))
    {
        ResponseMessage = Title + F (": Invalid Binary Value '") + value + F ("'");
        Response        = false;
    }
    // DEBUG_V (       String ("Response: ") + String (Response));
    // DEBUG_V (       String (" Message: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
