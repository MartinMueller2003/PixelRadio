/*
   File: ControllerMQTT.cpp
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
#include <Arduino.h>
#include "ControllerMQTT.h"
#include "../language.h"
#include <ArduinoLog.h>

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

static String EmptyString = "";

// *********************************************************************************************
// class c_ControllerMQTT : public c_ControllerCommon

// *********************************************************************************************
c_ControllerMQTT::c_ControllerMQTT() : c_ControllerCommon("MQTT", c_ControllerMgr::ControllerTypeId_t::MQTT_CNTRL)
{
} // c_ControllerMQTT

// *********************************************************************************************
c_ControllerMQTT::~c_ControllerMQTT()
{
}

// *********************************************************************************************
void c_ControllerMQTT::Begin () 
{
   // DEBUG_START;
   mqttClient.setClient(wifiClient);

   if (wifiConnect ())
   { 
      // Connected to Web Server.
      Init (); // Init MQTT services.
   }
   // DEBUG_END;
} // Begin

// *************************************************************************************************************************
// mqttInit (): Initialize MQTT and subscribe with broker.
//             MQTT is disabled when in HotSpot mode.
//
void c_ControllerMQTT::Init (void)
{
   // DEBUG_START;

   String payloadStr;
   String topicStr;

   if (!ControllerEnabled)
   { // MQTT Controller Disabled.
      // DEBUG_V();
      OnlineFlag = false;
      return;
   }
   // DEBUG_V();

   if (WiFi.status () == WL_CONNECTED)
   {
      // DEBUG_V();
      Log.traceln (F ("Initializing MQTT"));
      mqttClient.setServer (RemoteIp, mqttPort);
      mqttClient.setCallback ([] (const char *topic, byte *payload, unsigned int length)
                             { 
                                c_ControllerMQTT *pMe = static_cast<c_ControllerMQTT*> (ControllerMgr.GetControllerById (MqttControllerId));
                                pMe->mqttClientCallback (topic, payload, length);
                             }); // Topic Subscription callback handler.

      mqttClient.setKeepAlive (MQTT_KEEP_ALIVE);
      // DEBUG_V();

      if (mqttClient.connect (mqttNameStr.c_str (), mqttUserStr.c_str (), mqttPwStr.c_str ()))
      {
         // DEBUG_V();
         OnlineFlag = true;
         topicStr = mqttNameStr;
         topicStr += MQTT_CONNECT_STR;
         payloadStr = F ("{\"boot\": 0}");                             // JSON Formatted payload.
         mqttClient.publish (topicStr.c_str (), payloadStr.c_str ()); // Publish reconnect status.

         Log.infoln ( (String (F ("-> MQTT Started. Sent Topic: ")) + topicStr + F (", Payload: ") + payloadStr).c_str ());

         updateUiMqttMsg (MQTT_ONLINE_STR);

         topicStr = mqttNameStr;
         topicStr += MQTT_CMD_SUB_STR;

         if (mqttClient.subscribe (topicStr.c_str ()))
         {
            // DEBUG_V();
            Log.infoln ( (String (F ("-> MQTT Successfully Subscribed to \"")) + topicStr + F ("\"")).c_str ());
         }
         else
         {
            // DEBUG_V();
            Log.errorln (F ("-> MQTT subscribe failed!"));
            updateUiMqttMsg (MQTT_SUBCR_FAIL_STR);
         }
      }
      else
      {
         // DEBUG_V();
         Log.errorln (F ("-> MQTT NOT Started."));
         OnlineFlag = false;
      }
   }
   else
   {
      // DEBUG_V();
      updateUiMqttMsg (MQTT_NOT_AVAIL_STR);
      Log.errorln (F ("WiFi Router Not Connected; MQTT Disabled."));
   }
   // DEBUG_END;

} // Init

// ************************************************************************************************
void c_ControllerMQTT::AddControls (uint16_t ctrlTab)
{
   // DEBUG_START;
   String tempStr;

   ESPUI.addControl(
      ControlType::Separator,
      CTRL_MQTT_SEP_STR, 
      EmptyString,
      ControlColor::None,
      ctrlTab);

   ControlerEnabledElementId = ESPUI.addControl (
      ControlType::Switcher,
      CTRL_MQTT_STR,
      ControllerEnabled ? "1" : "0",
      ControlColor::Turquoise,
      ctrlTab,
      [](Control *sender, int type, void* param)
      {
         reinterpret_cast<c_ControllerMQTT*>(param)->ControllerEnabledCb(sender, type);
      },
      this);

   if ((ControllerEnabled == true) &&
       ((INADDR_NONE != RemoteIp) || (!mqttNameStr.length()) ||
        (!mqttUserStr.length()  ) || (!mqttPwStr.length())))
   {
      // DEBUG_V();
      MessageStr = MQTT_MISSING_STR;
      ControllerEnabled = false; // Force DHCP mode.
   }
   else if (getWifiMode () == WIFI_AP)
   {
      // DEBUG_V();
      MessageStr = MQTT_NOT_AVAIL_STR;
   }
   else
   {
      // DEBUG_V();
      MessageStr = EmptyString;
   }

   EspuiMessageAreaId = ESPUI.addControl(ControlType::Label,
                                         "MSG_AREA",
                                         MessageStr,
                                         ControlColor::Turquoise,
                                         ControlerEnabledElementId);
   ESPUI.setElementStyle(EspuiMessageAreaId, CSS_LABEL_STYLE_BLACK);
   // DEBUG_V();

   EspuiNameID = ESPUI.addControl (ControlType::Text,
                                 MQTT_SUBSCR_NM_STR,
                                 mqttNameStr,
                                 ControlColor::Turquoise,
                                 ctrlTab,
                                 [](Control *sender, int type, void* param)
                                 {
                                    reinterpret_cast<c_ControllerMQTT*>(param)->setMqttNameCallback(sender, type);
                                 },
                                 this);

   EspuiIpID = ESPUI.addControl (ControlType::Text,
                                CTRL_MQTT_IP_STR,
                                RemoteIp.toString(),
                                ControlColor::Turquoise,
                                ctrlTab,
                                 [](Control *sender, int type, void* param)
                                 {
                                    reinterpret_cast<c_ControllerMQTT*>(param)->setRemoteIpAddrCallback(sender, type);
                                 },
                                 this);
   // DEBUG_V();

   EspuiPortID = ESPUI.addControl (ControlType::Label, 
                                  CTRL_MQTT_PORT_STR, 
                                  String (mqttPort), 
                                  ControlColor::Turquoise, 
                                  ctrlTab);
   ESPUI.setPanelStyle(EspuiPortID, String(F("font-size: 1.25em;")));
   ESPUI.setElementStyle(EspuiPortID, "max-width: 40%;");

   // DEBUG_V();

   EspuiUserID = ESPUI.addControl (ControlType::Text,
                                  CTRL_MQTT_USER_STR,
                                  mqttUserStr,
                                  ControlColor::Turquoise,
                                  ctrlTab,
                                 [](Control *sender, int type, void* param)
                                 {
                                    reinterpret_cast<c_ControllerMQTT*>(param)->setMqttAuthenticationCallback(sender, type);
                                 },
                                 this);
   // DEBUG_V();

   EspuiPwID = ESPUI.addControl (ControlType::Text,
                                CTRL_MQTT_PW_STR,
                                MQTT_PASS_HIDE_STR,
                                ControlColor::Turquoise,
                                ctrlTab,
                                 [](Control *sender, int type, void* param)
                                 {
                                    reinterpret_cast<c_ControllerMQTT*>(param)->setMqttAuthenticationCallback(sender, type);
                                 },
                                 this);
   // DEBUG_END;
} // AddControls

// *************************************************************************************************************************
// mqttClientCallback (): Support function for MQTT message reception.
// mqttClientCallback is limited to 255 byte packets (topic size + payload size). If larger, topic is corrupted and
// mqttClientCallback will not be processed. This is a limitation of the PubSubClient library.
void c_ControllerMQTT::mqttClientCallback (const char *topic, byte *payload, unsigned int length)
{
   // DEBUG_START;
   // char cBuff[length + 2];                    // Allocate Character buffer.
   // char logBuff[length + strlen (topic) + 60]; // Allocate a big buffer space.
   // char mqttBuff[140 + sizeof (VERSION_STR) + STA_NAME_MAX_SZ];
   String payloadStr;
   String topicStr;

   payloadStr.reserve (MQTT_PAYLD_MAX_SZ + 1);
   topicStr.reserve (MQTT_TOPIC_MAX_SZ + 1);
   // DEBUG_V();

   if (length > MQTT_PAYLD_MAX_SZ)
   {
      // DEBUG_V();
      Log.warningln ("MQTT Message Length (%u bytes) too long! Truncated to %u bytes.", length, MQTT_PAYLD_MAX_SZ);
      length = MQTT_PAYLD_MAX_SZ;
   }

   // Log.verboseln ("MQTT Message Length: %u", length);

   // Copy payload to local char array.
   {
      char cBuff[length + 2];
      memcpy (cBuff, payload, length);
      cBuff[length] = 0;
      payloadStr = cBuff;
   }
   payloadStr.trim (); // Remove leading/trailing spaces. Do NOT change to lowercase!
   // DEBUG_V();

   topicStr = topic;
   topicStr.trim ();        // Remove leading/trailing spaces.
   topicStr.toLowerCase (); // Force lowercase, prevent case matching problems.

   if (topicStr.length () > MQTT_TOPIC_MAX_SZ)
   {
      // DEBUG_V();
      topicStr = topicStr.substring (0, MQTT_TOPIC_MAX_SZ);
   }

   // char buff[topicStr.length () + payloadStr.length () + 30];
   // sprintf (buff, "MQTT Message Arrived, [Topic]: %s", topicStr.c_str ());
   // Log.infoln (buff);
   // sprintf (buff, "MQTT Message Arrived, Payload: %s", payloadStr.c_str ());
   // Log.infoln (buff);

   // *************************************************************************************************************************
   // START OF MQTT COMMAND ACTIONS:
   // DEBUG_V();

   if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_AUDMODE_STR))
   {
      // DEBUG_V();
      Log.infoln ("MQTT: Received Audio Mode Command");
      audioModeCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_FREQ_STR))
   {
      // DEBUG_V();
      Log.infoln ("MQTT: Received Radio Frequency Command");
      frequencyCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_GPIO19_STR))
   {
      // DEBUG_V();
      Log.infoln ("MQTT: Received GPIO19 Command");
      gpioMqttControl (payloadStr, GPIO19_PIN);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_GPIO23_STR))
   {
      // DEBUG_V();
      Log.infoln ("MQTT: Received GPIO23 Command");
      gpioMqttControl (payloadStr, GPIO23_PIN);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_GPIO33_STR))
   {
      // DEBUG_V();
      Log.infoln ("MQTT: Received GPIO33 Command");
      gpioMqttControl (payloadStr, GPIO33_PIN);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_INFO_STR))
   {
      // DEBUG_V();
      String Message;
      Log.infoln (F ("MQTT: Received System Information Command."));

      if (infoCmd (payloadStr, MqttControllerId))
      {
         // DEBUG_V();
         extern String staNameStr;
         Message = String (F ("{\"")) + CMD_INFO_STR +
                   F ("\": \"ok\", \"version\": \"") + VERSION_STR +
                   F ("\", \"hostName\": \"") + staNameStr +
                   F ("\", \"ip\": \"") + WiFi.localIP ().toString () +
                   F ("\", \"rssi\": ") + WiFi.RSSI () +
                   F (", \"status\": \"0x") + getControllerStatus ();
      }
      else
      {
      // DEBUG_V();
         Message = String (F ("{\"")) + CMD_INFO_STR + F ("\": \"fail\"}");
      }
      // DEBUG_V();
      topicStr = mqttNameStr + MQTT_INFORM_STR;
      mqttClient.publish (topicStr.c_str (), Message.c_str ());
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_MUTE_STR))
   {
      // DEBUG_V();
      Log.infoln (F ("MQTT: Received Audio Mute Command"));
      muteCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_PICODE_STR))
   {
      // DEBUG_V();
      Log.infoln (F ("MQTT Received RDS PI Code Command"));
      piCodeCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_PSN_STR))
   {
      // DEBUG_V();
      Log.infoln (F ("MQTT: Received RDS Program Station Name Command"));
      programServiceNameCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_RADIOTEXT_STR))
   {
      // DEBUG_V();
      Log.infoln (F ("MQTT: Received RadioText Command"));
      radioTextCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_REBOOT_STR))
   {
      // DEBUG_V();
      Log.infoln (F ("MQTT: Received System Reboot Command."));
      rebootCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_START_STR))
   {
      // DEBUG_V();
      Log.infoln (F ("MQTT: Received RDS Restart Command."));
      startCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_STOP_STR))
   {
      // DEBUG_V();
      Log.infoln (F ("MQTT: Received RDS Stop Command."));
      stopCmd (payloadStr, MqttControllerId);
   }
   else if (topicStr == mqttNameStr + makeMqttCmdStr (CMD_PERIOD_STR))
   {
      // DEBUG_V();
      Log.infoln (F ("MQTT: Received RDS Display Time Command"));
      rdsTimePeriodCmd (payloadStr, MqttControllerId);
   }
   else
   {
      // DEBUG_V();
      Log.errorln ( (String (F ("MQTT: Received Unknown Command (")) + topicStr + F ("), Ignored.")).c_str ());
   }
   // DEBUG_END;
} // mqttClientCallback

// *************************************************************************************************************************
// mqttReconnect (): If connection to broker has been lost then attempt reconnection.
//                  On entry, if resetFlg arg is true then reset timer and failcount.
void c_ControllerMQTT::mqttReconnect(bool resetFlg)
{
   // DEBUG_START;
   static uint8_t ClientConnectionRetryCount = 0; // MQTT Connection Retry Counter. Each reconnect attempt is allowed several tries.
   static long previousWiFiMillis = millis();     // Timer for WiFi services.
   String topicStr;

   do // once
   {
      // DEBUG_V();
      if (resetFlg == true)
      {
         // DEBUG_V();
         ClientConnectionRetryCount = 0;
         previousWiFiMillis = millis() - MQTT_RECONNECT_TIME + 2000; // Allow Reconnect in 2 secs.
         Log.traceln(F("MQTT Reconnect Fail Count has Been Reset."));
         break;
      }
      // DEBUG_V();

      if (!ControllerEnabled)
      { // MQTT Controller Disabled.
         // DEBUG_V();
         OnlineFlag = false;

         if (mqttClient.connected())
         {
            // DEBUG_V();
            mqttClient.disconnect();
            Log.traceln(F("MQTT Controller Disabled: Closed Connection."));
         }
         break;
      }
      // DEBUG_V();

      if (millis() - previousWiFiMillis < MQTT_RECONNECT_TIME)
      {
         // DEBUG_V();
         // keep waiting
         break;
      }
      previousWiFiMillis = millis();
      // DEBUG_V();

      if (WiFi.status() != WL_CONNECTED)
      {
         // DEBUG_V();
         updateUiMqttMsg(MQTT_NOT_AVAIL_STR);
         Log.errorln(F("-> WiFi Router Not Connected; MQTT Disabled."));
         break;
      }

      if (mqttClient.connected())
      {
         // DEBUG_V();
         break;
      }

      if (ClientConnectionRetryCount++ >= MQTT_RETRY_CNT)
      {
         // DEBUG_V();
         break;
      }
      // DEBUG_V();
      updateUiMqttMsg(MQTT_RETRY_STR);
      Log.infoln((String(F("Attempting MQTT Reconnection #")) + String(ClientConnectionRetryCount)).c_str());
      mqttClient.setServer(RemoteIp, mqttPort);
      mqttClient.setCallback([](const char *topic, byte *payload, unsigned int length)
                             { 
                                c_ControllerMQTT *pMe = static_cast<c_ControllerMQTT*> (ControllerMgr.GetControllerById(MqttControllerId));
                                pMe->mqttClientCallback (topic, payload, length);
                             }); // Topic Subscription callback handler.

      // DEBUG_V();
      mqttClient.setKeepAlive(MQTT_KEEP_ALIVE);

      // DEBUG_V();
      Log.verboseln((String(F("-> Broker Name: \"")) + mqttNameStr + "\"").c_str());
      Log.verboseln((String(F("-> Broker User: \"")) + mqttUserStr + F("\", Password: \"") + mqttPwStr + "\"").c_str());
      Log.verboseln((String(F("-> Broker IP: ")) + RemoteIp.toString() + F(", PORT: ") + String(mqttPort)).c_str());
      // DEBUG_V();

      if (mqttClient.connect(mqttNameStr.c_str(), mqttUserStr.c_str(), mqttPwStr.c_str()))
      { // Connect to MQTT Server
         // DEBUG_V();
         OnlineFlag = true;
         topicStr = mqttNameStr + MQTT_CONNECT_STR;
         String Payload;
         Payload = String(F("{\"reconnect\": ")) + String(ClientConnectionRetryCount) + F("}"); // JSON Formatted payload.
         // DEBUG_V();
         mqttClient.publish(topicStr.c_str(), Payload.c_str()); // Publish reconnect status.
         // DEBUG_V();

         Log.infoln((String(F("-> MQTT Reconnected. Sent Topic:")) + topicStr + F(", Payload:") + Payload).c_str());

         updateUiMqttMsg(MQTT_ONLINE_STR);
         // DEBUG_V();

         topicStr = mqttNameStr + MQTT_CMD_SUB_STR;
         // DEBUG_V();

         if (mqttClient.subscribe(topicStr.c_str()))
         {
            // DEBUG_V();
            Log.infoln((String(F("-> MQTT Successfully Subscribed to ")) + topicStr).c_str());
         }
         else
         {
            // DEBUG_V();
            Log.errorln(F("-> MQTT subscribe failed!"));
            updateUiMqttMsg(MQTT_SUBCR_FAIL_STR);
         }
         ClientConnectionRetryCount = 0; // Successful reconnect, OK to reset counter.
         break;
      }
      // DEBUG_V();

      if (ClientConnectionRetryCount < MQTT_RETRY_CNT)
      {
         // DEBUG_V();
         Log.warningln((String(F("-> MQTT Connection Failure #")) + String(ClientConnectionRetryCount) + F(", Code= ") + returnClientCode(mqttClient.state())).c_str());
         updateUiMqttMsg(MQTT_RETRY_FAIL_STR + String(ClientConnectionRetryCount));
         break;
      }
      // DEBUG_V();

      Log.errorln(F("-> MQTT Controller Has Been Disabled, Too Many Reconnect Attempts."));
      updateUiMqttMsg(MQTT_DISABLED_STR);

   } while (false);
   // DEBUG_END;
} // mqttReconnect

// ************************************************************************************************
void c_ControllerMQTT::ControllerEnabledCb (Control *sender, int type)
{
   // DEBUG_START;

   if (type == S_ACTIVE)
   {
      // DEBUG_V();
      TestParameters();
   }
   else
   {
      // DEBUG_V();
      ControllerEnabled = false; // Must set flag BEFORE mqqtReconnect!
      mqttReconnect (true);                                   // Reset MQTT Reconnect values when ctrlMqttFlg is false.
      OnlineFlag ? updateUiMqttMsg (MQTT_DISCONNECT_STR) : updateUiMqttMsg (EmptyString);
   }
   // DEBUG_V();

   displaySaveWarning();
   displayRdsText(); // Update RDS RadioText.
   Log.infoln((String(F("MQTT Controller Set to: ")) + String(ControllerEnabled ? "On" : "Off")).c_str());

   // DEBUG_END;

} // controllerCallback

// ************************************************************************************************
void c_ControllerMQTT::TestParameters()
{
   // DEBUG_START;

   if ((INADDR_NONE == RemoteIp  ) || 
       (0 == mqttNameStr.length()) ||
       (0 == mqttUserStr.length()) || 
       (0 == mqttPwStr.length()  ))
   { // Missing MQTT Entries
      // DEBUG_V();
      ControllerEnabled = false;
      updateUiMqttMsg(MQTT_MISSING_STR);
      ESPUI.print(ControlerEnabledElementId, "0"); // Turn off MQTT Controller Switcher.
      Log.warningln(F("Invalid MQTT Controller Settings, Disabled MQTT."));
   }
   else
   {
      // DEBUG_V();
      ControllerEnabled = false;
      updateUiMqttMsg(EmptyString);
      mqttReconnect(true); // Reset MQTT Reconnect values while ctrlMqttFlg is false.
      ControllerEnabled = true;  // Must set flag AFTER mqqtReconnect!
   }

   // DEBUG_END;

} // TestParameters

// ************************************************************************************************
// setMqttAuthenticationCallback (): Update MQTT Broker Credentials (User Name & Password).
void c_ControllerMQTT::setMqttAuthenticationCallback (Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("   ID: ") + String(sender->id));
   // DEBUG_V(String("value: ") + sender->value);

   String shortHideStr = MQTT_PASS_HIDE_STR;

   // Log.verboseln ( (String (F ("setMqttAuthenticationCallback ID : " + String (sender->id) + " , Value  : " + sender->value + " s "))).c_str ());

   if (sender->id == EspuiUserID)
   {
      // DEBUG_V();
      mqttUserStr = sender->value.substring (0, MQTT_USER_MAX_SZ);
      ESPUI.print (EspuiUserID, mqttUserStr);
      displaySaveWarning ();
      Log.infoln ( (String (F ("MQTT Broker Username Set to: \"")) + mqttUserStr + "\"").c_str ());
   }
   else if (sender->id == EspuiPwID)
   {
      // DEBUG_V();
      String tempStr = sender->value.substring (0, MQTT_PW_MAX_SZ);
      shortHideStr = shortHideStr.substring (0, 6); // Get first 5 chars of the "PW Hidden" text.

      if (tempStr.indexOf (shortHideStr) >= 0)
      { // User has accidentally deleted part of PW Hidden msg.
         // DEBUG_V();
         ESPUI.print (EspuiPwID, MQTT_PASS_HIDE_STR);

         // Log.infoln ( (String (F ("MQTT Broker Password Unchanged: ")) + mqttPwStr).c_str ()); // Show Password in log.
         Log.infoln ( (String (F ("MQTT Broker Password Unchanged: ")) + WIFI_PASS_HIDE_STR).c_str ());
      }
      else
      {
         // DEBUG_V();
         mqttPwStr = tempStr;
         ESPUI.print (EspuiPwID, mqttPwStr);
         displaySaveWarning ();
         Log.infoln ( (String (F ("MQTT Broker Password Set to: \"")) + mqttPwStr).c_str ());
      }
   }
   else
   {
      // DEBUG_V();
      Log.errorln((String(F("setMqttAuthenticationCallback: ")) + BAD_SENDER_STR).c_str());
   }

   TestParameters();

   // DEBUG_END;
} // setMqttAuthenticationCallback

// ************************************************************************************************
// setRemoteIpAddrCallback (): Update MQTT Broker IP Address.
void c_ControllerMQTT::setRemoteIpAddrCallback (Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("   ID: ") + String(sender->id));
   // DEBUG_V(String("value: ") + sender->value);

   if (sender->id == EspuiIpID)
   {
      // DEBUG_V();
      String tempStr = sender->value;

      if ((tempStr.length() == 0))
      {
         // DEBUG_V();
         ControllerEnabled = false;
         // RemoteIpStr = EmptyString;
         ESPUI.print(ControlerEnabledElementId, "0"); // Missing IP Addreess, Turn Off MQTT Controller.
         Log.errorln(F("setRemoteIpAddrCallback: Broker IP Address Erased. Disabled MQTT Controller."));
      }
      else
      {
         // DEBUG_V();
         IPAddress NewIpAddr = INADDR_NONE;
         bool IpAddrConverstionStatus = NewIpAddr.fromString(tempStr); // Convert to IP Class Array.
         // DEBUG_V(String("   IP: ") + NewIpAddr.toString());
         // DEBUG_V(String("Status: ") + String(IpAddrConverstionStatus));

         if ((INADDR_NONE == NewIpAddr) || (false == IpAddrConverstionStatus))
         {
            // DEBUG_V();
            NewIpAddr = RemoteIp;
            ESPUI.print(EspuiIpID, RemoteIp.toString().c_str());
            Log.errorln(F("setRemoteIpAddrCallback: Broker IP Invalid, Ignored."));
         }
         else
         {
            // DEBUG_V();
            RemoteIp = NewIpAddr; // Convert IP to String.
            ESPUI.print(EspuiIpID, RemoteIp.toString().c_str());
            displaySaveWarning();
            Log.infoln((String(F("MQTT Broker IP Set to: ")) + RemoteIp.toString()).c_str());
         }
         // DEBUG_V();
      }
      // DEBUG_V();
   }
   else
   {
      // DEBUG_V();
      Log.errorln((String(F("setRemoteIpAddrCallback: ")) + F(BAD_SENDER_STR)).c_str());
   }
   // DEBUG_END;
} // setRemoteIpAddrCallback

// ************************************************************************************************
// setMqttNameCallback (): Update MQTT Device Name.
void c_ControllerMQTT::setMqttNameCallback (Control *sender, int type)
{
   // DEBUG_START;

   // DEBUG_V(String("   ID: ") + String(sender->id));
   // DEBUG_V(String("value: ") + sender->value);

   if (sender->id == EspuiNameID)
   {
      // DEBUG_V();
      mqttNameStr = sender->value;
      mqttNameStr = mqttNameStr.substring (0, MQTT_NAME_MAX_SZ);

      if (mqttNameStr.isEmpty())
      {
         // DEBUG_V();
         mqttNameStr = MQTT_NAME_DEF_STR;
      }
      // DEBUG_V();
      ESPUI.print (EspuiNameID, mqttNameStr);
      displaySaveWarning ();
      Log.infoln ( (String (F ("MQTT Device Name Set to: \"")) + mqttNameStr + "\"").c_str ());
   }
   else
   {
      // DEBUG_V();
      Log.errorln ( (String (F ("setMqttNameCallback: ")) + BAD_SENDER_STR).c_str ());
   }
   // DEBUG_END;
} // setMqttNameCallback

// *********************************************************************************************
void c_ControllerMQTT::RestoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::RestoreConfiguration(config);

   if (true == config.containsKey(F(N_MQTT_NAME_STR)))
   {
      // DEBUG_V();
      mqttNameStr = (const char *)config[F(N_MQTT_NAME_STR)];
   }

   if (true == config.containsKey(F(N_MQTT_PW_STR)))
   {
      // DEBUG_V();
      mqttPwStr = (const char *)config[F(N_MQTT_PW_STR)];
   }

   if (true == config.containsKey(F(N_MQTT_USER_STR)))
   {
      // DEBUG_V();
      mqttUserStr = (const char *)config[F(N_MQTT_USER_STR)];
   }

   if (true == config.containsKey(F(N_MQTT_IP_STR)))
   {
      // DEBUG_V();
      String Temp = (const char *)config[F(N_MQTT_IP_STR)];
      RemoteIp.fromString(Temp);
   }
   
   // DEBUG_END;
} // RestoreConfiguration

// *********************************************************************************************
void c_ControllerMQTT::SaveConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::SaveConfiguration(config);

   config[N_MQTT_NAME_STR] = mqttNameStr;
   config[N_MQTT_PW_STR]   = mqttPwStr;
   config[N_MQTT_USER_STR] = mqttUserStr;
   config[N_MQTT_IP_STR]   = RemoteIp.toString();

   // DEBUG_END;
} // SaveConfiguration

// *************************************************************************************************************************
// gpioMqttControl (): MQTT handler for GPIO Commands. This is a companion to mqttCallback ().
void c_ControllerMQTT::gpioMqttControl (String payloadStr, gpio_num_t pin)
{
   // DEBUG_START;
   bool successFlg = true;
   String topicStr;
   String CmdString;

   topicStr = Name + MQTT_GPIO_STR;

   Log.infoln ( (String (F ("-> MQTT Controller: Received GPIO Pin-")) + String (pin) + " Command").c_str ());

   successFlg = gpioCmd (payloadStr, MqttControllerId, pin);
   // DEBUG_V();

   if (!successFlg)
   {
      // DEBUG_V();
      CmdString = String (F ("{\"")) + CMD_GPIO_STR + String (pin) + F ("\": \"fail\"}");
   }
   else if (payloadStr == CMD_GPIO_READ_STR)
   {
      // DEBUG_V();
      CmdString = String (F ("{\"")) + CMD_GPIO_STR + String (pin) + F ("\": \"") + String (digitalRead (pin)) + F ("\"}");
   }
   else
   {
      // DEBUG_V();
      CmdString = String (F ("{\"")) + CMD_GPIO_STR + String (pin) + String (F ("\": \"ok\"}"));
   }
   mqttClient.publish (topicStr.c_str (), CmdString.c_str ());
   // DEBUG_END;
}

// ************************************************************************************************
// makeMqttCmdStr (): Return the MQTT command string. On entry cmdStr has command keyword.
//                   This is a companion to mqttCallback ().
String c_ControllerMQTT::makeMqttCmdStr (String cmdStr)
{
   return String (MQTT_CMD_STR) + cmdStr;
} // makeMqttCmdStr

// *************************************************************************************************************************
// mqttSendMessages(): Broadcast system messages to MQTT broker.
// The messages are sent periodically (MQTT_MSG_TIME) or immediately whenever they change.
void c_ControllerMQTT::mqttSendMessages(void)
{
   // DEBUG_START;
   do // once
   {
      String Payload;
      String topicStr;

      if (!ControllerEnabled)
      { // MQTT Controller Disabled.
         // DEBUG_V();
         break;
      }

      if (!mqttClient.connected())
      { // MQTT not connected, nothing to do. Exit.
         // DEBUG_V();
         break;
      }

      if (millis() - previousMqttMillis >= MQTT_MSG_TIME)
      {
         // DEBUG_V();
         previousMqttMillis = millis();
         refresh = true;
      }
      // DEBUG_V();

      extern float paVolts;
      extern float vbatVolts;
      if (refresh ||
          ((vbatVolts > oldVbatVolts + VOLTS_HYSTERESIS) || (vbatVolts < oldVbatVolts - VOLTS_HYSTERESIS)) ||
          ((paVolts > oldPaVolts + VOLTS_HYSTERESIS) || (paVolts < oldPaVolts - VOLTS_HYSTERESIS)))
      {
         // DEBUG_V();
         oldVbatVolts = vbatVolts;
         oldPaVolts = paVolts;
         topicStr = mqttNameStr + MQTT_VOLTS_STR;
         // JSON Formatted Payload.
         Payload = String(F("{\"vbat\": ")) + String(vbatVolts) + F(",\"pa\": ") + String(paVolts);
         mqttClient.publish(topicStr.c_str(), Payload.c_str()); // Publish Power Supply Voltage.

         Log.infoln((String(F("MQTT Publish, [Topic]: ")) + topicStr).c_str());
         Log.infoln((String(F("-> Payload: ")) + Payload).c_str());

         refresh = false;
         // DEBUG_V();
      }
   } while (false);

   // DEBUG_END;
} // mqttSendMessages

// *********************************************************************************************
void c_ControllerMQTT::poll(void)
{
   if (mqttClient.connected())
   {
      mqttClient.loop(); // Service MQTT background tasks.
      mqttSendMessages();
   }
   else
   {
      mqttReconnect(false);
   }
} // poll

// *************************************************************************************************************************
// returnClientCode(): Convert Client.state() code to String Message.
/* MQTT_CONNECTION_TIMEOUT     -4
   MQTT_CONNECTION_LOST        -3
   MQTT_CONNECT_FAILED         -2
   MQTT_DISCONNECTED           -1
   MQTT_CONNECTED               0
   MQTT_CONNECT_BAD_PROTOCOL    1
   MQTT_CONNECT_BAD_CLIENT_ID   2
   MQTT_CONNECT_UNAVAILABLE     3
   MQTT_CONNECT_BAD_CREDENTIALS 4
   MQTT_CONNECT_UNAUTHORIZED    5
 */
String c_ControllerMQTT::returnClientCode(int code)
{
   String Response;

   switch (code)
   {
   case MQTT_CONNECTION_TIMEOUT:
      Response = "MQTT_CONNECTION_TIMEOUT";
      break;
   case MQTT_CONNECTION_LOST:
      Response = "MQTT_CONNECTION_LOST";
      break;
   case MQTT_CONNECT_FAILED:
      Response = "MQTT_CONNECT_FAILED";
      break;
   case MQTT_DISCONNECTED:
      Response = "MQTT_DISCONNECTED";
      break;
   case MQTT_CONNECTED:
      Response = "MQTT_CONNECTED";
      break;
   case MQTT_CONNECT_BAD_PROTOCOL:
      Response = "MQTT_CONNECT_BAD_PROTOCOL";
      break;
   case MQTT_CONNECT_BAD_CLIENT_ID:
      Response = "MQTT_CONNECT_BAD_CLIENT_ID";
      break;
   case MQTT_CONNECT_UNAVAILABLE:
      Response = "MQTT_CONNECT_UNAVAILABLE";
      break;
   case MQTT_CONNECT_BAD_CREDENTIALS:
      Response = "MQTT_CONNECT_BAD_CREDENTIALS";
      break;
   case MQTT_CONNECT_UNAUTHORIZED:
      Response = "MQTT_CONNECT_UNAUTHORIZED";
      break;
   default:
      Response = "Undefined";
      break;
   }
   return Response;
} //

// ************************************************************************************************
// updateUiMqttMsg(): Update the MQTT CONTROL panel's status message on the ctrlTab.
void c_ControllerMQTT::updateUiMqttMsg(String msgStr)
{
   ESPUI.print(EspuiMessageAreaId, msgStr);
}

// *********************************************************************************************
// EOF
