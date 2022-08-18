#pragma once
/*
   File: language.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022.
   Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: Language text uses define's instead of const String. This saves a LOT of Ram and Flash.

 */

// ************************************************************************************************
#include "config.h"
#include <Arduino.h>

// ************************************************************************************************

#define ADJUST_ANA_VOL_STR    "AUDIO VOLUME"
#define ADJUST_FRQ_CTRL_STR   "FM TRANSMIT FREQUENCY"
#define ADJUST_AUDIO_SEP_STR  "AUDIO CONTROLS"
#define ADJUST_TAB_STR        "Adjust"
#define ADJUST_USB_VOL_STR    "USB VOLUME"



#define APP_NAME_STR  N_PixelRadio

#define BACKUP_TAB_STR       "Backup"
#define BACKUP_SAVE_STR      "SAVE"
#define BACKUP_SAV_CFG_STR   "SAVE TO SD CARD"
#define BACKUP_SAV_FAIL_STR  "SAVE FAILED: CHECK SD CARD"
#define BACKUP_SAV_PASS_STR  "Configuration Saved"
#define BACKUP_RESTORE_STR   "RESTORE"
#define BACKUP_RES_CFG_STR   "RESTORE FROM SD CARD"
#define BACKUP_RES_FAIL_STR  "RESTORE FAILED: CHECK SD CARD"
#define BACKUP_RES_PASS_STR  "Restore Complete: Reboot Required"

#define CTRL_LOCAL_STR       "LOCAL CONTROL"
#define CTRL_TAB_STR         "Controllers"
#define CTRL_HTTP_STR        "HTTP CONTROL"
#define CTRL_HTPP_SET_STR    "HTTP CONTROL SETTINGS"
// #define CTRL_LOCAL_SEP_STR   "LOCAL CONTROL SETTINGS"
#define CTRL_MQTT_IP_STR     "BROKER IP ADDRESS"
#define CTRL_MQTT_PW_STR     "BROKER PASSWORD"
#define CTRL_MQTT_PORT_STR   "BROKER PORT NUMBER"
#define CTRL_MQTT_STR        "MQTT CONTROL"
#define CTRL_MQTT_SEP_STR    "MQTT CONTROL SETTINGS"
#define CTRL_MQTT_USER_STR   "BROKER USERNAME"
#define CTLR_SERIAL_MSG_STR  "WARNING: DIAGNOSTIC SERIAL LOG IS ON"
#define CTRL_USB_SERIAL_STR  "USB SERIAL CONTROL SETTINGS"

#define DIAG_BOOT_MSG1_STR   "WARNING: SYSTEM WILL REBOOT<br>** RELEASE NOW TO ABORT **"
#define DIAG_BOOT_MSG2_STR   "** SYSTEM REBOOTING **<br>WAIT 30 SECONDS BEFORE ACCESSING WEB PAGE."
#define DIAG_DEBUG_SEP_STR   "CODE DEBUGGING"
#define DIAG_FREE_MEM_STR    "FREE MEMORY"
#define DIAG_HEALTH_SEP_STR  "HEALTH"
#define DIAG_LOG_LVL_STR     "SERIAL LOG LEVEL"
#define DIAG_LOG_MSG_STR     "WARNING: SERIAL CONTROLLER IS ON"
#define DIAG_LONG_PRESS_STR  "Long Press (5secs)"
#define DIAG_REBOOT_STR      "REBOOT SYSTEM"
#define DIAG_SYSTEM_SEP_STR  "SYSTEM"
#define DIAG_RUN_TIME_STR    "SYSTEM RUN TIME"
#define DIAG_TAB_STR         "Diagnostic"
#define DIAG_VBAT_STR        "SYSTEM VOLTAGE"
#define DIAG_VDC_STR         "RF PA VOLTAGE"

#define DIAG_LOG_ERROR_STR   "LOG_LEVEL_ERROR"
#define DIAG_LOG_FATAL_STR   "LOG_LEVEL_FATAL"
#define DIAG_LOG_INFO_STR    "LOG_LEVEL_INFO"
#define DIAG_LOG_SILENT_STR  "LOG_LEVEL_SILENT (default)"
#define DIAG_LOG_WARN_STR    "LOG_LEVEL_WARNING"
#define DIAG_LOG_TRACE_STR   "LOG_LEVEL_TRACE"
#define DIAG_LOG_VERB_STR    "LOG_LEVEL_VERBOSE"
#define DIAG_LOG_DEF_STR      DIAG_LOG_VERB_STR;


// #define DIG_AUD_GAIN0_STR     "DIAG0"
// #define DIG_AUD_GAIN1_STR     "DIAG1"
// #define DIG_AUD_GAIN2_STR     "DIAG2"
// #define DIG_AUD_GAIN_DEF_STR  DIG_AUD_GAIN0_STR;


#define ERROR_MSG_STR  "ERROR"

#define GPIO_19_STR        "GPIO PIN 19"
#define GPIO_23_STR        "GPIO PIN 23"
#define GPIO_33_STR        "GPIO PIN 33"
#define GPIO_OUT_LO_STR    "DIGITAL OUTPUT = LOW"
#define GPIO_OUT_HI_STR    "DIGITAL OUTPUT = HIGH"
#define GPIO_INP_FT_STR    "DIGITAL INPUT = FLOAT"
#define GPIO_INP_PU_STR    "DIGITAL INPUT = PULLUP"
#define GPIO_INP_PD_STR    "DIGITAL INPUT = PULLDOWN"
#define GPIO_DEF_STR       GPIO_INP_PD_STR
#define GPIO_RD_HI_STR     "PIN READ: 1"
#define GPIO_RD_LO_STR     "PIN READ: 0"
#define GPIO_SETTINGS_STR  "GPIO BOOT STATES"
#define GPIO_TAB_STR       "GPIO"
#define GPIO_SERIAL_STR    "Serial"
#define GPIO_MQTT_STR      "MQTT"
#define GPIO_HTTP_STR      "HTTP"
#define GPIO_LOCAL_STR     "Local"

#define HOME_FM_SEP_STR     "FM RADIO STATUS"
#define HOME_SEP_WIFI_STR   "WIFI"
#define HOME_SEP_RDS_STR    "RADIO DATA SYSTEM (RDS)"
#define HOME_STA_STR        "STA / AP STATUS"
#define HOME_TAB_STR        "Home"

#define MQTT_SUBSCR_NM_STR   "BROKER SUBSCRIBE NAME"
#define MQTT_DISABLED_STR    "CONNECTION FAILED!<br>DISABLED MQTT CONTROLLER"
#define MQTT_DISCONNECT_STR  "DISCONNECTED"
#define MQTT_MISSING_STR     "MISSING SETTINGS : MQTT DISABLED"
#define MQTT_NOT_AVAIL_STR   "MQTT NOT AVAILABLE IN AP MODE"
#define MQTT_ONLINE_STR      "ONLINE"
#define MQTT_PASS_HIDE_STR   "{PASSWORD HIDDEN}"
#define MQTT_RETRY_STR       "RETRY CONNECTION"
#define MQTT_RETRY_FAIL_STR  "RETRY FAIL #"
#define MQTT_SUBCR_FAIL_STR  "MQTT SUBCRIBE FAILED"

#define RADIO_AMEAS_SEP_STR   "AUDIO MEASUREMENT"
#define RADIO_AUTO_OFF_STR    "RF AUTO OFF"
#define RADIO_SEP_AUDIO_STR   "AUDIO SETTINGS"
#define RADIO_SEP_MOD_STR     "MODULATION SETTINGS"
#define RADIO_SEP_RF_SET_STR  "RF SETTINGS"
#define RADIO_TAB_STR         "Radio"

#define RDS_BLANK_STR         "{ RADIOTEXT MESSAGE IS BLANK }"
#define RDS_CTRLS_DIS_STR     "{ ALL RDS CONTROLLERS DISABLED }"
#define RDS_DISPLAY_TIME_STR  "RDS DISPLAY TIME (SECS)"
#define RDS_GENERAL_SET_STR   "GENERAL RDS SETTINGS"
#define RDS_LOCAL_DIS_STR     "{ LOCAL RADIOTEXT MESSAGES DISABLED }"
#define RDS_LOCAL_BLANK_STR   "{ LOCAL RADIOTEXT MESSAGE IS BLANK }"
#define RDS_LOCKED_STR        "{ LOCKED }"
#define RDS_NOT_AVAIL_STR     "{ RADIOTEXT UNAVAILABLE }"
#define RDS_REMOTE_BLANK_STR  "{ REMOTE RADIOTEXT MESSAGE IS BLANK }"
#define RDS_RESTORE_STR       "Restore Defaults"
#define RDS_TAB_STR           "Local RDS"
#define RDS_TEXT1_DEF_STR     "Welcome to Our Drive-by Holiday Light Show"
#define RDS_TEXT2_DEF_STR     "For Safety Keep Automobile Running Lights On"
#define RDS_TEXT3_DEF_STR     "Please Drive Slowly and Watch Out for Children and Pets"
#define RDS_WAITING_STR       "{ WAITING FOR RADIOTEXT }"

#define RF_AUTO_DIS_STR  "RF Always On (default)"
#define RF_AUTO_ENB_STR  "Turn Off RF >60 Secs Silence"
#define RF_AUTO_DEF_STR  RF_AUTO_DIS_STR;


#define SAVE_BACKUP_STR        "BACKUP SETTINGS TO SD CARD"

#define WIFI_ADDR_SEP_STR      "WIFI ADDRESS SETTINGS"
#define WIFI_DISABLE_STR       "WiFi Disabled"
#define WIFI_AP_FALLBK_STR     "AP FALLBACK ENABLE"
#define WIFI_AP_IP_SEP_STR     "AP (HOTSPOT) SETTINGS"
#define WIFI_AP_MODE_STR       "AP Mode (Hot Spot)"
#define WIFI_AP_REBOOT_STR     "AP REBOOT"
#define WIFI_AP_STA_STR        "AP/STA Mode"
#define WIFI_BLANK_MSG_STR     "LEAVE BLANK FOR AUTO LOGIN"
#define WIFI_CRED_SEP_STR      "WIFI ROUTER CREDENTIALS"
#define WIFI_DEV_CRED_SEP_STR  "DEVICE LOGIN CREDENTIALS"
#define WIFI_DHCP_LBL_STR      " (DHCP)"
#define WIFI_NAME_SEP_STR      "NAME SETTINGS"
#define WIFI_RSSI_STR          "RSSI"
#define WIFI_STATUS_SEP_STR    "WIFI STATUS"
#define WIFI_STATIC_LBL_STR    " (STATIC)"
#define WIFI_STA_MODE_STR      "Station Mode"
#define WIFI_TAB_STR           "WiFi"
#define WIFI_UNKNOWN_STR       "(- ? -)"

extern const PROGMEM char       SERIAL_096_STR[];
extern const PROGMEM char       SERIAL_192_STR[];
extern const PROGMEM char       SERIAL_576_STR[];
extern const PROGMEM char       SERIAL_115_STR[];

extern const PROGMEM char       CSS_LABEL_STYLE_BLACK[];
extern const PROGMEM char       CSS_LABEL_STYLE_GREEN[];
extern const PROGMEM char       CSS_LABEL_STYLE_RED[];
extern const PROGMEM char       CSS_LABEL_STYLE_MAROON[];
extern const PROGMEM char       CSS_LABEL_STYLE_TRANSPARENT[];
extern const PROGMEM char       CSS_LABEL_STYLE_WHITE[];

extern const PROGMEM char UNITS_DBM_STR[];

extern const PROGMEM char       N_About[];
extern const PROGMEM char       N_About_PixelRadio[];
extern const PROGMEM char       N_BAUDRATE[];
extern const PROGMEM char       N_Baudrate[];
extern const PROGMEM char       N_br[];
extern const PROGMEM char       N_ControllerEnabled[];
extern const PROGMEM char       N_controllers[];
extern const PROGMEM char       N_command[];
extern const PROGMEM char       N_default[];
extern const PROGMEM char       N_DisplayFseqName[];
extern const PROGMEM char       N_durationSec[];
extern const PROGMEM char       N_Enable[];
extern const PROGMEM char       N_enabled[];
extern const PROGMEM char       N_list[];
extern const PROGMEM char       N_MaxIdleSec[];
extern const PROGMEM char       N_message[];
extern const PROGMEM char       N_messages[];
extern const PROGMEM char       N_Messages[];
extern const PROGMEM char       N_MQTT_IP_STR[];
extern const PROGMEM char       N_MQTT_NAME_STR[];
extern const PROGMEM char       N_MQTT_PW_STR[];
extern const PROGMEM char       N_MQTT_USER_STR[];
extern const PROGMEM char       N_name[];
extern const PROGMEM char       N_path[];
extern const PROGMEM char       N_PayloadTest[];
extern const PROGMEM char       N_PixelRadio[];
extern const PROGMEM char       N_sequences[];
extern const PROGMEM char       N_SequenceLearningEnabled[];
extern const PROGMEM char       N_type[];
extern const PROGMEM char       N_Version[];

// Controller Command Keywords
extern const PROGMEM char       CMD_AUDMODE_STR[];
extern const PROGMEM char       CMD_FREQ_STR[];
extern const PROGMEM char       CMD_GPIO_STR[];
extern const PROGMEM char       CMD_GPIO19_STR[];
extern const PROGMEM char       CMD_GPIO23_STR[];
extern const PROGMEM char       CMD_GPIO33_STR[];
extern const PROGMEM char       CMD_INFO_STR[];
extern const PROGMEM char       CMD_MUTE_STR[];
extern const PROGMEM char       CMD_PERIOD_STR[];
extern const PROGMEM char       CMD_PTYCODE_STR[];
extern const PROGMEM char       CMD_PSN_STR[];
extern const PROGMEM char       CMD_REBOOT_STR[];
extern const PROGMEM char       CMD_RF_CARRIER_STR[];
extern const PROGMEM char       CMD_RADIOTEXT_STR[];
extern const PROGMEM char       CMD_START_STR[];
extern const PROGMEM char       CMD_STOP_STR[];
