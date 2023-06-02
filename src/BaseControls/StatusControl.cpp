/*
  *    File: StatusControl.cpp
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
#include "StatusControl.hpp"
#include "memdebug.h"

// *********************************************************************************************
cStatusControl::cStatusControl (const String & _Title) :
    cControlCommonMsg (emptyString, ControlType::Label, _Title, emptyString, 0)
{
    // _ DEBUG_START;
    setSaveUpdates (false);
    // _ DEBUG_END;
}

// *********************************************************************************************
cStatusControl::cStatusControl () :
    cControlCommonMsg (emptyString, ControlType::Label, emptyString, emptyString, 0)
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cStatusControl::set (const String & value, eCssStyle style, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    cControlCommonMsg::setControlStyle (style);

    set (value, SkipLogOutput, ForceUpdate);

    // DEBUG_END;
}

// *********************************************************************************************
bool cStatusControl::set (const String & value, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    String  Dummy;
    bool    Response = cControlCommon::set (value, Dummy, SkipLogOutput, ForceUpdate);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
