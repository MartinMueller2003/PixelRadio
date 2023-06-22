/*
  *    File: ControlGroup.cpp
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
#include "ControlGroup.hpp"
#include "memdebug.h"

// *********************************************************************************************
cControlGroup::cControlGroup () :
    cControlCommon (emptyString, ControlType::Label, emptyString, emptyString, 0)
{
    // _ DEBUG_START;

    setSaveUpdates (false);

    // _ DEBUG_END;
}

// *********************************************************************************************
cControlGroup::cControlGroup (const String & _Title) :
    cControlCommon (emptyString, ControlType::Label, _Title, emptyString, 0)
{
    // _ DEBUG_START;

    setSaveUpdates (false);

    // _ DEBUG_END;
}

// *********************************************************************************************
// OEF
