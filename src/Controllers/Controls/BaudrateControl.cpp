/*
   File: BaudrateControl.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Apr-06-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
   absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

    Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
 */

// *********************************************************************************************
#include <ArduinoLog.h>
#include "BaudrateControl.hpp"
# include "memdebug.h"

static const PROGMEM String Name = F("Baudrate");
#define  SERIAL_DEF_STR SERIAL_115_STR

static ChoiceListVector_t MapOfBaudrates
{
    {"9600",    "9600"},
    {"19.2K",   "19200"},
    {"57.6K",   "57600"},
    {"115.2K",  "115200"},
};

// ================================================================================================
cBaudrateControl::cBaudrateControl () : cChoiceListControl (Name, Name, "115.2K", &MapOfBaudrates)
{}      // cBaudrateControl

// ================================================================================================
cBaudrateControl::~cBaudrateControl ()
{}

// *********************************************************************************************
void cBaudrateControl::AddControls (uint16_t value, ControlColor color)
{
 // DEBUG_START;

    ESPUI.addControl(ControlType::Label, Name.c_str(), Name, color, value);
    cChoiceListControl::AddControls(value, color);

 // DEBUG_END;
}

// *********************************************************************************************
// EOF
