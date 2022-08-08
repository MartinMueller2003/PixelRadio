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
#include "ControllerCommon.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include "credentials.h"
#include "ControllerMessages.h"

class c_ControllerMQTT : public c_ControllerCommon
{
public:
            c_ControllerMQTT(void);
   virtual  ~c_ControllerMQTT(void);
   void     Begin(void);
   void     poll(void);
   void     saveConfiguration(ArduinoJson::JsonObject &config);
   void     restoreConfiguration(ArduinoJson::JsonObject &config);

   void     AddControls(uint16_t ctrlTab);
   void     setMqttAuthenticationCallback(Control *sender, int type);
   void     setMqttNameCallback(Control *sender, int type);
   void     setRemoteIpAddrCallback(Control *sender, int type);
   void     mqttClientCallback(const char *topic, byte *payload, unsigned int length);
   void     GetNextRdsMessage(c_ControllerMgr::RdsMsgInfo_t &Response) { if(ControllerEnabled){ Messages.GetNextRdsMessage(Response); }}

private:
   void     Init(void);
   void     mqttReconnect(bool resetFlg);
   void     mqttSendMessages(void);
   void     updateUiMqttMsg(String msgStr);
   String   makeMqttCmdStr(String cmdStr);
   void     gpioMqttControl(String payloadStr, gpio_num_t pin);
   String   returnClientCode(int code);
   void     TestParameters();

   WiFiClient           wifiClient;
   PubSubClient         mqttClient;
   bool                 OnlineFlag = false;
   c_ControllerMessages Messages;

#define MQTT_NAME_DEF_STR F("pixelradio") // Default MQTT Topic / Subscription Name.
/* If you have an MQTT Broker then configure it here. */
const IPAddress MQTT_IP_DEF = { 0u, 0u, 0u, 0u }; // Default IP of MQTT Broker server. Can be changed in Web UI.
//const IPAddress MQTT_IP_DEF = { 192u, 168u, 1u, 202u }; // Default IP of MQTT Broker server. Can be changed in Web UI.
const uint32_t MQTT_PORT_DEF = 1883;                // 1883 is Default MQTT Port. Change it here if differnt port is needed.

   uint16_t    EspuiMessageAreaId   = Control::noParent;
   uint16_t    EspuiNameID          = Control::noParent;
   uint16_t    EspuiIpID            = Control::noParent;
   uint16_t    EspuiPortID          = Control::noParent;
   uint16_t    EspuiUserID          = Control::noParent;
   uint16_t    EspuiPwID            = Control::noParent;
   
   uint16_t    mqttPort             = MQTT_PORT_DEF;
   String      mqttNameStr          = MQTT_NAME_DEF_STR;                // Control.
   String      mqttPwStr            = MQTT_PW_STR;                      // Control.
   String      mqttUserStr          = MQTT_ID_STR;                      // Control.
   String      MessageStr           = "";
   IPAddress   RemoteIp             = MQTT_IP_DEF;
   
const uint8_t  MQTT_FAIL_CNT       = 10;          // Maximum failed MQTT reconnects before disabling MQTT.
const uint16_t MQTT_KEEP_ALIVE     = 90;          // MQTT Keep Alive Time, in Secs.
const int32_t  MQTT_MSG_TIME       = 30000;       // MQTT Periodic Message Broadcast Time, in mS.
const uint8_t  MQTT_NAME_MAX_SZ    = 18;
const uint8_t  MQTT_PAYLD_MAX_SZ   = 100;         // Must be larger than RDS_TEXT_MAX_SZ.
const uint8_t  MQTT_PW_MAX_SZ      = 48;
const int32_t  MQTT_RECONNECT_TIME = 30000;       // MQTT Reconnect Delay Time, in mS;
const uint8_t  MQTT_RETRY_CNT      = 6;           // MQTT Reconnection Count (max attempts).
const uint8_t  MQTT_TOPIC_MAX_SZ   = 45;
const uint8_t  MQTT_USER_MAX_SZ    = 48;

   bool        refresh = true;
   long        previousMqttMillis = millis(); // Timer for MQTT services.
   float       oldVbatVolts = -1.0f;
   float       oldPaVolts = -1.0f;

// MQTT Command
#define MQTT_CMD_STR       F("/cmd/")                // Publish topic preamble, Client MQTT Command Preampble.

// MQTT Subscriptions
#define MQTT_CMD_SUB_STR   F("/cmd/#")               // MQTT wildcard Subscription, receive all /cmd messages.

// MQTT Publish Topics
#define MQTT_CONNECT_STR   F("/connect")               // Publish topic, Client MQTT Subscription.
#define MQTT_GPIO_STR      F("/gpio")                  // Publish topic, Client MQTT Subscription.
#define MQTT_INFORM_STR    F("/info")                  // Publish topic, Client MQTT Subscription.
#define MQTT_VOLTS_STR     F("/volts")                 // Publish topic, Client MQTT Subscription.


}; // c_ControllerMQTT

// *********************************************************************************************
// EOF
