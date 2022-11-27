/*
  *    File: BinaryControl.cpp
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
#include "PixelRadio.h"
#include "BinaryControl.hpp"
#include "memdebug.h"

static const PROGMEM char   ENABLED_STR     []  = "Enabled";
static const PROGMEM char   DISABLED_STR    []  = "Disabled";

static std::map <String, bool> ValidInputValues =
{
    {"enable",   true        },
    {"disable",  false       },
    {"enabled",  true        },
    {"disabled", false       },
    {"1",        true        },
    {"0",        false       },
    {"on",       true        },
    {"off",      false       },
    {"true",     true        },
    {"false",    false       },
};

// *********************************************************************************************
cBinaryControl::cBinaryControl (
                                const String    & ConfigName,
                                const String    & _Title,
                                bool            DefaultValue) :
    cControlCommonMsg (ConfigName, ControlType::Switcher, _Title, String (DefaultValue), 10)
{
    // _ DEBUG_START;

    DataValue = DefaultValue;

    if (OnString.isEmpty ())
    {
        OnString = ENABLED_STR;
    }

    if (OffString.isEmpty ())
    {
        OffString = DISABLED_STR;
    }

    // _ DEBUG_END;
}

// *********************************************************************************************
void cBinaryControl::addInputCondition (const String & Name, bool value)
{
    // _ DEBUG_START;

    String Temp = Name;
    Temp.toLowerCase ();
    ValidInputValues[Name] = value;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cBinaryControl::restoreConfiguration (JsonObject & config)
{
    // DEBUG_START;

    // DEBUG_V (String (" OnString: ") + OnString);
    // DEBUG_V (String ("OffString: ") + OffString);

    if (!ConfigName.isEmpty ())
    {
        // DEBUG_V("Process Config");
        bool NewValue = DataValue;
        ReadFromJSON (NewValue, config, ConfigName);
        String  NewValueStr = String (DataValue);
        String  Response;
        set (NewValueStr, Response, false);
    }

    // DEBUG_V (String (" OnString: ") + OnString);
    // DEBUG_V (String ("OffString: ") + OffString);

    // DEBUG_END;
}

// *********************************************************************************************
void cBinaryControl::saveConfiguration (JsonObject & config)
{
    // DEBUG_START;
    // DEBUG_V (String (" OnString: ") + OnString);
    // DEBUG_V (String ("OffString: ") + OffString);
    // DEBUG_V (String ("    Guard: ") + String (Guard, HEX));

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
    // DEBUG_V (String ("       Title: ") + GetTitle());
    // DEBUG_V (String ("       value: ") + value);
    // DEBUG_V (String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (String ("   DataValue: ") + String (DataValue));
    // DEBUG_V (String ("    OnString: ") + OnString);
    // DEBUG_V (String ("   OffString: ") + OffString);
    // DEBUG_V (String ("  SkipSetLog: ") + String (SkipSetLog));

    if (OnString.isEmpty ())
    {
        OnString = ENABLED_STR;
    }

    if (OffString.isEmpty ())
    {
        OffString = DISABLED_STR;
    }

    // DEBUG_V (String ("    OnString: ") + OnString);
    // DEBUG_V (String ("   OffString: ") + OffString);

    bool OldSkipSetLog = SkipSetLog;
    SkipSetLog = true;
    bool Response = cControlCommonMsg::set (value, ResponseMessage, ForceUpdate);
    SkipSetLog = OldSkipSetLog;

    if (Response)
    {
        // DEBUG_V ();
        ResponseMessage = DataValue ? OnString : OffString;
        // DEBUG_V (String ("    Response: ") + ResponseMessage);
        setMessage (ResponseMessage, DataValue ? OnStyle : OffStyle);

        if (!SkipSetLog)
        {
            String LogMsg;
            LogMsg.reserve (128);
            LogMsg = GetTitle () + F (": Set To '") + ResponseMessage + F ("'");
            Log.infoln (LogMsg.c_str ());
        }
    }

    // DEBUG_V (String ("       value: ") + value);
    // DEBUG_V (String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (String ("   DataValue: ") + String (DataValue));
    // DEBUG_V (String ("    Response: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
void cBinaryControl::setOffMessage (const String & value, eCssStyle style)
{
    // DEBUG_START;

    OffString   = value;
    OffStyle    = style;

    if (!getBool ())
    {
        setMessage (OffString, OffStyle);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cBinaryControl::setOffMessageStyle (eCssStyle style)
{
    // DEBUG_START;
    // DEBUG_V (String ("style: ") + String (style));

    OffStyle = style;

    if (!getBool ())
    {
        setMessage (OffString, OffStyle);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cBinaryControl::setOnMessage (const String & value, eCssStyle style)
{
    // DEBUG_START;

    OnString    = value;
    OnStyle     = style;

    if (getBool ())
    {
        setMessage (OnString, OnStyle);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cBinaryControl::setOnMessageStyle (eCssStyle style)
{
    // DEBUG_START;
    // DEBUG_V (String ("style: ") + String (style));

    OnStyle = style;

    if (getBool ())
    {
        setMessage (OnString, OnStyle);
    }

    // DEBUG_END;
}

// *********************************************************************************************
bool cBinaryControl::validate (const String & value, String & ResponseMessage, bool)
{
    // DEBUG_START;
    // DEBUG_V (String ("    value: ") + value);
    // DEBUG_V (String (" OnString: ") + OnString);
    // DEBUG_V (String ("OffString: ") + OffString);
    // DEBUG_V (String ("    Guard: ") + String (Guard, HEX));

    bool Response = true;

    String Temp = value;
    Temp.toLowerCase ();

    if (ValidInputValues.end () == ValidInputValues.find (Temp))
    {
        ResponseMessage = GetTitle () + F (": BAD_VALUE: Invalid Binary Value '") + value + F ("'");
        Response        = false;
        // DEBUG_V (String ("ResponseMessage: ") + ResponseMessage);
    }
    else
    {
        // DEBUG_V ("Accept the input value");
        DataValue = ValidInputValues[Temp];
        SetDataValueStr (String (DataValue));
    }

    // DEBUG_V (String ("Response: ") + String (Response));
    // DEBUG_V (String (" Message: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
