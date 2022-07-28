/*
   File: PixelRadio.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022.
   Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Revision History:
   V0.0,   Dec-16-2021: Started Project.
   V0.1,   Jan-20-2022: Updated to ESP32UI PR#149 (Jan-20-2022 build). Resolved ongoing ESPUI Issues.
   V0.2,   Jan-28-2022: Migrated Hardware, ICStation FM Transmitter Board changed to basic QN8027 I2C Module.
   V0.3,   Feb-04-2022: Updated platformio/espressif32 3.4.0 to 3.5.0.
   V1.0B,  Feb-28-2022: Non-public beta release by thomastech.
   V1.0,   Apr-05-2022: First Public Release.
   V1.1.0, Jun-13-2022: Updated espressif32 platform to 4.4.0, was 3.5.0. Core now 6.0.2, was 5.2.5
                        PlatformIO Package = framework-arduinoespressif32 3.20003.0 (2.0.3).
                        Updated ArduinoJson to 6.19.4, was 6.19.1
                        Updated LittleFS to 2.0.0 (built-in), was 1.0.6.
                        Changed platformio.ini board type to ttgo-t1, was esp32dev.
                        Added PTY (Program Type) command to HTTP, MQTT, and Serial Controllers (dkulp submission).
                        Added programmable Local Controller PTY code to Web UI (on Local RDS Tab).
                        Added RF Carrier On/Off to the Controller API.
                        Fixed 49 day System Run Time hang.
                        Fixed Web UI's displayed IP Address so it updates on reconnects.
                        Added test for broken QN8027 Oscillator (post failure to serial log).
                        Updated test tone routine for compatibility with latest espressif32 + removed blocking code.
                        Corrected millis var type (found some that were int32, must be uint32).
                        Added Percent (%) Decoding to HTTP API's URL.
                        Performed misc code cleanup.
                        Updated Readme documentation, added photos for external WiFi antenna modification.
                        -> Validation completed on Jun-17-2022, public release approved.

   Notes:
   1. This "Arduino" project must be compiled with VSCode / Platformio. Do not use the Arduino IDE.
   2. The target CPU is a LilyGO "TTGO T8" (V1.7.1) ESP32 with 4MB Flash (minimum).
      Early version of the TTGO-T8 used the CP2104 USB UART. Latest versions use the CH9102F. WIN10 will automatically
      install the driver.
      Important: The TTGO-T8 slide switch must be set "ON" (position nearest the RST switch). Otherwise USB is disabled.
   3. This project's QN8027 Arduino Library has been modified for use with PixelRadio. The original library version can be
      found here:
      https://github.com/ManojBhakarPCM/Arduino-QN8027-with-Full-RDS-support

    4. Flash Tool Memory Map
        0x1000  bootloader_dio_40m.bin
        0x8000  partitions.bin
        0xe000  boot_app0.bin
        0x10000 pixelradio.bin
 */

// ************************************************************************************************
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ArduinoLog.h>
#include <DNSServer.h>
#include <WiFi.h>
#include "config.h"
#include "credentials.h"
#include "PixelRadio.h"
#include "language.h"
#include "QN8027Radio.h"
#include "Controllers/ControllerMgr.h"
#include "Network/WiFiDriver.hpp"

// ************************************************************************************************
// Global Section
//
QN8027Radio radio = QN8027Radio();           // FM Transmitter Module (QN8027 radio)

// Global System Vars

bool newAutoRfFlg    = false;                // new RF Auto Off Setting Avail Semaphore.
bool newCarrierFlg   = false;                // New Carrier Settings Avail Semaphore.
bool newDigGainFlg   = false;                // New Digital Audio Gain Setting Avail Semaphore.
bool newGpio19Flg    = false;                // New GPIO Pin 19 State.
bool newGpio23Flg    = false;                // New GPIO Pin 23 State.
bool newGpio33Flg    = false;                // New GPIO Pin 33 State.
bool newFreqFlg      = false;                // New Radio Frequency Setting Avail Semaphore.
bool newInpImpFlg    = false;                // New Input Impedance Setting Avail Semaphore.
bool newAudioModeFlg = false;                // New Mono/Stereo Audio Setting Avail Semaphore.
bool newMuteFlg      = false;                // New Audio Mute Setting Avail Semaphore.
bool newPreEmphFlg   = false;                // New Radio Pre-Emphasis Setting Avail Semaphore.
bool newRfPowerFlg   = false;                // New RfPower Setting Avail Semaphore.
bool newVgaGainFlg   = false;                // New Analog VGA Gain Setting Avail Semaphore.
bool rebootFlg       = false;                // Reboot System if true;
bool stopHttpFlg     = false;                // HTTP Controller Stop Command was sent if true.
bool stopMqttFlg     = false;                // MQTT Controller Stop Command was sent if true.
bool stopSerialFlg   = false;                // Serial Controller Stop Command was sent if true.
bool testModeFlg     = false;                // Audio Test Tone Mode if true. Do NOT save in config file.

uint8_t fmRadioTestCode = FM_TEST_OK;        // FM Radio Module Test Result Code.


uint32_t rdsMsgTime       = RDS_DSP_TM_DEF;  // Global (Master) RDS Message Time. Set by RDS Controllers.
float vbatVolts = 0.0f;                      // ESP32's Onboard "VBAT" Voltage. Typically 5V.
float paVolts   = 0.0f;                      // RF Power Amp's Power Supply Voltage. Typically 9V.

String gpio19CtrlStr    = "";                // GPIO-19 State if Changed by Serial/MQTT/HTTP Controller.
String gpio23CtrlStr    = "";                // GPIO-23 State if Changed by Serial/MQTT/HTTP Controller.
String gpio33CtrlStr    = "";                // GPIO-33 State if Changed by Serial/MQTT/HTTP Controller.
String rdsTextMsgStr    = "";                // Current RDS RadioText Message.

#ifdef OldWay
 ipAddrStr        = "";                // DHCP IP Address;

IPAddress hotSpotIP   = HOTSPOT_IP_DEF;
IPAddress staticIP    = IP_ADDR_DEF;
IPAddress subNet      = SUBNET_MASK_DEF;
IPAddress wifiDNS     = WIFI_ADDR_DEF;
IPAddress wifiGateway = WIFI_ADDR_DEF;
#endif // def OldWay

// ************************************************************************************************
// Configuration Vars (Can be saved to LittleFS and SD Card)

bool apFallBackFlg = AP_FALLBACK_DEF_FLG;                  // Control, Switch to AP mode if STA fails.
bool muteFlg        = RADIO_MUTE_DEF_FLG;                  // Control, Mute audio if true.
bool rfAutoFlg      = RF_AUTO_OFF_DEF_FLG;                 // Control, Turn Off RF carrier if no audio for 60Sec. false=Never turn off.
bool rfCarrierFlg   = RF_CARRIER_DEF_FLG;                  // Control, Turn off RF if false.
bool stereoEnbFlg   = STEREO_ENB_DEF_FLG;                  // Control, Enable Stereo FM if true (false = Mono).
// bool wifiDhcpFlg    = CTRL_DHCP_DEF_FLG;                   // Control, Use DHCP if true, else static IP.
// bool WiFiRebootFlg  = WIFI_REBOOT_DEF_FLG;                 // Control, Reboot if all connections fail.

uint8_t analogVol = (atoi(ANA_VOL_DEF_STR));               // Control. Unused, for future expansion.
uint8_t usbVol    = (atoi(USB_VOL_DEF_STR));               // Control. Unused, for future expansion.

// uint8_t  rdsLocalPtyCode = RDS_PTY_CODE_DEF;               // Control. Local PTY Code, default for all controllers.
// uint16_t rdsLocalPiCode  = RDS_PI_CODE_DEF;                // Control. Local PI Code, default for all controllers.

uint32_t baudRate = ESP_BAUD_DEF;                          // Control.

uint16_t fmFreqX10 = FM_FREQ_DEF_X10;                      // Control. FM MHz Frequency X 10 (881 - 1079).

String preEmphasisStr = PRE_EMPH_DEF_STR;                  // Control.
String digitalGainStr = DIG_GAIN_DEF_STR;                  // Control.
String gpio19BootStr  = GPIO_DEF_STR;                      // Control.
String gpio23BootStr  = GPIO_DEF_STR;                      // Control.
String gpio33BootStr  = GPIO_DEF_STR;                      // Control.
String inpImpedStr    = INP_IMP_DEF_STR;                   // Control.
String logLevelStr    = DIAG_LOG_DEF_STR;                  // Control, Serial Log Level.
String rfPowerStr     = RF_PWR_DEF_STR;                    // Control.
#ifdef OldWay
String staticIpStr    = "";
String subNetStr      = subNet.toString();                 // Control.
#endif // def OldWay
String userNameStr    = LOGIN_USER_NAME_STR;               // Control.
String userPassStr    = LOGIN_USER_PW_STR;                 // Control.
String vgaGainStr     = VGA_GAIN_DEF_STR;                  // Control.
#ifdef OldWay
String wifiDnsStr     = "";                                // Control.
String wifiGatewayStr = "";                                // Control.
String wifiSSIDStr    = SSID_NM_STR;                       // Control, Private WiFi Credentials.
String wifiWpaKeyStr  = WPA_KEY_STR;                       // Control, Private WiFi Credentials.
#endif // def OldWay

// *********************************************************************************************

void setup()
{
    bool successFlg = true;
    char logBuff[80];

    // enableCore1WDT();
    pinMode(ON_AIR_PIN, OUTPUT);       // "ON AIR" Sign Driver Output.
    pinMode(MISO_PIN,   INPUT_PULLUP); // MISO Requires Internal Pull-up.
    pinMode(SCL_PIN,    INPUT_PULLUP); // I2C Clock Pin.
    pinMode(SDA_PIN,    INPUT);        // I2C Data Pin. Do NOT Enable Internal Pullup.
    pinMode(MUX_PIN,    OUTPUT);       // Audio MUX Control (Line-In:Tone-In), Output.
    pinMode(TONE_PIN,   OUTPUT);       // PWM Audio Test Tone, Output.
    pinMode(SD_CS_PIN,  OUTPUT);       // SD Card Chip Select, Output.

    digitalWrite(MUX_PIN,   TONE_OFF); // Init Audio Mux, Enable Audio Line-In Jack, Music LED On.
    digitalWrite(SD_CS_PIN, HIGH);

    // Initialize Tone Generator.
    toneInit();

    // Initialize i2c.
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(I2C_FREQ_HZ);     // 100KHz i2c speed.
    pinMode(SCL_PIN, INPUT_PULLUP); // I2C Clock Pin.

    // delay(3000);                 // DEBUG ONLY, wait for Platformio's monitor terminal.

    // Initialize USB Serial.
    Serial.begin(baudRate, SERIAL_8N1); // Open Serial-0 Port to Log System Messages.
    // Serial1.begin(baudRate, SERIAL_8N1, SER1_RXD, SER1_TXD); // Optional Serial-1 Port.
    // Serial1.println("\r\n\r\n");
    // Serial1.println("COM1 NOW ALIVE");

    while (!Serial && !Serial.available()) {} // Wait for ESP32 Device Serial Port to be available.
    Serial.println("\r\n\r\n");
    Serial.flush();

    // Initialize the System Serial Log.
    initSerialLog(true); // Initally Set to verbose log level. Will use config file setting at end of setup.

    // Let's Start System Initialization
    Log.infoln((String(F("PixelRadio FM Transmitter ")) + F(AUTHOR_STR)).c_str());
    Log.infoln(((String(F("Version ")) + F(VERSION_STR) + F(", ") + F(BLD_DATE_STR))).c_str());
    Log.infoln(F("System is Starting ..."));

    // Initialize EEPROM Emulation.
    // initEprom();

    // Setup ADC
    initVdcAdc();    // Initialize the Bat Voltage ADC.
    initVdcBuffer(); // Initialize the two Power Supply Measurement Buffers.
    Log.infoln(F("Initialized VBAT and RF_VDC ADCs."));

    // Setup the File System.
    littlefsInit();
    instalLogoImageFile();
    WiFiDriver.Begin();
    ControllerMgr.begin();

    // Restore System Settings from File System.
    restoreConfiguration(LITTLEFS_MODE, BACKUP_FILE_NAME);
    resetControllerRdsValues();                             // Must be called after restoreConfiguration().
    setGpioBootPins();                                      // Must be called after restoreConfiguration().

    if (checkEmergencyCredentials(CRED_FILE_NAME)) {        // Check for Emergency WiFi Credential File on SD Card.
        saveConfiguration(LITTLEFS_MODE, BACKUP_FILE_NAME); // Save restored credentials to file system.
    }

    digitalWrite(MUX_PIN, TONE_ON);                         // Turn off Music (Mux) LED.

    digitalWrite(MUX_PIN, TONE_OFF); // Turn on Music (Mux) LED, restore Line-In to external audio.

    // Startup the I2C Devices).
    i2cScanner();                      // Scan the i2c bus and report all devices.
    fmRadioTestCode = initRadioChip(); // If QN8027 fails we will warn user on UI homeTab.
    Log.infoln(F("FM Radio RDS/RBDS Started."));

    // Startup the Web GUI. DO THIS LAST!
    Log.infoln(F("Initializing Web UI ..."));
    startGUI();
    Log.infoln(F("-> Web UI Loaded."));

    // End of System Initialization. Report pass/fail message to Serial Log.
    if (successFlg && (fmRadioTestCode == FM_TEST_OK)) {
        Log.infoln("PixelRadio System Init Complete, Success!");
    }
    else {
        Log.fatalln("PixelRadio System Init Failed. Please Review Serial Log.");
    }

    Log.infoln("Changing Log Level to %s", logLevelStr.c_str());
    initSerialLog(false);
    Serial.flush();

}

// *********************************************************************************************
// Main Loop.
void loop()
{
    Log.setLevel(LOG_LEVEL_INFO);

    // Background tasks
    WiFiDriver.Poll();
    ControllerMgr.poll();
#ifdef OldWay 
    processDnsServer();     // AP ESPUI DNS Server
    processRDS();           // Send any available RadioText.
    processMeasurements();  // Measure the two system voltages.

    updateUiRSSI();         // Update the WiFi Signal Strength on UI homeTab & wifiTab.
    updateUiFreeMemory();   // Update the Memory value on UI diagTab.
    updateUiAudioLevel();   // Update the Audio Level value on UI diagtab.
    updateUiDiagTimer();    // Upddate the Elapsed Timer on UI diagTab.
    updateUiVolts();        // Update the two system voltages on UI diagTab.

    updateRadioSettings();  // Update the QN8027 device registers.
    updateGpioBootPins();   // Update the User Programmable GPIO Pins.
    updateTestTones(false); // Update the Test Tone, false=Don't Reset Tone Sequence.
    updateOnAirSign();      // Update the Optional "On Air" 12V LED Sign.

    wifiReconnect();        // Reconnect to WiFi if not connected to router.
    rebootSystem();         // Check to see if Reboot has been requested.

    #ifdef HTTP_ENB
    processWebClient();     // Process Any Available HTTP RDS commands.
    #endif // ifdef HTTP_ENB

    #ifdef OTA_ENB
    ArduinoOTA.handle(); // OTA Service.
    #endif // ifdef OTA_ENB
#endif // def OldWay 

}

// *********************************************************************************************

/*
   // Hardware interrupts are not used in this project. This isr is a placeholder for future use.
   //  Warning: It is not possible to execute delay() or yield() from an ISR, or do blocking operations,
   // or operations that disable the interrupts. Code MUST be short & sweet, such as set a flag then exit.
   // Function protype must use ICACHE_RAM_ATTR type.
   IRAM_ATTR void isr() // interrupt service routine
   {
   }
 */

// EOF
