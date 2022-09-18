/*
  *    File: CreateButton.cpp
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

#include "CreateButton.hpp"
#include "memdebug.h"

// *********************************************************************************************

static const PROGMEM char _Title       [] = " Create ";

// *********************************************************************************************
cCreateButton::cCreateButton () :   cButtonControl (_Title)
{
    // DEBUG_START;
    // DEBUG_END;
}

// *********************************************************************************************
void cCreateButton::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;
    // DEBUG_V(String("TabId: ") + String(TabId));
    // DEBUG_V(String("color: ") + String(color));

    cButtonControl::AddControls (TabId, color);
    setControl (_Title, eCssStyle::CssStyleBlack);

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
