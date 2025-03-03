/*
  *    File: IpAddressControlStacked.cpp
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
#include "IpAddressControlStacked.hpp"
#include "memdebug.h"

static const PROGMEM uint32_t IpAddress_MAX_SZ = 16;

// *********************************************************************************************
cIpAddressControlStacked::cIpAddressControlStacked (const String & ConfigName, const String & Title, IPAddress DefaultValue) :
    cIpAddressControl (ConfigName, Title, DefaultValue)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cIpAddressControlStacked::~cIpAddressControlStacked ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cIpAddressControlStacked::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    uint16_t TempId = ESPUI.addControl (
        ControlType::Label,
        emptyString.c_str (),
        GetTitle (),
        color,
        TabId);
    ESPUI.setElementStyle (TempId, CSS_LABEL_STYLE_BLACK);
    ESPUI.setPanelStyle (TempId, F ("font-size: 1.35em;"));

    cIpAddressControl::AddControls (TabId, color);

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
