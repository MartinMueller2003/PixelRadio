/*
   File: SystemVoltage.cpp
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

#include "SystemVoltage.hpp"

const PROGMEM char SYSTEM_VOLTAGE [] = "SYSTEM VOLTAGE";
// Onboard ESP32 Resistor Attenuator on "VBAT" ADC Port.
const float Scale = ((100000.0 + 100000.0) / 100000.0);

// *********************************************************************************************
cSystemVoltage::cSystemVoltage () :   cVoltageStatus (SYSTEM_VOLTAGE, ADC1_CHANNEL_7, Scale)
{}

// *********************************************************************************************
cSystemVoltage SystemVoltage;

// *********************************************************************************************
// OEF
