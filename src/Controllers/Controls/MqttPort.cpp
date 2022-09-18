/*
  *    File: MqttPort.cpp
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

#include "MqttPort.hpp"
#include "memdebug.h"

static const PROGMEM char   MQTT_NAME_STR         []    = "MQTT_NAME_STR";
static const PROGMEM char   CTRL_MQTT_PORT_STR    []    = "BROKER PORT NUMBER";
static const PROGMEM uint32_t   MQTT_NAME_MAX_SZ        = 18;
static const PROGMEM uint32_t   MQTT_PORT_DEF           = 1883; // 1883 is Default MQTT Port. Change it here if differnt port is needed.

// *********************************************************************************************
cMqttPort::cMqttPort () :   cNumberControl (MQTT_NAME_STR,
        CTRL_MQTT_PORT_STR,
        MQTT_PORT_DEF,
        0,
        65535)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMqttPort::~cMqttPort ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMqttPort MqttPort;

// *********************************************************************************************
// OEF
