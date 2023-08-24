/*
  *    File: RfCarrierStatus.cpp
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
#include "language.h"
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include "RfCarrierStatus.hpp"

static const PROGMEM String HOME_RAD_STAT_STR = "STATUS";

// *********************************************************************************************
cRfCarrierStatus::cRfCarrierStatus () :   cStatusControl (HOME_RAD_STAT_STR, emptyString)
{
    // _ DEBUG_START;

    setControlPanelStyle (ePanelStyle::PanelStyle300);

    // _ DEBUG_END;
}

// *********************************************************************************************
cRfCarrierStatus RfCarrierStatus;

// *********************************************************************************************
// OEF
