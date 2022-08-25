/*
   File: MqttName.cpp
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
#include "MqttName.hpp"
#include "memdebug.h"
#include "credentials_user.h"

static const PROGMEM char   MQTT_NAME_STR        []     = "MQTT_NAME_STR";
static const PROGMEM char   MQTT_SUBSCR_NM_STR    []    = "BROKER SUBSCRIBE NAME";
static const PROGMEM char   MQTT_NAME_DEF_STR     []    = "pixelradio";
static const PROGMEM uint32_t MQTT_NAME_MAX_SZ          = 18;

// *********************************************************************************************
cMqttName::cMqttName () :   cControlCommon (MQTT_NAME_STR,
                                              ControlType::Text,
                                              MQTT_SUBSCR_NM_STR,
                                              MQTT_NAME_DEF_STR,
                                              MQTT_NAME_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cMqttName::~cMqttName ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cMqttName::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, color);
    setMessage (emptyString, eCssStyle::CssStyleTransparent);

    // DEBUG_END;
}

// *********************************************************************************************
bool cMqttName::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cControlCommon::validate (value, ResponseMessage, ForceUpdate);

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
cMqttName MqttName;

// *********************************************************************************************
// OEF
