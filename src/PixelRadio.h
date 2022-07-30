/*
   File: PixelRadio.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
   absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

    Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
 */

// *********************************************************************************************
#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "Controllers/ControllerMgr.h"
#include "Network/WiFiDriver.hpp"
#include "config.h"
#include "ESPUI.h"

// *********************************************************************************************
// VERSION STRING: Must be updated with each public release.
// The version is shown during boot on Serial Log and on the "About" UI web Tab page.
#define VERSION_STR       "1.1.0"
#define AUTHOR_STR        "by Thomas Black"
#define BLD_DATE_STR      "Jun-13-2022"
#define GITHUB_REPO_STR   "<a href=\"https://github.com/thomastech/PixelRadio/\" target=\"_blank\">Click Here for Information</a>"

// *********************************************************************************************

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) ((_time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) (_time_ / SECS_PER_DAY)

// Controller Command Limits
const uint8_t CMD_AUD_MAX_SZ  = 6;  // AUD Command Arg max length is 6 ("mono" / "stereo").
const uint8_t CMD_FREQ_MAX_SZ = 4;  // FREQ Command Arg max length is 4 (879 - 1079).
const uint8_t CMD_GPIO_MAX_SZ = 7;  // GPIO Cmd Code max length is 7 ("input/inputpd/inputpu/outhigh/outlow/read").
const uint8_t CMD_LOG_MAX_SZ  = 7;  // Serial Log Level Arg max length is 7 ("silent" / "restore");
const uint8_t CMD_MUTE_MAX_SZ = 3;  // MUTE Command Arg max length is 3 ("on" / "off").
const uint8_t CMD_PI_MAX_SZ   = 7;  // PI Command Arg max length is 6 (Examples, "ffff" and/or "0xffff"). Add +1 to trap typos.
const uint8_t CMD_PTY_MAX_SZ  = 3;  // PTY Command Arg max length is 3 (Example, "10" or "010").
const uint8_t CMD_PSN_MAX_SZ  = 8;  // PSN Command Arg max length for Program Service Name.
const uint8_t CMD_RDS_MAX_SZ  = 3;  // RDS Control Cmd Code length is 3 ("rds").
const uint8_t CMD_RF_MAX_SZ   = 3;  // RF Carrier Cmd Arg max length (Example "on" or "off").
const uint8_t CMD_RT_MAX_SZ   = 64; // RT Command Arg max Length for RadioText.
const uint8_t CMD_SYS_MAX_SZ  = 6;  // System Cmd Code length is 6 ("system").
const uint8_t CMD_TIME_MAX_SZ = 4;  // Time Command Arg length is 3 (5-900). Add +1 to trap typos.

// Controller Command Parameters
#define CMD_GPIO_OUT_HIGH_STR "outhigh"
#define CMD_GPIO_OUT_LOW_STR  "outlow"
#define CMD_GPIO_READ_STR     "read"

#define CMD_LOG_RST_STR    "restore"
#define CMD_LOG_SIL_STR    "silent"

#define CMD_MODE_MONO_STR  "mono"
#define CMD_MODE_STER_STR  "stereo"

#define CMD_MUTE_OFF_STR  "off"
#define CMD_MUTE_ON_STR   "on"

#define CMD_RDS_CODE_STR  "rds"

#define CMD_RF_OFF_STR    "off"
#define CMD_RF_ON_STR     "on"

#define CMD_SYS_CODE_STR  "system"

// Controller Flags
const bool AP_FALLBACK_DEF_FLG = true;
const bool CTRL_DHCP_DEF_FLG   = true;

// const bool CTRL_SERIAL_DEF_FLG = true;
const bool RADIO_MUTE_DEF_FLG  = false;
const bool RDS_TEXTX_DEF_FLG   = true;
const bool RF_AUTO_OFF_DEF_FLG = false;
const bool RF_CARRIER_DEF_FLG  = true;
const bool STEREO_ENB_DEF_FLG  = true;

// EEPROM: (Currently Not Used in PixelRadio)
const uint16_t EEPROM_SZ = 32;            // E2Prom Size, must be large enough to hold all values below.
const uint8_t  INIT_BYTE = 0xA5;          // E2Prom Initialization Stamping value.
const int INIT_ADDR      = 0;             // E2Prom Address for Init value, 8-bits.
const int XXXX_SET_ADDR  = 10;            // E2Prom Address for (unused), 16-bit.

// ESP32
const uint32_t ESP_BAUD_DEF     = 115200; // Default ESP32 Baud Rate.
const uint32_t ELAPSED_TMR_TIME = 1000;   // RDS Elapsed Time Update period, in mS. Do Not change.

// ESPUI (Web UI):
#define ANA_VOL_MIN_STR "0"
#define ANA_VOL_MAX_STR "100"
#define ANA_VOL_DEF_STR "50"
#define BAD_SENDER_STR  "BAD_SENDER_ID"
#define BAD_VALUE_STR   "BAD_VALUE"
#define COLOR_GRN_STR   "#66ff33"
#define COLOR_RED_STR   "#FF3500"
#define COLOR_BLK_STR   "#000000"
#define COLOR_WHT_STR   "#FFFFFF"
#define COLOR_GRY_STR   "#bfbfbf"
#define CSS_LABEL_STYLE_RED    "background-color: unset; color: red; margin-top: .1rem; margin-bottom: .1rem;"
#define CSS_LABEL_STYLE_BLACK  "background-color: unset; color: black; margin-top: .1rem; margin-bottom: .1rem;"
#define CSS_LABEL_STYLE_WHITE  "background-color: unset; color: white; margin-top: .1rem; margin-bottom: .1rem;"
#define CSS_LABEL_STYLE_MAROON "background-color: unset; color: Maroon; margin-top: .1rem; margin-bottom: .1rem;"

#define USB_VOL_MIN_STR "0"
#define USB_VOL_MAX_STR "30"
#define USB_VOL_DEF_STR USB_VOL_MAX_STR

// File System
#define  BACKUP_FILE_NAME "/backup.cfg"
#define  CRED_FILE_NAME   "/credentials.txt"
#define  LOGO_GIF_NAME    "/RadioLogo225x75_base64.gif" // Base64 gif file, 225 pixel W x 75 pixel H.
const uint8_t LITTLEFS_MODE = 1;
const uint8_t SD_CARD_MODE  = 2;

// FM Radio: QN8027 Test Codes
const uint8_t FM_TEST_OK   = 0;          // QN8027 Is Ok.
const uint8_t FM_TEST_VSWR = 1;          // QN8027 RF Out has Bad VSWR.
const uint8_t FM_TEST_FAIL = 2;          // QN8027 Chip Bad or missing.

// FM Radio: All FM Frequencies are X10.
const uint16_t FM_FREQ_DEF_X10 = 887;    // 88.7MHz FM.
const uint16_t FM_FREQ_MAX_X10 = 1079;   // 107.9MHz FM.
const uint16_t FM_FREQ_MIN_X10 = 881;    // 88.1MHz FM.
const uint16_t FM_FREQ_SKP_KHZ = 1;      // 100Khz.
const uint16_t FM_FREQ_SKP_MHZ = 10;     // 1MHz.

// FM Radio RF
const float PA_VOLT_MIN = 8.1f;          // Minimum allowed voltage for Power Amp, 9V -10%.
const float PA_VOLT_MAX = 9.9f;          // Maximum allowed voltage for Power Amp, 9V + 10%.
// Free Memory
const uint32_t FREE_MEM_UPD_TIME = 1750; // Update time for Free Memory (on diagTab), in mS.

// GPIO Pins:
// Note: GPIOs 34-39 do not support internal pullups or pulldowns.
const gpio_num_t GPIO19_PIN   = gpio_num_t::GPIO_NUM_19;
const gpio_num_t GPIO23_PIN   = gpio_num_t::GPIO_NUM_23;
const gpio_num_t GPIO33_PIN   = gpio_num_t::GPIO_NUM_33;
const gpio_num_t MISO_PIN     = gpio_num_t::GPIO_NUM_2;  // SD Card D0, requires INPUT_PULLUP During SD Card Use.
const gpio_num_t MOSI_PIN     = gpio_num_t::GPIO_NUM_15; // SD Card CMD.
const gpio_num_t ON_AIR_PIN   = gpio_num_t::GPIO_NUM_12;
const gpio_num_t SCL_PIN      = gpio_num_t::GPIO_NUM_22; // I2C Clock Pin, Output.
const gpio_num_t SDA_PIN      = gpio_num_t::GPIO_NUM_18; // I2C Data Pin, I/O.
const gpio_num_t SD_CLK_PIN   = gpio_num_t::GPIO_NUM_14; // SD Card CLK.
const gpio_num_t SD_CS_PIN    = gpio_num_t::GPIO_NUM_13; // SD Card CS.
const gpio_num_t SER1_RXD     = gpio_num_t::GPIO_NUM_34; // Serial1 RxD Input, Optional 3.3V TTL, 5V tolerant.
const gpio_num_t SER1_TXD     = gpio_num_t::GPIO_NUM_32; // Serial1 TxD Output, 3.3V TTL.
const gpio_num_t MUX_PIN      = gpio_num_t::GPIO_NUM_21; // Audio Signal MUX Control, Output.
const gpio_num_t TONE_PIN     = gpio_num_t::GPIO_NUM_25; // PWM Test Tone Pin, Output.

// GPIO Pin States
const int SIGN_OFF = 0;
const int SIGN_ON  = 1;
const int TONE_OFF = 1;
const int TONE_ON  = 0;

// HTTP Controller
#define  HTTP_CMD_STR         "/cmd?"             // Command preamble.
#define  HTTP_EMPTY_RESP_STR  "get /favicon.ico"  // Empty Reply, ignore this payload.
#define  HTTP_POST_STR        "content-length:"   // Post data length Keyword.
#define  HTTP_CMD_END_STR     "http/"             // This string marks the End of HTTP Controller Command.


const uint16_t HTTP_RESPONSE_MAX_SZ = 225;        // Maximum Chars Allowed in HTTP client response.

// I2C:
const uint8_t  I2C_QN8027_ADDR = 0x2c;            // I2C Address of QN8027 FM Radio Chip.
const uint8_t  I2C_DEV_CNT     = 1;               // Number of expected i2c devices on bus.
const uint32_t I2C_FREQ_HZ     = 100000;          // I2C master clock frequency

// Measurement:
const int32_t MEAS_TIME = 50;                     // Measurement Refresh Time, in mS.

// Radio
const uint16_t AUDIO_LEVEL_MAX = 675;             // Maxium Audio Level (peak mV) Measurement by QN8027.
const uint32_t AUDIO_MEAS_TIME = 2000;            // Sample period for Audio Level Meas.
const uint8_t  RADIO_CAL_RETRY = 3;               // RF Port Calibration Retry Count (Maximum Retry Count).
const uint8_t  RF_LOW_POWER    = 27;              // 88dBuv.
const uint8_t  RF_MED_POWER    = 40;              // 96dBuV.
const uint8_t  RF_HIGH_POWER   = 78;              // 119dBuV. Do Not change.

// RDS:
const uint16_t RDS_DSP_TM_MAX   = 900;            // Maximum RadioText Display Period, in secs.
const uint16_t RDS_DSP_TM_MIN   = 5;              // Minimum RadioText Display Period, in secs.
const uint32_t RDS_DSP_TM_DEF   = 15000;          // Default Radio Display Period, in mS.
const uint16_t RDS_MSG_UPD_TIME = 1000;           // RadioText UI Update Time, im mS.
const uint16_t RDS_PI_CODE_DEF  = 0x6400;         // Default RDS PI Code, 16-bit hex value, 0x00ff - 0xffff.
const uint32_t RDS_PI_CODE_MAX  = 0xffff;         // Maximum PI Code Value (hex).
const uint32_t RDS_PI_CODE_MIN  = 0x00ff;         // Minumum PI Code Value (hex).
const uint8_t  RDS_PSN_MAX_SZ   = CMD_PSN_MAX_SZ; // RDS Program Service Name, Max Allowed Length.
const uint8_t  RDS_PTY_CODE_DEF = 9;              // Default RDS PTY Code "Top 40", 0-29 allowed.
const uint8_t  RDS_PTY_CODE_MIN = 0;              // Min RDS PTY Code "None", See https://en.wikipedia.org/wiki/Radio_Data_System
const uint8_t  RDS_PTY_CODE_MAX = 29;             // Max RDS PTY Code "Weather".
const uint8_t  RDS_REFRESH_TM   = 5;              // RadioText Refresh Time, in Seconds.
const uint8_t  RDS_TEXT_MAX_SZ  = CMD_RT_MAX_SZ;  // RDS RadioText Message, Max Allowed Length.

// RSSI:
const uint16_t RSSI_UPD_TIME = 2500;              // RSSI GUI Update time (on homeTab), in mS.

// Serial Controller
#define SERIAL_LOG_STR       "log"                // Serial Log Command Keyword.

// Test Tone
const uint8_t  TEST_TONE_CHNL = 0;                // Test Tone PWM Channel.
const uint32_t TEST_TONE_TIME = 300;              // Test Tone Sequence Time, in mS.
const uint16_t TONE_A3        = 220;              // 220Hz Audio Tone.
const uint16_t TONE_A4        = 440;
const uint16_t TONE_C4        = 262;
const uint16_t TONE_C5        = 523;
const uint16_t TONE_E4        = 330;
const uint16_t TONE_F4        = 349;
const uint16_t TONE_NONE      = 0;

// Time Conversion
const uint32_t MSECS_PER_SEC = 1000UL;
const uint32_t SECS_PER_HOUR = 3600UL;
const uint32_t SECS_PER_DAY  = SECS_PER_HOUR * 24UL;
const uint32_t SECS_PER_MIN  = 60UL;

// OTA:
const uint16_t OTA_PORT    = 3232;     // Port for OTA.
const uint16_t OTA_TIMEOUT = 3000;     // Max allowed time to receive OTA data.

// Volts:
const float MIN_VOLTS         = 4.5f;  // Minimum Power Supply volts.
const float VOLTS_HYSTERESIS  = 0.15f; // Voltage Hysterisis.
const uint16_t VOLTS_UPD_TIME = 3750;  // Power Supply Volts GUI Update time (on diagTab), in mS.
// const uint32_t CLIENT_TIMEOUT = 500;   // Webserver Client Timeout, in mS.

// Web Server
#define HTML_HEADER_STR   "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
#define HTML_DOCTYPE_STR  "<!DOCTYPE HTML>\r\n<html>"
#define HTML_CLOSE_STR    "</html>\r\n\r\n"

const uint16_t HTTP_PORT         = 8080; // Port for HTTP commands
const uint16_t WEBSERVER_PORT    = 80;    // Port for Web Server. Do not change.

// *********************************************************************************************

// Controller Command Prototypes
bool    audioModeCmd(String  payloadStr,
                     ControllerTypeId controller);
bool    frequencyCmd(String  payloadStr,
                     ControllerTypeId controller);
bool    infoCmd(String  payloadStr,
                ControllerTypeId controller);
int16_t getCommandArg(String& requestStr,
                      uint8_t maxSize);
// uint8_t getControllerStatus(void);
#ifdef OldWay
bool    gpioCmd(String  payloadStr,
                ControllerTypeId controller,
                uint8_t pin);
bool    logCmd(String  payloadStr,
               ControllerTypeId controller);
bool    muteCmd(String  payloadStr,
                ControllerTypeId controller);
bool    piCodeCmd(String  payloadStr,
                  ControllerTypeId controller);
bool    ptyCodeCmd(String  payloadStr,
                  ControllerTypeId controller);
bool    programServiceNameCmd(String  payloadStr,
                              ControllerTypeId controller);
bool    radioTextCmd(String  payloadStr,
                     ControllerTypeId controller);
bool    rdsTimePeriodCmd(String  payloadStr,
                         ControllerTypeId controller);
bool    rebootCmd(String  payloadStr,
                  ControllerTypeId controller);
bool    rfCarrierCmd(String  payloadStr,
                  ControllerTypeId controller);
bool    startCmd(String  payloadStr,
                 ControllerTypeId controller);
bool    stopCmd(String  payloadStr,
                ControllerTypeId controller);
#endif // def OldWay

// ESPUI (WebGUI) Prototypes
void   buildGUI(void);
void   displayActiveController(uint8_t controller);
void   displayRdsText(void);
void   displaySaveWarning(void);
int8_t getAudioGain(void);
void   initCustomCss(void);
void   startGUI(void);
void   updateUiAudioLevel(void);
void   updateUiAudioMode(bool SteroMode);
void   updateUiAudioMute(bool value);
void   updateUiFreeMemory(void);
void   updateUiFrequency(int Freq10x);
bool   updateUiGpioMsg(gpio_num_t pin, String & ControllerName, bool PinState);
void   updateUiIpaddress(String ipStr);
void   updateUiLocalPtyCode(void);
void   updateUiRdsText(String textStr);
void   updateUiRDSTmr(bool ClearDisplay);
void   updateUiRfCarrier(void);
void   updateUiDiagTimer(void);
void   updateUiVolts(void);
void   updateUiWfiMode(void);

// ESPUI Callbacks
void   adjFmFreqCallback(Control *sender,
                         int      type);
void   apBootCallback(Control *sender,
                      int      type);
void   apFallBkCallback(Control *sender,
                        int      type);
void   audioCallback(Control *sender,
                     int      type);
void   backupCallback(Control *sender,
                      int      type);
void   dhcpCallback(Control *sender,
                    int      type);
void   controllerCallback(Control *sender,
                          int      type);
void   diagBootCallback(Control *sender,
                        int      type);
void   diagLogCallback(Control *sender,
                       int      type);
void   gainAdjustCallback(Control *sender,
                          int      type);
void   gpioCallback(Control *sender,
                    int      type);
void   impAdjustCallback(Control *sender,
                         int      type);
void   muteCallback(Control *sender,
                    int      type);
void   radioEmphasisCallback(Control *sender,
                             int      type);
void   rdsRstCallback(Control *sender,
                      int      type);
void   rdsTextCallback(Control *sender,
                       int      type);
void   rfAutoCallback(Control *sender,
                      int      type);
void   rfCarrierCallback(Control *sender,
                         int      type);
void   rfPowerCallback(Control *sender,
                       int      type);
void   saveSettingsCallback(Control *sender,
                            int      type);
void   setLoginCallback(Control *sender,
                        int      type);
void setPiCodeCallback(Control *sender,
                       int      type);
void setPtyCodeCallback(Control *sender,
                        int      type);
void setWiFiAuthenticationCallback(Control *sender,
                                   int      type);
void setWiFiNamesCallback(Control *sender,
                          int      type);
void setWiFiAddrsCallback(Control *sender,
                          int      type);

void testModeCallback(Control *sender,
                      int      type);

void volAdjustCallback(Control *sender,
                       int      type);

// File System (LITTLEFS) prototypes
void         instalLogoImageFile(void);
const String makeWebGif(String   fileName,
                        uint16_t width,
                        uint16_t height,
                        String   backGroundColorStr);
void littlefsInit(void);


// JSON Prototypes
bool checkEmergencyCredentials(const char *fileName);
bool restoreConfiguration(uint8_t     restoreMode,
                          const char *fileName);
bool saveConfiguration(uint8_t     saveMode,
                       const char *filename);

// Measure Prototypes
void         initVdcAdc(void);
void         initVdcBuffer(void);
void         measureVbatVoltage(void);
void         measureVdcVoltage(void);
void         processMeasurements(void);

// Misc Prototypes
void         initEprom(void);
uint8_t      i2cScanner(void);
void         rebootSystem(void);
void         setGpioBootPins(void);
void         spiSdCardShutDown(void);
bool         strIsUint(String intStr);
void         toneInit(void);
void         toneOn(uint8_t  pin,
                    uint16_t freq,
                    uint8_t  channel);
void         toneOff(uint8_t pin,
                     uint8_t channel);
void         updateGpioBootPins(void);
void         updateTestTones(bool resetTimerFlg);

const String returnClientCode(int code);

// OTA Prototypes
void         otaInit(String & mdnsname);

// Radio Protypes
bool         calibrateAntenna(void);
bool         checkRadioIsPresent(void);
uint8_t      initRadioChip(void);
uint16_t     measureAudioLevel(void);
void         setAudioImpedance(void);
void         setAudioMute(void);
void         setDigitalGain(void);
void         setFrequency(void);
void         setMonoAudio(void);
void         setPreEmphasis(void);
void         setRfAutoOff(void);
void         setRfCarrier(void);
void         setRfPower(void);
void         setVgaGain(void);
void         updateOnAirSign(void);
void         updateRadioSettings(void);
void         waitForIdle(uint16_t waitMs);

// RDS Prototypes
void         processRDS(void);
void         resetControllerRdsValues(void);

// Serial Log
uint8_t      getLogLevel(void);
void         initSerialLog(bool verbose);

// webServer Prototypes
void         processWebClient(void);

String       urlDecode(String urlStr);
uint8_t      urlDecodeHex(char c);

template <typename J, typename N>
bool ReadFromJSON (float & OutValue, J& Json, N Name)
{
    bool HasBeenModified = false;

    if (true == Json.containsKey (Name))
    {
        float temp = Json[Name];
        if (fabs (temp - OutValue) > 0.000005F)
        {
            OutValue = temp;
            HasBeenModified = true;
        }
    }

    return HasBeenModified;
};

template <typename T, typename J, typename N>
bool ReadFromJSON (T& OutValue, J& Json, N Name)
{
    bool HasBeenModified = false;

    if (true == Json.containsKey (Name))
    {
        T temp = Json[Name];
        if (temp != OutValue)
        {
            OutValue = temp;
            HasBeenModified = true;
        }
    }

    return HasBeenModified;
};

// *********************************************************************************************
// EOF
