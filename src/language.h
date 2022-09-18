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

#define CTRL_TAB_STR         "Controllers"

// #define CTLR_SERIAL_MSG_STR  "WARNING: DIAGNOSTIC SERIAL LOG IS ON"

#define DIAG_TAB_STR         "Diagnostic"

#define ERROR_MSG_STR  "ERROR"

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
#define HOME_TAB_STR        "Home"
#define RADIO_TAB_STR         "Radio"

#define RDS_GENERAL_SET_STR   "GENERAL RDS SETTINGS"
#define RDS_TAB_STR           "Local RDS"

#define SAVE_BACKUP_STR        "BACKUP SETTINGS TO SD CARD"

#define WIFI_ADDR_SEP_STR      "WIFI ADDRESS SETTINGS"
#define WIFI_DISABLE_STR       "WiFi Disabled"
#define WIFI_AP_IP_SEP_STR     "AP (HOTSPOT) SETTINGS"
#define WIFI_AP_MODE_STR       "AP Mode (Hot Spot)"
#define WIFI_AP_STA_STR        "AP/STA Mode"
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

extern const PROGMEM char CSS_LABEL_STYLE_BLACK[];
extern const PROGMEM char   CSS_LABEL_STYLE_GREEN[];
extern const PROGMEM char   CSS_LABEL_STYLE_RED[];
extern const PROGMEM char   CSS_LABEL_STYLE_MAROON[];
extern const PROGMEM char   CSS_LABEL_STYLE_TRANSPARENT[];
extern const PROGMEM char   CSS_LABEL_STYLE_WHITE[];

extern const PROGMEM char UNITS_DBM_STR[];

extern const PROGMEM char   N_About[];
extern const PROGMEM char   N_About_PixelRadio[];
extern const PROGMEM char   N_BAUDRATE[];
extern const PROGMEM char   N_Baudrate[];
extern const PROGMEM char   N_br[];
extern const PROGMEM char   N_ControllerEnabled[];
extern const PROGMEM char   N_controllers[];
extern const PROGMEM char   N_command[];
extern const PROGMEM char   N_default[];
extern const PROGMEM char   N_DisplayFseqName[];
extern const PROGMEM char   N_durationSec[];
extern const PROGMEM char   N_Enable[];
extern const PROGMEM char   N_enabled[];
extern const PROGMEM char   N_list[];
extern const PROGMEM char   N_MaxIdleSec[];
extern const PROGMEM char   N_message[];
extern const PROGMEM char   N_messages[];
extern const PROGMEM char   N_Messages[];
extern const PROGMEM char   N_MQTT_IP_STR[];
extern const PROGMEM char   N_MQTT_USER_STR[];
extern const PROGMEM char   N_name[];
extern const PROGMEM char   N_path[];
extern const PROGMEM char   N_PayloadTest[];
extern const PROGMEM char   N_PixelRadio[];
extern const PROGMEM char   N_sequences[];
extern const PROGMEM char   N_SequenceLearningEnabled[];
extern const PROGMEM char   N_type[];
extern const PROGMEM char   N_Version[];

// Controller Command Keywords
extern const PROGMEM char   CMD_AUDMODE_STR[];
extern const PROGMEM char   CMD_FREQ_STR[];
extern const PROGMEM char   CMD_GPIO_STR[];
extern const PROGMEM char   CMD_INFO_STR[];
extern const PROGMEM char   CMD_MUTE_STR[];
extern const PROGMEM char   CMD_PERIOD_STR[];
extern const PROGMEM char   CMD_PTYCODE_STR[];
extern const PROGMEM char   CMD_PSN_STR[];
extern const PROGMEM char   CMD_REBOOT_STR[];
extern const PROGMEM char   CMD_RF_CARRIER_STR[];
extern const PROGMEM char   CMD_RADIOTEXT_STR[];
extern const PROGMEM char   CMD_START_STR[];
extern const PROGMEM char   CMD_STOP_STR[];
