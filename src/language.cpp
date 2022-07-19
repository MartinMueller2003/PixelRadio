/*
   File: language.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  MAR-12-2022
   Public Release:
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022.
   Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: const String in flash uses zero ram and only a single instance in flash

 */

// ************************************************************************************************

// ************************************************************************************************
#include "Language.h"

const PROGMEM char N_About                    [] = "About";
const PROGMEM char N_About_PixelRadio         [] = "About PixelRadio";
const PROGMEM char N_Baudrate                 [] = "Baudrate";
const PROGMEM char N_br                       [] = "<br>";
const PROGMEM char N_ControllerEnabled        [] = "ControllerEnabled";
const PROGMEM char N_command                  [] = "command";
const PROGMEM char N_controllers              [] = "controllers";
const PROGMEM char N_default                  [] = "default";
const PROGMEM char N_DisplayFseqName          [] = "DisplayFseqName";
const PROGMEM char N_durationSec              [] = "durationSec";
const PROGMEM char N_Enable                   [] = "Enable";
const PROGMEM char N_enabled                  [] = "enabled";
const PROGMEM char N_list                     [] = "list";
const PROGMEM char N_MaxIdleSec               [] = "MaxIdleSec";
const PROGMEM char N_message                  [] = "message";
const PROGMEM char N_messages                 [] = "messages";
const PROGMEM char N_Messages                 [] = "Messages";
const PROGMEM char N_MQTT_IP_STR              [] = "MQTT_IP_STR";
const PROGMEM char N_MQTT_NAME_STR            [] = "MQTT_NAME_STR";
const PROGMEM char N_MQTT_PW_STR              [] = "MQTT_PW_STR";
const PROGMEM char N_MQTT_USER_STR            [] = "MQTT_USER_STR";
const PROGMEM char N_name                     [] = "name";
const PROGMEM char N_path                     [] = "path";
const PROGMEM char N_PayloadTest              [] = "PayloadTest";
const PROGMEM char N_PiCode                   [] = "PiCode";
const PROGMEM char N_PixelRadio               [] = "PixelRadio";
const PROGMEM char N_ProgramServiceName       [] = "ProgramServiceName";
const PROGMEM char N_PtyCode                  [] = "PtyCode";
const PROGMEM char N_sequences                [] = "sequences";
const PROGMEM char N_SequenceLearningEnabled  [] = "SequenceLearningEnabled";
const PROGMEM char N_type                     [] = "type";
const PROGMEM char N_Version                  [] = "Version";

   // Controller Command Keywords
const PROGMEM char  CMD_AUDMODE_STR           [] = "aud";    // Radio Stereo / Mono Audio Mode.
const PROGMEM char  CMD_FREQ_STR              [] = "freq";   // FM Transmit Frequency, value is MHz x10.
const PROGMEM char  CMD_GPIO_STR              [] = "gpio";   // GPIO Control.
const PROGMEM char  CMD_GPIO19_STR            [] = "gpio19"; // GPIO Pin-19 Control.
const PROGMEM char  CMD_GPIO23_STR            [] = "gpio23"; // GPIO Pin-23 Control.
const PROGMEM char  CMD_GPIO33_STR            [] = "gpio33"; // GPIO Pin-33 Control.
const PROGMEM char  CMD_INFO_STR              [] = "info";   // System Info.
const PROGMEM char  CMD_MUTE_STR              [] = "mute";   // Radio Audio Mute, On/Off.
const PROGMEM char  CMD_PERIOD_STR            [] = "rtper";  // RadioText Display Time Period, in seconds.
const PROGMEM char  CMD_PICODE_STR            [] = "pic";    // RDS Program Information Code.
const PROGMEM char  CMD_PTYCODE_STR           [] = "pty";    // RDS PTY Code.
const PROGMEM char  CMD_PSN_STR               [] = "psn";    // RadioText Program Service Name, 8 chars max.
const PROGMEM char  CMD_REBOOT_STR            [] = "reboot"; // Reboot System.
const PROGMEM char  CMD_RF_CARRIER_STR        [] = "rfc";    // RF Carrier, On/Off.
const PROGMEM char  CMD_RADIOTEXT_STR         [] = "rtm";    // RadioText Message, 64 chars max.
const PROGMEM char  CMD_START_STR             [] = "start";  // Start RDS using existing HTTP settings.
const PROGMEM char  CMD_STOP_STR              [] = "stop";   // Stop HTTP Controller's RDS Transmission.
