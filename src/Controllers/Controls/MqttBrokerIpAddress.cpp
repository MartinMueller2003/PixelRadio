/*
  *    File: MqttBrokerIpAddress.cpp
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

#include "MqttBrokerIpAddress.hpp"
#include "memdebug.h"

static const PROGMEM char   CTRL_MQTT_IP_STR     [] = "BROKER IP ADDRESS";
static const PROGMEM char   MQTT_IP_STR          [] = "MQTT_IP_STR";

// *********************************************************************************************
cMqttBrokerIpAddress::cMqttBrokerIpAddress () :   cIpAddressControl (MQTT_IP_STR, CTRL_MQTT_IP_STR, IPAddress (192, 168, 1, 202))
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMqttBrokerIpAddress::~cMqttBrokerIpAddress ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMqttBrokerIpAddress MqttBrokerIpAddress;

// *********************************************************************************************
// OEF
