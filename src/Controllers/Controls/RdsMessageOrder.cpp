/*
  *    File: RdsMessageOrder.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2023
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2023, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>

#include "RdsMessageOrder.hpp"

#include "memdebug.h"

static const PROGMEM char   ConfigName  []  = "RdsMessageOrderEnabled";
static const PROGMEM char   _Title []       = "RDS Message Order";

// *********************************************************************************************
cRdsMessageOrder::cRdsMessageOrder () :   cBinaryControl (ConfigName, _Title, true)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cRdsMessageOrder::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cBinaryControl::AddControls (TabId, color);
    setMessageStyle (eCssStyle::CssStyleWhite);
    setOnMessage ("Priority", eCssStyle::CssStyleWhite);
    setOffMessage ("Round Robin", eCssStyle::CssStyleWhite);

    // DEBUG_END;
}

// *********************************************************************************************
cRdsMessageOrder RdsMessageOrder;

// *********************************************************************************************
// OEF
