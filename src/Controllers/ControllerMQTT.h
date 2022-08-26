/*
   File: ControllerMQTT.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Apr-06-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
   absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

    Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
 */

// *********************************************************************************************
#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "credentials.h"
#include "ControllerCommon.h"
#include "ControllerMessages.h"

class fsm_Connection_state;

class c_ControllerMQTT : public cControllerCommon
{
public:

    c_ControllerMQTT (void);
    virtual ~c_ControllerMQTT (void);
    void    begin (void);
    void    poll (void);
    void    saveConfiguration (ArduinoJson::JsonObject & config);
    void    restoreConfiguration (ArduinoJson::JsonObject & config);

    void    AddControls (uint16_t TabId, ControlColor color);
    void    mqttClientCallback (const char * topic, byte * payload, unsigned int length);
    void    GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response);

private:

    void    mqttReconnect (bool resetFlg);
    void    mqttSendMessages (void);
    String  makeMqttCmdStr (String cmdStr);
    void    gpioMqttControl (String payloadStr, gpio_num_t pin);
    String  returnClientCode (int code);
    void    TestParameters ();
    bool    ValidateConfiguration();
    bool    ConfigHasChanged();

    WiFiClient wifiClient;
    PubSubClient mqttClient;
    bool OnlineFlag = false;
    c_ControllerMessages Messages;

    /* If you have an MQTT Broker then configure it here. */
    // const IPAddress MQTT_IP_DEF = { 192u, 168u, 1u, 202u }; // Default IP of MQTT Broker server. Can be changed in Web UI.

    uint16_t EspuiMessageAreaId = Control::noParent;
    uint16_t EspuiNameID        = Control::noParent;
    uint16_t EspuiPortID        = Control::noParent;
    uint16_t EspuiUserID        = Control::noParent;
    uint16_t EspuiPwID          = Control::noParent;

    String MessageStr = "";

    bool refresh            = true;
    long previousMqttMillis = millis ();    // Timer for MQTT services.
    float oldVbatVolts      = -1.0f;
    float oldPaVolts        = -1.0f;

// MQTT Command
#define MQTT_CMD_STR       F ("/cmd/")      // Publish topic preamble, Client MQTT Command Preampble.

// MQTT Subscriptions
#define MQTT_CMD_SUB_STR   F ("/cmd/#")     // MQTT wildcard Subscription, receive all /cmd messages.

// MQTT Publish Topics
#define MQTT_CONNECT_STR   F ("/connect")   // Publish topic, Client MQTT Subscription.
#define MQTT_GPIO_STR      F ("/gpio")      // Publish topic, Client MQTT Subscription.
#define MQTT_INFORM_STR    F ("/info")      // Publish topic, Client MQTT Subscription.
#define MQTT_VOLTS_STR     F ("/volts")     // Publish topic, Client MQTT Subscription.

    friend class fsm_Connection_state_disabled;
    friend class fsm_Connection_state_WaitForWiFi;
    friend class fsm_Connection_state_ValidateConfig;
    friend class fsm_Connection_state_connecting;
    friend class fsm_Connection_state_connected;
    friend class fsm_Connection_state_Disconnecting;
    fsm_Connection_state * pCurrentFsmState = nullptr;
    uint32_t FsmTimerExpirationTime         = 1000;
};  // class c_ControllerMQTT

// *********************************************************************************************
// *********************************************************************************************
// *********************************************************************************************
class fsm_Connection_state
{
protected:
    c_ControllerMQTT * pParent = nullptr;

public:
    fsm_Connection_state () {}

    virtual ~fsm_Connection_state () {}

    virtual void    Poll (uint32_t) = 0;
    virtual void    Init (void)     = 0;
    void            SetParent (c_ControllerMQTT * value) {pParent = value;}
};  // class fsm_Connection_state

// *********************************************************************************************
class fsm_Connection_state_disabled : public fsm_Connection_state
{
public:
    void    Poll (uint32_t);
    void    Init (void);
};  // class fsm_Connection_state_disabled

// *********************************************************************************************
class fsm_Connection_state_WaitForWiFi : public fsm_Connection_state
{
public:
    void    Poll (uint32_t);
    void    Init (void);
};  // class fsm_Connection_state_WaitForWiFi

// *********************************************************************************************
class fsm_Connection_state_ValidateConfig : public fsm_Connection_state
{
public:
    void    Poll (uint32_t);
    void    Init (void);
};  // class fsm_Connection_state_ValidateConfig

// *********************************************************************************************
class fsm_Connection_state_connecting : public fsm_Connection_state
{
public:
    void    Poll (uint32_t);
    void    Init (void);
};  // class fsm_Connection_state_connecting

// *********************************************************************************************
class fsm_Connection_state_connected : public fsm_Connection_state
{
public:
    void    Poll (uint32_t);
    void    Init (void);
};  // class fsm_Connection_state_connected

// *********************************************************************************************
class fsm_Connection_state_Disconnecting : public fsm_Connection_state
{
public:
    void    Poll (uint32_t);
    void    Init (void);
};  // class fsm_Connection_state_Disconnecting

// *********************************************************************************************
// *********************************************************************************************
// EOF
