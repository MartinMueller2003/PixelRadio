/*
  *    File: MqttUser.cpp
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
#include "MqttUser.hpp"
#include "memdebug.h"
#include "credentials_user.h"

static const PROGMEM char   MQTT_USER_STR       []  = "MQTT_USER_STR";
static const PROGMEM char   CTRL_MQTT_USER_STR  []  = "BROKER USERNAME";
static const PROGMEM uint32_t MQTT_NAME_MAX_SZ      = 18;


// *********************************************************************************************
cMqttUser::cMqttUser () :   cControlCommonMsg (MQTT_USER_STR,
        ControlType::Text,
        CTRL_MQTT_USER_STR,
        MQTT_ID_STR,
        MQTT_NAME_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMqttUser::~cMqttUser ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cMqttUser::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommonMsg::AddControls (value, color);
    setMessage (emptyString, eCssStyle::CssStyleTransparent);

    // DEBUG_END;
}

// *********************************************************************************************
bool cMqttUser::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cControlCommonMsg::validate (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        if (value.isEmpty ())
        {
            Response        = false;
            ResponseMessage = GetTitle () + F (": BAD_VALUE: '") + value + F ("'");
        }
    }

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cMqttUser MqttUser;

// *********************************************************************************************
// OEF
