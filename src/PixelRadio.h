#pragma once
/*
  *    File: PixelRadio.h
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
  *    absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *     Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
  */

// *********************************************************************************************
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "ControllerMgr.h"
#include "ESPUI.h"
#include "WiFiDriver.hpp"

// *********************************************************************************************
// VERSION STRING: Must be updated with each public release.
// The version is shown during boot on Serial Log and on the "About" UI web Tab page.
#define VERSION_STR     "2.1.0"
#define AUTHOR_STR      "by Thomas Black / Martin Mueller"
#define BLD_DATE_STR    "Jun-13-2022"
#define GITHUB_REPO_STR "<a href=\"https://github.com/MartinMueller2003/PixelRadio/\" target=\"_blank\">Click Here for Information</a>"
extern bool SystemBooting;

// *********************************************************************************************

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_)   ((_time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_)     (_time_ / SECS_PER_DAY)

// Controller Command Limits
const uint8_t   CMD_GPIO_MAX_SZ = 7;    // GPIO Cmd Code max length is 7 ("input/inputpd/inputpu/outhigh/outlow/read").
const uint8_t   CMD_LOG_MAX_SZ  = 7;    // Serial Log Level Arg max length is 7 ("silent" / "restore");
const uint8_t   CMD_SYS_MAX_SZ  = 6;    // System Cmd Code length is 6 ("system").
const uint8_t   CMD_TIME_MAX_SZ = 4;    // Time Command Arg length is 3 (5-900). Add +1 to trap typos.

#define CMD_RDS_CODE_STR    "rds"
#define CMD_SYS_CODE_STR    "system"

// EEPROM: (Currently Not Used in PixelRadio)
const uint16_t  EEPROM_SZ   = 32;   // E2Prom Size, must be large enough to hold all values below.
const uint8_t   INIT_BYTE   = 0xA5; // E2Prom Initialization Stamping value.
const int   INIT_ADDR       = 0;    // E2Prom Address for Init value, 8-bits.
const int   XXXX_SET_ADDR   = 10;   // E2Prom Address for (unused), 16-bit.

#define COLOR_GRN_STR   "#66ff33"
#define COLOR_RED_STR   "#FF3500"
#define COLOR_BLK_STR   "#000000"
#define COLOR_WHT_STR   "#FFFFFF"
#define COLOR_GRY_STR   "#bfbfbf"

// File System
#define  BACKUP_FILE_NAME   "/backup.cfg"
#define  CRED_FILE_NAME     "/credentials.txt"
#define  LOGO_GIF_NAME      "/RadioLogo225x75_base64.gif"   // Base64 gif file, 225 pixel W x 75 pixel H.
const uint8_t   LITTLEFS_MODE   = 1;
const uint8_t   SD_CARD_MODE    = 2;

// FM Radio RF
const float PA_VOLT_MIN = 8.1f;     // Minimum allowed voltage for Power Amp, 9V -10%.
const float PA_VOLT_MAX = 9.9f;     // Maximum allowed voltage for Power Amp, 9V + 10%.
// Free Memory

// GPIO Pins:
// Note: GPIOs 34-39 do not support internal pullups or pulldowns.
const gpio_num_t    MISO_PIN    = gpio_num_t::GPIO_NUM_2;   // SD Card D0, requires INPUT_PULLUP During SD Card Use.
const gpio_num_t    MOSI_PIN    = gpio_num_t::GPIO_NUM_15;  // SD Card CMD.
const gpio_num_t    ON_AIR_PIN  = gpio_num_t::GPIO_NUM_12;
const gpio_num_t    SCL_PIN     = gpio_num_t::GPIO_NUM_22;  // I2C Clock Pin, Output.
const gpio_num_t    SDA_PIN     = gpio_num_t::GPIO_NUM_18;  // I2C Data Pin, I/O.
const gpio_num_t    SD_CLK_PIN  = gpio_num_t::GPIO_NUM_14;  // SD Card CLK.
const gpio_num_t    SD_CS_PIN   = gpio_num_t::GPIO_NUM_13;  // SD Card CS.
const gpio_num_t    SER1_RXD    = gpio_num_t::GPIO_NUM_34;  // Serial1 RxD Input, Optional 3.3V TTL, 5V tolerant.
const gpio_num_t    SER1_TXD    = gpio_num_t::GPIO_NUM_32;  // Serial1 TxD Output, 3.3V TTL.
const gpio_num_t    MUX_PIN     = gpio_num_t::GPIO_NUM_21;  // Audio Signal MUX Control, Output.
const gpio_num_t    TONE_PIN    = gpio_num_t::GPIO_NUM_25;  // PWM Test Tone Pin, Output.

// GPIO Pin States
const int   SIGN_OFF    = 0;
const int   SIGN_ON     = 1;

// Measurement:
const int32_t MEAS_TIME = 50;   // Measurement Refresh Time, in mS.

// Radio
const uint8_t RADIO_CAL_RETRY = 3;  // RF Port Calibration Retry Count (Maximum Retry Count).


// Time Conversion
const uint32_t  MSECS_PER_SEC   = 1000UL;
const uint32_t  SECS_PER_HOUR   = 3600UL;
const uint32_t  SECS_PER_DAY    = SECS_PER_HOUR * 24UL;
const uint32_t  SECS_PER_MIN    = 60UL;

// OTA:
const uint16_t  OTA_PORT    = 3232; // Port for OTA.
const uint16_t  OTA_TIMEOUT = 3000; // Max allowed time to receive OTA data.

// Volts:
const float MIN_VOLTS           = 4.5f;     // Minimum Power Supply volts.
const float VOLTS_HYSTERESIS    = 0.15f;    // Voltage Hysterisis.
const uint16_t VOLTS_UPD_TIME   = 3750;     // Power Supply Volts GUI Update time (on diagTab), in mS.
// const uint32_t CLIENT_TIMEOUT = 500;   // Webserver Client Timeout, in mS.

const uint16_t WEBSERVER_PORT = 80;   // Port for Web Server. Do not change.

// *********************************************************************************************

// ESPUI (WebGUI) Prototypes
void    buildGUI (void);
void    displaySaveWarning (void);
void    initCustomCss (void);
void    startGUI (void);

// File System (LITTLEFS) prototypes
void            instalLogoImageFile (void);
const String    makeWebGif (
    String      fileName,
    uint16_t    width,
    uint16_t    height,
    String      backGroundColorStr);
void littlefsInit (void);


// JSON Prototypes
bool    checkEmergencyCredentials (const char * fileName);
bool    restoreConfiguration (
    uint8_t     restoreMode,
    const char  * fileName);
bool saveConfiguration (
    uint8_t     saveMode,
    const char  * filename);

// Misc Prototypes
void    initEprom (void);
void    spiSdCardShutDown (void);

const String returnClientCode (int code);

// OTA Prototypes
void otaInit (String & mdnsname);

template <typename J, typename N>
bool ReadFromJSON (float & OutValue, J & Json, N Name)
{
    bool HasBeenModified = false;

    if (true == Json.containsKey (Name))
    {
        float temp = Json[Name];

        if (fabs (temp - OutValue) > 0.000005F)
        {
            OutValue        = temp;
            HasBeenModified = true;
        }
    }

    return HasBeenModified;
}

template <typename T, typename J, typename N>
bool ReadFromJSON (T & OutValue, J & Json, N Name)
{
    bool HasBeenModified = false;

    if (true == Json.containsKey (Name))
    {
        T temp = Json[Name];

        if (temp != OutValue)
        {
            OutValue        = temp;
            HasBeenModified = true;
        }
    }

    return HasBeenModified;
}

// *********************************************************************************************
// EOF
