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

#include "memdebug.h"

// *********************************************************************************************
static const PROGMEM char   Name []             = "MQTT";
static const PROGMEM char   MQTT_SUBCR_FAIL []  = "MQTT SUBCRIBE FAILED";
static const PROGMEM char   MQTT_NOT_AVAIL []   = "MQTT NOT AVAILABLE IN AP MODE";
static const PROGMEM char   MQTT_RETRY []       = "RETRY CONNECTION";
static const PROGMEM char   MQTT_DISABLED []    = "CONNECTION FAILED!<br>DISABLED MQTT CONTROLLER";
static const PROGMEM char   MQTT_MISSING []     = "MISSING SETTINGS : MQTT DISABLED";

static const String MQTT_SUBCR_FAIL_STR = MQTT_SUBCR_FAIL;
static const String MQTT_NOT_AVAIL_STR  = MQTT_NOT_AVAIL;
static const String MQTT_RETRY_STR      = MQTT_RETRY;
static const String MQTT_DISABLED_STR   = MQTT_DISABLED;
static const String MQTT_MISSING_STR    = MQTT_MISSING;

fsm_Connection_state_disabled       fsm_Connection_state_disabled_imp;
fsm_Connection_state_WaitForWiFi    fsm_Connection_state_WaitForWiFi_imp;
fsm_Connection_state_ValidateConfig fsm_Connection_state_ValidateConfig_imp;
fsm_Connection_state_connecting     fsm_Connection_state_connecting_imp;
fsm_Connection_state_connected      fsm_Connection_state_connected_imp;
fsm_Connection_state_Disconnecting  fsm_Connection_state_Disconnecting_imp;

static cControlCommon * ListOfControls [] =
{
    & MqttName,
    & MqttBrokerIpAddress,
    & MqttPort,
    & MqttUser,
    & MqttPassword
};

static    const uint8_t MQTT_FAIL_CNT         = 10;       // Maximum failed MQTT reconnects before disabling MQTT.
static    const uint16_t MQTT_KEEP_ALIVE      = 90;       // MQTT Keep Alive Time, in Secs.
static    const int32_t MQTT_MSG_TIME         = 30000;    // MQTT Periodic Message Broadcast Time, in mS.
static    const uint8_t MQTT_PAYLD_MAX_SZ     = 100;      // Must be larger than RDS_TEXT_MAX_SZ.
static    const uint8_t MQTT_PW_MAX_SZ        = 48;
static    const int32_t MQTT_RECONNECT_TIME   = 30000;    // MQTT Reconnect Delay Time, in mS;
static    const uint8_t MQTT_RETRY_CNT        = 6;        // MQTT Reconnection Count (max attempts).
static    const uint8_t MQTT_TOPIC_MAX_SZ     = 45;
static    const uint8_t MQTT_USER_MAX_SZ      = 48;

// *********************************************************************************************
c_ControllerMQTT::c_ControllerMQTT () :   cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::MQTT_CNTRL)
{
}   // c_ControllerMQTT

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
        CurrentControl->AddControls (TabId, color);
    }

#ifdef OldWay

        String tempStr;

        if ((ControllerIsEnabled () == true) &&
        ((INADDR_NONE != MqttBrokerIpAddress.getIpaddress ()) || (!MqttName.get ().length ()) ||
         (!MqttUser.get ().length ()) || (!MqttPassword.get ().length ())))
        {
            // DEBUG_V();
            MessageStr = MQTT_MISSING_STR;
        }

        else if (getWifiMode () == WIFI_AP)
        {
            // DEBUG_V();
            MessageStr = MQTT_NOT_AVAIL_STR;
        }
        else
        {
            // DEBUG_V();
            MessageStr = emptyString;
        }
#endif // def OldWay
    // DEBUG_END;
}   // AddControls

// *********************************************************************************************
void c_ControllerMQTT::begin ()
{
    // DEBUG_START;

    fsm_Connection_state_disabled_imp.SetParent (this);
    fsm_Connection_state_WaitForWiFi_imp.SetParent (this);
    fsm_Connection_state_ValidateConfig_imp.SetParent(this);
    fsm_Connection_state_connecting_imp.SetParent (this);
    fsm_Connection_state_connected_imp.SetParent (this);
    fsm_Connection_state_Disconnecting_imp.SetParent (this);

    fsm_Connection_state_disabled_imp.Init ();

    mqttClient.setClient (wifiClient);

    // DEBUG_END;
}   // Begin

// *********************************************************************************************
bool c_ControllerMQTT::ConfigHasChanged()
{
    // DEBUG_START;

    bool Response = false;

    for(auto * CurrentControl : ListOfControls)
    {
        Response |= CurrentControl->GetAndResetValueChangedFlag();
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void c_ControllerMQTT::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response)
{
    if (ControllerIsEnabled ())
    {
        Messages.GetNextRdsMessage (Response);
    }
}

// *************************************************************************************************************************
void c_ControllerMQTT::mqttClientCallback (const char * topic, byte * payload, unsigned int length)
{
    // DEBUG_START;
#ifdef OldWay
        // char cBuff[length + 2];                    // Allocate Character buffer.
        // char logBuff[length + strlen (topic) + 60]; // Allocate a big buffer space.
        // char mqttBuff[140 + sizeof (VERSION_STR) + STA_NAME_MAX_SZ];
        String  payloadStr;
        String  topicStr;

        payloadStr.reserve (MQTT_PAYLD_MAX_SZ + 1);
        topicStr.reserve (MQTT_TOPIC_MAX_SZ + 1);
        // DEBUG_V();

        if (length > MQTT_PAYLD_MAX_SZ)
        {
            // DEBUG_V();
            Log.warningln (F ("MQTT Message Length (%u bytes) too long! Truncated to %u bytes."), length, MQTT_PAYLD_MAX_SZ);
            length = MQTT_PAYLD_MAX_SZ;
        }
        // Log.verboseln ("MQTT Message Length: %u", length);

        // Copy payload to local char array.
        {
            char cBuff[length + 2];
            memcpy (cBuff, payload, length);
            cBuff[length]   = 0;
            payloadStr      = cBuff;
        }
        payloadStr.trim (); // Remove leading/trailing spaces. Do NOT change to lowercase!
        // DEBUG_V();

        topicStr = topic;
        topicStr.trim ();           // Remove leading/trailing spaces.
        topicStr.toLowerCase ();    // Force lowercase, prevent case matching problems.

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

        if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_AUDMODE_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received Audio Mode Command"));
            audioModeCmd (payloadStr, MqttControllerId);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_FREQ_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received Radio Frequency Command"));
            frequencyCmd (payloadStr, MqttControllerId);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_GPIO19_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received GPIO19 Command"));
            gpioMqttControl (payloadStr, GPIO19_PIN);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_GPIO23_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received GPIO23 Command"));
            gpioMqttControl (payloadStr, GPIO23_PIN);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_GPIO33_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received GPIO33 Command"));
            gpioMqttControl (payloadStr, GPIO33_PIN);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_INFO_STR))
        {
            // DEBUG_V();
            String Message;
            Log.infoln (F ("MQTT: Received System Information Command."));

            if (infoCmd (payloadStr, MqttControllerId))
            {
                // DEBUG_V();
                String HostName;
                WiFiDriver.GetHostname (HostName);
                Message = String (F ("{\"")) + CMD_INFO_STR +
                                F ( "\": \"ok\", \"version\": \"") + VERSION_STR +
                                F ( "\", \"hostName\": \"") + HostName +
                                F ( "\", \"ip\": \"") + WiFi.localIP ().toString () +
                                F ( "\", \"rssi\": ") + WiFi.RSSI () +
                                F ( ", \"status\": \"0x") + ControllerMgr.getControllerStatusSummary ();
            }
            else
            {
                // DEBUG_V();
                Message = String (F ("{\"")) + CMD_INFO_STR + F ("\": \"fail\"}");
            }
            // DEBUG_V();
            topicStr = MqttName.get () + MQTT_INFORM_STR;
            mqttClient.publish (topicStr.c_str (), Message.c_str ());
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_MUTE_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received Audio Mute Command"));
            muteCmd (payloadStr, Name);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_PICODE_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT Received RDS PI Code Command"));
            piCodeCmd (payloadStr, MqttControllerId);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_PSN_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received RDS Program Station Name Command"));
            programServiceNameCmd (payloadStr, MqttControllerId);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_RADIOTEXT_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received RadioText Command"));
            radioTextCmd (payloadStr, MqttControllerId);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_REBOOT_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received System Reboot Command."));
            rebootCmd (payloadStr, MqttControllerId);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_START_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received RDS Restart Command."));
            startCmd (payloadStr, MqttControllerId);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_STOP_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received RDS Stop Command."));
            stopCmd (payloadStr, MqttControllerId);
        }
        else if (topicStr == MqttName.get () + makeMqttCmdStr (CMD_PERIOD_STR))
        {
            // DEBUG_V();
            Log.infoln (F ("MQTT: Received RDS Display Time Command"));
            rdsTimePeriodCmd (payloadStr, MqttControllerId);
        }
        else
        {
            // DEBUG_V();
            Log.errorln ((String (F ("MQTT: Received Unknown Command (")) + topicStr + F ("), Ignored.")).c_str ());
        }
#endif // def OldWay
    // DEBUG_END;
}   // mqttClientCallback

// *************************************************************************************************************************
void c_ControllerMQTT::mqttReconnect (bool resetFlg)
{
    // DEBUG_START;
    static uint8_t ClientConnectionRetryCount = 0;  // MQTT Connection Retry Counter. Each reconnect attempt is allowed
                                                    // several tries.
    static long previousWiFiMillis = millis ();     // Timer for WiFi services.
    String topicStr;

    do                                              // once
    {
        // DEBUG_V();
        if (resetFlg == true)
        {
            // DEBUG_V();
            ClientConnectionRetryCount  = 0;
            previousWiFiMillis          = millis () - MQTT_RECONNECT_TIME + 2000;   // Allow Reconnect in 2 secs.
            Log.traceln (F ("MQTT Reconnect Fail Count has Been Reset."));
            break;
        }
        // DEBUG_V();

        if (!ControllerIsEnabled ())
        {   // MQTT Controller Disabled.
            // DEBUG_V();
            OnlineFlag = false;

            if (mqttClient.connected ())
            {
                // DEBUG_V();
                mqttClient.disconnect ();
                Log.traceln (F ("MQTT Controller Disabled: Closed Connection."));
            }
            break;
        }
        // DEBUG_V();

        if (millis () - previousWiFiMillis < MQTT_RECONNECT_TIME)
        {
            // DEBUG_V();
            // keep waiting
            break;
        }
        previousWiFiMillis = millis ();
        // DEBUG_V();

        if (WiFi.status () != WL_CONNECTED)
        {
            // DEBUG_V();
            setMessage (MQTT_NOT_AVAIL_STR);
            Log.errorln (F ("-> WiFi Router Not Connected; MQTT Disabled."));
            break;
        }

        if (mqttClient.connected ())
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
        setMessage (MQTT_RETRY_STR);
        Log.infoln ((String (F ("Attempting MQTT Reconnection #")) + String (ClientConnectionRetryCount)).c_str ());
        mqttClient.setServer (MqttBrokerIpAddress.GetIpAddress (), MqttPort.get32 ());
        mqttClient.setCallback ([] (const char * topic, byte * payload, unsigned int length)
                                {
                                    c_ControllerMQTT * pMe =
                                        static_cast <c_ControllerMQTT *> (ControllerMgr.GetControllerById (MqttControllerId));
                                    pMe->mqttClientCallback (topic, payload, length);
                                }); // Topic Subscription callback handler.

        // DEBUG_V();
        mqttClient.setKeepAlive (MQTT_KEEP_ALIVE);

        // DEBUG_V();
        Log.verboseln ( (String (F ("-> Broker Name: \"")) + MqttName.get () + "\"").c_str ());
        Log.verboseln ( (String (F ("-> Broker User: \"")) + MqttUser.get () + F ("\", Password: \"") + MqttPassword.get () + "\"").c_str ());
        Log.verboseln ( (String (F ("-> Broker IP: ")) + MqttBrokerIpAddress.get () + F (", PORT: ") + MqttPort.get ()).c_str ());
        // DEBUG_V();

        if (mqttClient.connect (MqttName.get ().c_str (), MqttUser.get ().c_str (), MqttPassword.get ().c_str ()))
        {   // Connect to MQTT Server
            // DEBUG_V();
            OnlineFlag  = true;
            topicStr    = MqttName.get () + MQTT_CONNECT_STR;
            String Payload;
            Payload = String (F ("{\"reconnect\": ")) + String (ClientConnectionRetryCount) + F ("}");  // JSON Formatted payload.
            // DEBUG_V();
            mqttClient.publish (topicStr.c_str (), Payload.c_str ());                                   // Publish reconnect status.
            // DEBUG_V();

            Log.infoln ((String (F ("-> MQTT Reconnected. Sent Topic:")) + topicStr + F (", Payload:") + Payload).c_str ());
            // DEBUG_V();

            topicStr = MqttName.get () + MQTT_CMD_SUB_STR;
            // DEBUG_V();

            if (mqttClient.subscribe (topicStr.c_str ()))
            {
                // DEBUG_V();
                Log.infoln ((String (F ("-> MQTT Successfully Subscribed to ")) + topicStr).c_str ());
            }
            else
            {
                // DEBUG_V();
                Log.errorln (F ("-> MQTT subscribe failed!"));
                setMessage (MQTT_SUBCR_FAIL_STR);
            }
            ClientConnectionRetryCount = 0; // Successful reconnect, OK to reset counter.
            break;
        }
        // DEBUG_V();

        if (ClientConnectionRetryCount < MQTT_RETRY_CNT)
        {
            // DEBUG_V();
            Log.warningln ((String (F ("-> MQTT Connection Failure #")) + String (ClientConnectionRetryCount) + F (", Code= ") +
                            returnClientCode (mqttClient.state ())).c_str ());
            setMessage (MQTT_RETRY_FAIL_STR + String (ClientConnectionRetryCount));
            break;
        }
        // DEBUG_V();

        Log.errorln (F ("-> MQTT Controller Has Been Disabled, Too Many Reconnect Attempts."));
        setMessage (MQTT_DISABLED_STR);
    } while (false);
    // DEBUG_END;
}   // mqttReconnect

// ************************************************************************************************
void c_ControllerMQTT::TestParameters ()
{
    // DEBUG_START;

    if ((INADDR_NONE == MqttBrokerIpAddress.GetIpAddress ()) ||
        (MqttName.get ().isEmpty ()) ||
        (MqttUser.get ().isEmpty ()) ||
        (MqttPassword.get ().isEmpty ()))
    {   // Missing MQTT Entries
        // DEBUG_V();
        setMessage (MQTT_MISSING_STR);
        // ESPUI.print (ControlerEnabledElementId, "0");   // Turn off MQTT Controller Switcher.
        Log.warningln (F ("Invalid MQTT Controller Settings, Disabled MQTT."));
    }
    else
    {
        // DEBUG_V();
        // ControllerIsEnabled() = false;
        setMessage (emptyString);
        mqttReconnect (true);   // Reset MQTT Reconnect values while ControllerIsEnabled() is false.
        // ControllerIsEnabled() = true;       // Must set flag AFTER mqqtReconnect!
    }
    // DEBUG_END;
}   // TestParameters

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

// *************************************************************************************************************************
// gpioMqttControl (): MQTT handler for GPIO Commands. This is a companion to mqttCallback ().
void c_ControllerMQTT::gpioMqttControl (String payloadStr, gpio_num_t pin)
{
    // DEBUG_START;
    bool successFlg = true;
    String  topicStr;
    String  CmdString;

    topicStr = GetTitle () + MQTT_GPIO_STR;

    Log.infoln ((String (F ("-> MQTT Controller: Received GPIO Pin-")) + String (pin) + " Command").c_str ());

#ifdef OldWay
        successFlg = gpioCmd (payloadStr, MqttControllerId, pin);
#endif // def OldWay
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
String c_ControllerMQTT::makeMqttCmdStr (String cmdStr) {return String (MQTT_CMD_STR) + cmdStr;}    // makeMqttCmdStr

// *************************************************************************************************************************
// mqttSendMessages(): Broadcast system messages to MQTT broker.
// The messages are sent periodically (MQTT_MSG_TIME) or immediately whenever they change.
void c_ControllerMQTT::mqttSendMessages (void)
{
    // DEBUG_START;
    do  // once
    {
        String  Payload;
        String  topicStr;

        if (!ControllerIsEnabled ())
        {   // MQTT Controller Disabled.
            // DEBUG_V();
            break;
        }

        if (!mqttClient.connected ())
        {   // MQTT not connected, nothing to do. Exit.
            // DEBUG_V();
            break;
        }

        if (millis () - previousMqttMillis >= MQTT_MSG_TIME)
        {
            // DEBUG_V();
            previousMqttMillis  = millis ();
            refresh             = true;
        }
        // DEBUG_V();

        extern float    paVolts;
        extern float    vbatVolts;

        if (refresh ||
            ((vbatVolts > oldVbatVolts + VOLTS_HYSTERESIS) || (vbatVolts < oldVbatVolts - VOLTS_HYSTERESIS)) ||
            ((paVolts > oldPaVolts + VOLTS_HYSTERESIS) || (paVolts < oldPaVolts - VOLTS_HYSTERESIS)))
        {
            // DEBUG_V();
            oldVbatVolts    = vbatVolts;
            oldPaVolts      = paVolts;
            topicStr        = MqttName.get () + MQTT_VOLTS_STR;
            // JSON Formatted Payload.
            Payload = String (F ("{\"vbat\": ")) + String (vbatVolts) + F (",\"pa\": ") + String (paVolts);
            mqttClient.publish (topicStr.c_str (), Payload.c_str ());   // Publish Power Supply Voltage.

            Log.infoln ((String (F ("MQTT Publish, [Topic]: ")) + topicStr).c_str ());
            Log.infoln ((String (F ("-> Payload: ")) + Payload).c_str ());

            refresh = false;
            // DEBUG_V();
        }
    } while (false);

    // DEBUG_END;
}   // mqttSendMessages

// *********************************************************************************************
void c_ControllerMQTT::poll (void)
{
    // _ DEBUG_START;

    uint32_t Now = millis();

    if (Now > FsmTimerExpirationTime)
    {
        FsmTimerExpirationTime += 1000;
        if(pCurrentFsmState)
        {
            pCurrentFsmState->Poll (Now);
        }
        else
        {
         // DEBUG_V("pCurrentFsmState not set");
        }
    }
#ifdef OldWay

        if (mqttClient.connected ())
        {
            mqttClient.loop (); // Service MQTT background tasks.
            mqttSendMessages ();
        }
        else
        {
            mqttReconnect (false);
        }
#endif // def OldWay
    // _ DEBUG_END;
}   // poll

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
String c_ControllerMQTT::returnClientCode (int code)
{
    String Response;

    switch (code)
    {
        case MQTT_CONNECTION_TIMEOUT:
        {
            Response = F ("MQTT_CONNECTION_TIMEOUT");
            break;
        }

        case MQTT_CONNECTION_LOST:
        {
            Response = F ("MQTT_CONNECTION_LOST");
            break;
        }

        case MQTT_CONNECT_FAILED:
        {
            Response = F ("MQTT_CONNECT_FAILED");
            break;
        }

        case MQTT_DISCONNECTED:
        {
            Response = F ("MQTT_DISCONNECTED");
            break;
        }

        case MQTT_CONNECTED:
        {
            Response = F ("MQTT_CONNECTED");
            break;
        }

        case MQTT_CONNECT_BAD_PROTOCOL:
        {
            Response = F ("MQTT_CONNECT_BAD_PROTOCOL");
            break;
        }

        case MQTT_CONNECT_BAD_CLIENT_ID:
        {
            Response = F ("MQTT_CONNECT_BAD_CLIENT_ID");
            break;
        }

        case MQTT_CONNECT_UNAVAILABLE:
        {
            Response = F ("MQTT_CONNECT_UNAVAILABLE");
            break;
        }

        case MQTT_CONNECT_BAD_CREDENTIALS:
        {
            Response = F ("MQTT_CONNECT_BAD_CREDENTIALS");
            break;
        }

        case MQTT_CONNECT_UNAUTHORIZED:
        {
            Response = F ("MQTT_CONNECT_UNAUTHORIZED");
            break;
        }

        default:
        {
            Response = F ("Undefined");
            break;
        }
    }   // switch

    return Response;
}

// *********************************************************************************************
bool c_ControllerMQTT::ValidateConfiguration()
{
    // DEBUG_START;

    bool response = false;

    do // once
    {
        String Temp = MqttName.get();
        if(Temp.isEmpty())
        {
            setMessage(F("Broker Subscriber Name is not set"), cControlCommon::eCssStyle::CssStyleRed_bw);
            break;
        }

        if(MqttBrokerIpAddress.GetIpAddress() == IPAddress(uint32_t(0)))
        {
            setMessage(F("Broker IP Address is not set"), cControlCommon::eCssStyle::CssStyleRed_bw);
            break;
        }

        // MqttPort: port is always a valid number

        Temp = MqttUser.get();
        if(Temp.isEmpty() || Temp.equals(MqttUser.getDefault()))
        {
            setMessage(F("Broker Username Name is not set"), cControlCommon::eCssStyle::CssStyleRed_bw);
            break;
        }

        Temp = MqttPassword.get();
        if(Temp.isEmpty() || Temp.equals(MqttPassword.getDefault()))
        {
            setMessage(F("Broker Password is not set"), cControlCommon::eCssStyle::CssStyleRed_bw);
            break;
        }

     // DEBUG_V("Made it to the end");
        response = true;

    } while(false);

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
    pParent->setMessage(F("Disabled"), cControlCommon::eCssStyle::CssStyleWhite);

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
    pParent->ValidateConfiguration();

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
    else if(pParent->ValidateConfiguration())
    {
        fsm_Connection_state_WaitForWiFi_imp.Init();
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
    pParent->setMessage(F("Waiting For WiFi to come up"), cControlCommon::eCssStyle::CssStyleWhite);

 // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_WaitForWiFi::Poll (uint32_t Now)
{
    // DEBUG_START;

    if ((!pParent->getBool ()) ||
        (pParent->ConfigHasChanged()) ||
        (!pParent->ValidateConfiguration()))
    {
        fsm_Connection_state_Disconnecting_imp.Init ();
    }
    else if (WiFi.status () == WL_CONNECTED)
    {
        fsm_Connection_state_connecting_imp.Init();
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
    pParent->setMessage(F("Connecting to Broker"), cControlCommon::eCssStyle::CssStyleWhite);

    Log.traceln (F ("Initializing MQTT"));
    pParent->mqttClient.setServer (MqttBrokerIpAddress.GetIpAddress (), MqttPort.get32 ());
/*
        mqttClient.setCallback ([] (const char * topic, byte * payload, unsigned int length)
                                {
                                    c_ControllerMQTT * pMe =
                                        static_cast <c_ControllerMQTT *> (ControllerMgr.GetControllerById (MqttControllerId));
                                    pMe->mqttClientCallback (topic, payload, length);
                                }); // Topic Subscription callback handler.
*/
    pParent->mqttClient.setKeepAlive (MQTT_KEEP_ALIVE);
    // DEBUG_V();

    pParent->mqttClient.connect (MqttName.get ().c_str (), MqttUser.get ().c_str (), MqttPassword.get ().c_str ());

 // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_connecting::Poll (uint32_t Now)
{
 // DEBUG_START;

    if ((!pParent->getBool ()) ||
        (WiFi.status () != WL_CONNECTED) ||
        (pParent->ConfigHasChanged()) ||
        (!pParent->ValidateConfiguration()))
    {
        fsm_Connection_state_Disconnecting_imp.Init ();
    }
    else if(pParent->mqttClient.connected())
    {
        // DEBUG_V("mqtt has connected, move to the connected state.");
        fsm_Connection_state_connected_imp.Init();
    }

 // DEBUG_END;
}

// *********************************************************************************************
// *********************************************************************************************
void fsm_Connection_state_connected::Init ()
{
 // DEBUG_START;

    pParent->pCurrentFsmState = this;
    pParent->setMessage(F("Connected to Broker"), cControlCommon::eCssStyle::CssStyleWhite);

    // DEBUG_V();
    String topicStr;
    topicStr    = MqttName.get ();
    topicStr    += MQTT_CONNECT_STR;
    String payloadStr = F ("{\"boot\": 0}");                        // JSON Formatted payload.
    pParent->mqttClient.publish (topicStr.c_str (), payloadStr.c_str ());    // Publish reconnect status.

    Log.infoln ((String (F ("-> MQTT Started. Sent Topic: ")) + topicStr + F (", Payload: ") + payloadStr).c_str ());

    topicStr    = MqttName.get ();
    topicStr    += MQTT_CMD_SUB_STR;

    if (pParent->mqttClient.subscribe (topicStr.c_str ()))
    {
        // DEBUG_V();
        Log.infoln ((String (F ("-> MQTT Successfully Subscribed to \"")) + topicStr + F ("\"")).c_str ());
    }
    else
    {
        // DEBUG_V();
        Log.errorln (F ("-> MQTT subscribe failed!"));
        fsm_Connection_state_Disconnecting_imp.Init();
    }

 // DEBUG_END;
}

// *********************************************************************************************
void fsm_Connection_state_connected::Poll (uint32_t Now)
{
 // DEBUG_START;

    if ((!pParent->getBool ()) ||
        (pParent->ConfigHasChanged()) ||
        (WiFi.status () != WL_CONNECTED) ||
        (!pParent->ValidateConfiguration()) ||
        (!pParent->mqttClient.connected()))
    {
        fsm_Connection_state_Disconnecting_imp.Init ();
    }
    else
    {
        // DEBUG_V("Do MQTT related stuff");
    }

 // DEBUG_END;
}

// *********************************************************************************************
// *********************************************************************************************
void fsm_Connection_state_Disconnecting::Init ()
{
 // DEBUG_START;

    pParent->pCurrentFsmState = this;
    pParent->setMessage(F("Disconnecting From Broker"), cControlCommon::eCssStyle::CssStyleWhite);

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
