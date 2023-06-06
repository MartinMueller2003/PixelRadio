/*
  *    File: CurrentSequence.cpp
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
#include "CurrentSequence.hpp"
#include "memdebug.h"

static const PROGMEM char _Title [] = "Current Sequence";

// *********************************************************************************************
cCurrentSequence::cCurrentSequence () :   cStatusControl (_Title)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cCurrentSequence::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cStatusControl::AddControls (TabId, color);
    setControl (emptyString, eCssStyle::CssStyleWhite);

    // DEBUG_END;
}

// *********************************************************************************************
cCurrentSequence CurrentSequence;

// *********************************************************************************************
// OEF
