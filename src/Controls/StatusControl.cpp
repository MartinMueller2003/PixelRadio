/*
   File: StatusControl.cpp
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
#include "StatusControl.hpp"
#include "memdebug.h"

// *********************************************************************************************
cStatusControl::cStatusControl (const String &_Title) :
    cControlCommon (emptyString, ControlType::Label, _Title)
{
    // _ DEBUG_START;

    SkipSetLog = true;

    // _ DEBUG_END;
}

// *********************************************************************************************
bool cStatusControl::set (const String &value)
{
    // DEBUG_START;

    String      Dummy;
    bool        Response = cControlCommon::set (value, Dummy);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
