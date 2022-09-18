/*
  *    File: Gpio19.cpp
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

#include "Gpio19.hpp"
#include "memdebug.h"

static const PROGMEM char ConfigName [] = "GPIO19_STR";

// *********************************************************************************************
cGpio19::cGpio19 () :   cGpioCommon (ConfigName, gpio_num_t (19))
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
cGpio19 Gpio19;

// *********************************************************************************************
// OEF
