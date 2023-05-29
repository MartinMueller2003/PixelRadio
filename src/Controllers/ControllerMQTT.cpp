/*
  *    File: ControllerMQTT.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.0
  *    Creation: Dec-16-2021
  *    Revised:  Apr-06-2022
  *    Public Release:
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *    Revision History: See PixelRadio.cpp
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
  *    absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *     Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
  */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <WiFi.h>

#include "ControllerMgr.h"
#include "ControllerMQTT.h"
#include "language.h"

#include "MqttName.hpp"
#include "MqttBrokerIpAddress.hpp"
#include "MqttPassword.hpp"
#include "MqttUser.hpp"
#include "MqttPort.hpp"
#include "SystemVoltage.hpp"
#include "RfPaVoltage.hpp"
#include "memdebug.h"

// *********************************************************************************************
static const PROGMEM char   Name []             = "MQTT";
static const PROGMEM char   MQTT_SUBCR_FAIL []  = "MQTT SUBCRIBE FAILED";
static const PROGMEM char   MQTT_NOT_AVAIL []   = "MQTT NOT AVAILABLE IN AP MODE";
static const PROGMEM char   MQTT_RETRY []       = "RETRY CONNECTION";
static const PROGMEM char   MQTT_DISABLED []    = "CONNECTION FAILED!<br>DISABLED MQTT CONTROLLER";
static const PROGMEM char   MQTT_MISSING []     = "MISSING SETTINGS : MQTT DISABLED";
static const PROGMEM char   MQTT_CMD_STR []     = "/cmd/";  // Publish topic preamble, Client MQTT Command Preampble.

static const String MQTT_SUBCR_FAIL_STR = MQTT_SUBCR_FAIL;
static const String MQTT_NOT_AVAIL_STR  = MQTT_NOT_AVAIL;
static const String MQTT_RETRY_STR      = MQTT_RETRY;
static const String MQTT_DISABLED_STR   = MQTT_DISABLED;
static const String MQTT_MISSING_STR    = MQTT_MISSING;

fsm_Connection_state_disabled fsm_Connection_state_disabled_imp;
fsm_Connection_state_WaitForWiFi fsm_Connection_state_WaitForWiFi_imp;
fsm_Connection_state_ValidateConfig fsm_Connection_state_ValidateConfig_imp;
fsm_Connection_state_connecting fsm_Connection_state_connecting_imp;
fsm_Connection_state_connected  fsm_Connection_state_connected_imp;
fsm_Connection_state_Disconnecting fsm_Connection_state_Disconnecting_imp;

static cControlCommon * ListOfControls [] =
{
    & MqttName,
    & MqttBrokerIpAddress,
    & MqttPort,
    & MqttUser,
    & MqttPassword
};

static const uint8_t MQTT_FAIL_CNT          = 10;       // Maximum failed MQTT reconnects before disabling MQTT.
static const uint16_t   MQTT_KEEP_ALIVE     = 90;       // MQTT Keep Alive Time, in Secs.
static const int32_t    MQTT_MSG_TIME       = 30000;    // MQTT Periodic Message Broadcast Time, in mS.
static const uint8_t    MQTT_PAYLD_MAX_SZ   = 100;      // Must be larger than RDS_TEXT_MAX_SZ.
static const uint8_t    MQTT_PW_MAX_SZ      = 48;
static const int32_t    MQTT_RECONNECT_TIME = 30000;    // MQTT Reconnect Delay Time, in mS;
static const uint8_t    MQTT_RETRY_CNT      = 6;        // MQTT Reconnection Count (max attempts).
static const uint8_t    MQTT_TOPIC_MAX_SZ   = 45;
static const uint8_t    MQTT_USER_MAX_SZ    = 48;

// *********************************************************************************************
c_ControllerMQTT::c_ControllerMQTT () :   cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::MQTT_CNTRL)
{}  // c_ControllerMQTT

// *********************************************************************************************
c_ControllerMQTT::~c_ControllerMQTT ()
{}

// ************************************************************************************************
void c_ControllerMQTT::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cControllerCommon::AddControls (TabId, color);
    setMessageStyle (eCssStyle::CssStyleWhite);

    for (auto CurrentControl : ListOfControls)
    {
        CurrentControl->AddControls (
            TabId,
            color);
    }

    // DEBUG_END;
}   // AddControls

// *********************************************************************************************
void c_ControllerMQTT::begin ()
{
    // DEBUG_START;

    fsm_Connection_state_disabled_imp.SetParent (this);
    fsm_Connection_state_WaitForWiFi_imp.SetParent (this);
    fsm_Connection_state_ValidateConfig_imp.SetParent (this);
    fsm_Connection_state_connecting_imp.SetParent (this);
    fsm_Connection_state_connected_imp.SetParent (this);
    fsm_Connection_state_Disconnecting_imp.SetParent (this);

    fsm_Connection_state_disabled_imp.Init ();

    mqttClient.setClient (wifiClient);
    mqttClient.setCallback (
        [] (const char * topic, byte * payload, unsigned int length)
        {
            // Serial.println("Got a callback");
            c_ControllerMQTT * pMe = static_cast <c_ControllerMQTT *> (ControllerMgr.GetControllerById (MqttControllerId));
            pMe->mqttClientCallback (topic, payload, length);
        });  // Topic Subscription callback handler.

    // DEBUG_END;
}   // Begin

// *********************************************************************************************
bool c_ControllerMQTT::ConfigHasChanged ()
{
    // DEBUG_START;

    bool Response = false;

    for (auto * CurrentControl : ListOfControls)
    {
        Response |= CurrentControl->GetAndResetValueChangedFlag ();
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void c_ControllerMQTT::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response) {Messages.GetNextRdsMessage (Response);}

// *************************************************************************************************************************
void c_ControllerMQTT::mqttClientCallback (const char * topic, byte * payload, unsigned int length)
{
    // DEBUG_START;

    if (pCurrentFsmState)
    {
        pCurrentFsmState->mqttClientCallback (topic, payload, length);
    }

    // DEBUG_END;
}   // mqttClientCallback

// *********************************************************************************************
void c_ControllerMQTT::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::restoreConfiguration (config);

    for (auto CurrentControl : ListOfControls)
    {
        CurrentControl->restoreConfiguration (config);
    }

    // DEBUG_END;
}   // restoreConfiguration

// *********************************************************************************************
void c_ControllerMQTT::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::saveConfiguration (config);

    for (auto CurrentControl : ListOfControls)
    {
        CurrentControl->saveConfiguration (config);
    }

    // DEBUG_END;
}   // saveConfiguration

// ************************************************************************************************
// makeMqttCmdStr (): Return the MQTT command string. On entry cmdStr has command keyword.
//                   This is a companion to mqttCallback ().
String c_ControllerMQTT::makeMqttCmdStr (String cmdStr) {return String (MQTT_CMD_STR) + cmdStr;}    // makeMqttCmdStr

// *************************************************************************************************************************
// Broadcast system messages to MQTT broker.
void c_ControllerMQTT::SendStatusMessage (void)
{
    // DEBUG_START;
    String  Payload;
    String  topicStr;

    // DEBUG_V();
    oldVbatVolts    = SystemVoltage.GetVoltage ();
    oldPaVolts      = RfPaVoltage.GetVoltage ();
    topicStr        = MqttName.get () + MQTT_VOLTS_STR;

    StaticJsonDocument <256> doc;
    doc[F ("vbat")] = String (SystemVoltage.GetVoltage ());
    doc[F ("pa")]   = String (RfPaVoltage.GetVoltage ());
    serializeJson (doc, Payload);

    // JSON Formatted Payload.
    mqttClient.publish (topicStr.c_str (), Payload.c_str ());   // Publish Power Supply Voltage.

    Log.infoln ((String (F ("MQTT Publish, [Topic]: ")) + topicStr).c_str ());
    Log.infoln ((String (F ("-> Payload: ")) + Payload).c_str ());

    // DEBUG_V();

    // DEBUG_END;
}   // mqttSendMessages

// *************************************************************************************************************************
bool c_ControllerMQTT::VoltagesHaveChanged ()
{
    // DEBUG_START;
    bool Response = false;

    float   paVolts     = RfPaVoltage.GetVoltage ();
    float   vbatVolts   = SystemVoltage.GetVoltage ();

    if (((vbatVolts > oldVbatVolts + VOLTS_HYSTERESIS) ||
         (vbatVolts < oldVbatVolts - VOLTS_HYSTERESIS)) ||
        ((paVolts > oldPaVolts + VOLTS_HYSTERESIS) ||
         (paVolts < oldPaVolts - VOLTS_HYSTERESIS)))
    {
        Response = true;
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void c_ControllerMQTT::poll (void)
{
    // _ DEBUG_START;

    uint32_t Now = millis ();

    mqttClient.loop ();

    if (Now > FsmTimerExpirationTime)
    {
        FsmTimerExpirationTime += 1000;

        if (pCurrentFsmState)
        {
            pCurrentFsmState->Poll (Now);
        }
        else
        {
            // DEBUG_V("pCurrentFsmState not set");
        }
    }

    // _ DEBUG_END;
}   // poll

// *********************************************************************************************
bool c_ControllerMQTT::ValidateConfiguration ()
{
    // DEBUG_START;

    bool response = false;

    do  // once
    {
        // clear this flag for later
        ConfigHasChanged ();

        String Temp = MqttName.get ();

        if (Temp.isEmpty ())
        {
            setMessage (F ("Broker Subscriber Name is not set"), cControlCommon::eCssStyle::CssStyleRed_bw);
            break;
        }

        if (MqttBrokerIpAddress.GetIpAddress () == IPAddress (uint32_t (0)))
        {
            setMessage (F ("Broker IP Address is not set"), cControlCommon::eCssStyle::CssStyleRed_bw);
            break;
        }

        // MqttPort: port is always a valid number

        Temp = MqttUser.get ();

        if (Temp.isEmpty () || Temp.equals (MqttUser.getDefault ()))
        {
            setMessage (F ("Broker Username Name is not set"), cControlCommon::eCssStyle::CssStyleRed_bw);
            break;
        }

        Temp = MqttPassword.get ();

        if (Temp.isEmpty () || Temp.equals (MqttPassword.getDefault ()))
        {
            setMessage (F ("Broker Password is not set"), cControlCommon::eCssStyle::CssStyleRed_bw);
            break;
        }

        // DEBUG_V("Made it to the end");
        response = true;
    } while (false);

    // DEBUG_END;
    return response;
}

// *********************************************************************************************
// *********************************************************************************************
// *********************************************************************************************
void fsm_Connection_state_disabled::Init ()
{
    // DEBUG_START;

    pParent->pCurrentFsmState = this;
    pParent->setMessage (F ("Disabled"), cControlCommon::eCssStyle::CssStyleWhite);

    // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_disabled::Poll (uint32_t Now)
{
    // DEBUG_START;

    if (pParent->getBool ())
    {
        fsm_Connection_state_ValidateConfig_imp.Init ();
    }

    // DEBUG_END;
}

// *********************************************************************************************
// *********************************************************************************************
void fsm_Connection_state_ValidateConfig::Init ()
{
    // DEBUG_START;

    pParent->pCurrentFsmState = this;
    pParent->ValidateConfiguration ();

    // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_ValidateConfig::Poll (uint32_t Now)
{
    // DEBUG_START;

    if (!pParent->getBool ())
    {
        fsm_Connection_state_Disconnecting_imp.Init ();
    }
    else if (pParent->ValidateConfiguration ())
    {
        fsm_Connection_state_WaitForWiFi_imp.Init ();
    }
    else
    {
        // DEBUG_V("Waiting for a valid configuration");
    }

    // DEBUG_END;
}

// *********************************************************************************************
// *********************************************************************************************
void fsm_Connection_state_WaitForWiFi::Init ()
{
    // DEBUG_START;

    pParent->pCurrentFsmState = this;
    pParent->setMessage (F ("Waiting For WiFi to come up"), cControlCommon::eCssStyle::CssStyleWhite);

    // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_WaitForWiFi::Poll (uint32_t Now)
{
    // DEBUG_START;

    if ((!pParent->getBool ()) ||
        (pParent->ConfigHasChanged ()) ||
        (!pParent->ValidateConfiguration ()))
    {
        fsm_Connection_state_Disconnecting_imp.Init ();
    }
    else if (WiFi.status () == WL_CONNECTED)
    {
        fsm_Connection_state_connecting_imp.Init ();
    }
    else
    {
        // DEBUG_V("if wifi is up then move to the connecting state");
    }

    // DEBUG_END;
}

// *********************************************************************************************
// *********************************************************************************************
void fsm_Connection_state_connecting::Init ()
{
    // DEBUG_START;

    pParent->pCurrentFsmState = this;
    pParent->setMessage (F ("Connecting to Broker"), cControlCommon::eCssStyle::CssStyleWhite);

    Log.traceln (F ("Initializing MQTT"));

    if (pParent->mqttClient.connected ())
    {
        // DEBUG_V();
        fsm_Connection_state_Disconnecting_imp.Init ();
    }
    else
    {
        pParent->mqttClient.setServer (MqttBrokerIpAddress.GetIpAddress (), MqttPort.get32 ());
        /*
          *         mqttClient.setCallback ([] (const char * topic, byte * payload, unsigned int length)
          *                                 {
          *                                     c_ControllerMQTT * pMe =
          *                                         static_cast <c_ControllerMQTT *> (ControllerMgr.GetControllerById (MqttControllerId));
          *                                     pMe->mqttClientCallback (topic, payload, length);
          *                                 }); // Topic Subscription callback handler.
          */
        pParent->mqttClient.setKeepAlive (MQTT_KEEP_ALIVE);
        pParent->mqttClient.connect (MqttName.get ().c_str (), MqttUser.get ().c_str (), MqttPassword.get ().c_str ());
    }

    // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_connecting::Poll (uint32_t Now)
{
    // DEBUG_START;

    if ((!pParent->getBool ()) ||
        (WiFi.status () != WL_CONNECTED) ||
        (pParent->ConfigHasChanged ()) ||
        (!pParent->ValidateConfiguration ()))
    {
        fsm_Connection_state_Disconnecting_imp.Init ();
    }
    else if (pParent->mqttClient.connected ())
    {
        // DEBUG_V("mqtt has connected, move to the connected state.");
        fsm_Connection_state_connected_imp.Init ();
    }

    // DEBUG_END;
}

// *********************************************************************************************
// *********************************************************************************************
void fsm_Connection_state_connected::Init ()
{
    // DEBUG_START;

    pParent->pCurrentFsmState   = this;
    NextStatusMessageMS         = millis ();    // send a status message right away
    pParent->setMessage (F ("Connected to Broker"), cControlCommon::eCssStyle::CssStyleWhite);

    // DEBUG_V();
    String topicStr;
    topicStr    = MqttName.get ();
    topicStr    += MQTT_CONNECT_STR;
    String payloadStr = F ("{\"boot\": 0}");                                // JSON Formatted payload.
    pParent->mqttClient.publish (topicStr.c_str (), payloadStr.c_str ());   // Publish reconnect status.

    Log.infoln ((String (F ("-> MQTT Started. Sent Topic: ")) + topicStr + F (", Payload: ") + payloadStr).c_str ());

    topicStr    = MqttName.get ();
    topicStr    += MQTT_CMD_SUB_STR;

    // if (pParent->mqttClient.subscribe ("pixelradio/#"))
    if (pParent->mqttClient.subscribe (topicStr.c_str ()))
    {
        // DEBUG_V();
        Log.infoln ((String (F ("-> MQTT Successfully Subscribed to \"")) + topicStr + F ("\"")).c_str ());
    }
    else
    {
        // DEBUG_V();
        Log.errorln (F ("-> MQTT subscribe failed!"));
        fsm_Connection_state_Disconnecting_imp.Init ();
    }

    // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_connected::Poll (uint32_t Now)
{
    // DEBUG_START;

    if ((!pParent->getBool ()) ||
        (pParent->ConfigHasChanged ()) ||
        (WiFi.status () != WL_CONNECTED) ||
        (!pParent->ValidateConfiguration ()) ||
        (!pParent->mqttClient.connected ()))
    {
        fsm_Connection_state_Disconnecting_imp.Init ();
    }
    // The messages are sent periodically or immediately whenever they change.
    else if ((Now > NextStatusMessageMS) || (pParent->VoltagesHaveChanged ()))
    {
        NextStatusMessageMS += MQTT_MSG_TIME;
        pParent->SendStatusMessage ();
    }
    else
    {
        // DEBUG_V("Nothing to do");
    }

    // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_connected::mqttClientCallback (const char * topic, byte * payload, unsigned int length)
{
    // DEBUG_START;

    do  // once
    {
        String  payloadStr;
        String  topicStr;

        payloadStr.reserve (MQTT_PAYLD_MAX_SZ + 1);
        topicStr.reserve (MQTT_TOPIC_MAX_SZ + 1);

        topicStr = topic;
        topicStr.trim ();           // Remove leading/trailing spaces.
        topicStr.toLowerCase ();    // Force lowercase, prevent case matching problems.

        // DEBUG_V(String("topicStr: ") + topicStr);
        if (topicStr.length () > MQTT_TOPIC_MAX_SZ)
        {
            // DEBUG_V();
            Log.warningln (String (F ("MQTT Topic Length (%u bytes) is too long! Discarding message")).c_str (), topicStr.length ());
            break;
        }

        if (length > MQTT_PAYLD_MAX_SZ)
        {
            // DEBUG_V();
            Log.warningln (String (F ("MQTT Message Length (%u bytes) is too long! Discarding message")).c_str (), length);
            break;
        }

        payloadStr = String (payload, length);
        // DEBUG_V(String("payloadStr: ") + payloadStr);

        // topic contains the command in the format: subscriberTopicName/cmd/???
        if (0 != topicStr.indexOf (MqttName.get ()))
        {
            Log.warningln ((String (F ("MQTT Subscription Topic '")) + topicStr + F ("' does not match '") + MqttName.get () + F ("'")).c_str ());
            break;
        }

        String Command = topicStr.substring (MqttName.get ().length () + strlen (MQTT_CMD_STR));
        // DEBUG_V(String("Command: ") + Command);

        String Response;
        CommandProcessor.ProcessCommand (Command, payloadStr, Response);
        // DEBUG_V(String("Response: ") + Response);
        Response += F ("\n");
        pParent->mqttClient.publish (
            String (MqttName.get () + MQTT_INFORM_STR).c_str (),
            String (String (F ("Response: ")) + Response).c_str ());

        DynamicJsonDocument mqttMsg (1024);
        mqttMsg[CMD_INFO_STR]   = F ("ok");
        mqttMsg[F ("version")]  = VERSION_STR;
        mqttMsg[F ("hostName")] = WiFi.getHostname ();
        mqttMsg[F ("ip")]       = WiFi.localIP ().toString ();
        mqttMsg[F ("rssi")]     = WiFi.RSSI ();
        mqttMsg[F ("status")]   = ControllerMgr.getControllerStatusSummary ();
        String mqttStr;
        mqttStr.reserve (1024);
        serializeJson (mqttMsg, mqttStr);
        topicStr = MqttName.get () + MQTT_INFORM_STR;
        pParent->mqttClient.publish (topicStr.c_str (), mqttStr.c_str ());
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
// *********************************************************************************************
void fsm_Connection_state_Disconnecting::Init ()
{
    // DEBUG_START;

    pParent->pCurrentFsmState = this;
    pParent->setMessage (F ("Disconnecting From Broker"), cControlCommon::eCssStyle::CssStyleWhite);

    if (pParent->mqttClient.connected ())
    {
        // DEBUG_V();
        pParent->mqttClient.disconnect ();
        Log.traceln (F ("MQTT Controller Disabled: Closed Connection."));
    }

    // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_Disconnecting::Poll (uint32_t Now)
{
    // DEBUG_START;

    fsm_Connection_state_disabled_imp.Init ();

    // DEBUG_END;
}

// *********************************************************************************************
// EOF
