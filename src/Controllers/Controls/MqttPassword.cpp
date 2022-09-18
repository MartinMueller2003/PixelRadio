/*
  *    File: MqttPassword.cpp
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

#include "MqttPassword.hpp"
#include "credentials_user.h"
#include "memdebug.h"

static const PROGMEM char   CTRL_MQTT_PW_STR    []  = "BROKER PASSWORD";
static const PROGMEM char   SAVE_MQTT_PW_STR    []  = "MQTT_PW_STR";
static const PROGMEM char   WIFI_BLANK_MSG_STR  []  = "LEAVE BLANK FOR AUTO LOGIN";
static const PROGMEM uint32_t MQTT_PW_MAX_SZ        = 48;

// *********************************************************************************************
cMqttPassword::cMqttPassword () :   cPasswordControl (SAVE_MQTT_PW_STR,
        CTRL_MQTT_PW_STR,
        MQTT_PW_STR,
        MQTT_PW_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMqttPassword::~cMqttPassword ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMqttPassword MqttPassword;

// *********************************************************************************************
// OEF
