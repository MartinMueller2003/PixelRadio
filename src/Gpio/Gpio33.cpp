/*
  *    File: Gpio33.cpp
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

#include "Gpio33.hpp"
#include "memdebug.h"

static const PROGMEM char ConfigName [] = "GPIO33_STR";
static const PROGMEM char _Title [] = "GPIO PIN 33";

// *********************************************************************************************
cGpio33::cGpio33 () :   cGpioCommon (ConfigName, _Title, gpio_num_t (33))
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
cGpio33 Gpio33;

// *********************************************************************************************
// OEF
