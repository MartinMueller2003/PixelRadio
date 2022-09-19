/*
  *    File: PixelRadio.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech, dkulp
  *
  *    (c) copyright T. Black 2021-2022.
  *    Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *    Revision History:
  *    V0.0,   Dec-16-2021: Started Project.
  *    V0.1,   Jan-20-2022: Updated to ESP32UI PR#149 (Jan-20-2022 build). Resolved ongoing ESPUI Issues.
  *    V0.2,   Jan-28-2022: Migrated Hardware, ICStation FM Transmitter Board changed to basic QN8027 I2C Module.
  *    V0.3,   Feb-04-2022: Updated platformio/espressif32 3.4.0 to 3.5.0.
  *    V1.0B,  Feb-28-2022: Non-public beta release by thomastech.
  *    V1.0,   Apr-05-2022: First Public Release.
  *    V1.1.0, Jun-13-2022: Updated espressif32 platform to 4.4.0, was 3.5.0. Core now 6.0.2, was 5.2.5
  *                         PlatformIO Package = framework-arduinoespressif32 3.20003.0 (2.0.3).
  *                         Updated ArduinoJson to 6.19.4, was 6.19.1
  *                         Updated LittleFS to 2.0.0 (built-in), was 1.0.6.
  *                         Changed platformio.ini board type to ttgo-t1, was esp32dev.
  *                         Added PTY (Program Type) command to HTTP, MQTT, and Serial Controllers (dkulp submission).
  *                         Added programmable Local Controller PTY code to Web UI (on Local RDS Tab).
  *                         Added RF Carrier On/Off to the Controller API.
  *                         Fixed 49 day System Run Time hang.
  *                         Fixed Web UI's displayed IP Address so it updates on reconnects.
  *                         Added test for broken QN8027 Oscillator (post failure to serial log).
  *                         Updated test tone routine for compatibility with latest espressif32 + removed blocking code.
  *                         Corrected millis var type (found some that were int32, must be uint32).
  *                         Added Percent (%) Decoding to HTTP API's URL.
  *                         Performed misc code cleanup.
  *                         Updated Readme documentation, added photos for external WiFi antenna modification.
  *                         -> Validation completed on Jun-17-2022, public release approved.
  *
  *    Notes:
  *    1. This "Arduino" project must be compiled with VSCode / Platformio. Do not use the Arduino IDE.
  *    2. The target CPU is a LilyGO "TTGO T8" (V1.7.1) ESP32 with 4MB Flash (minimum).
  *       Early version of the TTGO-T8 used the CP2104 USB UART. Latest versions use the CH9102F. WIN10 will automatically
  *       install the driver.
  *       Important: The TTGO-T8 slide switch must be set "ON" (position nearest the RST switch). Otherwise USB is disabled.
  *    3. This project's QN8027 Arduino Library has been modified for use with PixelRadio. The original library version can be
  *       found here:
  *       https://github.com/ManojBhakarPCM/Arduino-QN8027-with-Full-RDS-support
  *
  *     4. Flash Tool Memory Map
  *         0x1000  bootloader_dio_40m.bin
  *         0x8000  partitions.bin
  *         0xe000  boot_app0.bin
  *         0x10000 pixelradio.bin
  */

// ************************************************************************************************

#include <Arduino.h>
#include <ArduinoLog.h>
// #include <ArduinoOTA.h>
// #include <WiFi.h>

#include "PixelRadio.h"
#include "config.h"
#include "credentials.h"
#include "ControllerMgr.h"
#include "language.h"
#include "memdebug.h"
#include "PeakAudio.hpp"
#include "radio.hpp"
#include "TestTone.hpp"
#include "WiFiDriver.hpp"
#include "Diagnostics.hpp"

// ************************************************************************************************
// Global Section
//

// Global System Vars

bool rebootFlg = false;     // Reboot System if true;

float   vbatVolts   = 0.0f; // ESP32's Onboard "VBAT" Voltage. Typically 5V.
float   paVolts     = 0.0f; // RF Power Amp's Power Supply Voltage. Typically 9V.

// ************************************************************************************************
// Configuration Vars (Can be saved to LittleFS and SD Card)
uint8_t usbVol = (atoi (USB_VOL_DEF_STR));  // Control. Unused, for future expansion.

// *********************************************************************************************

void setup ()
{
    bool    successFlg = true;
    char    logBuff[80];

    // enableCore1WDT();
    pinMode (   ON_AIR_PIN, OUTPUT);        // "ON AIR" Sign Driver Output.
    pinMode (   MISO_PIN,   INPUT_PULLUP);  // MISO Requires Internal Pull-up.
    pinMode (   SCL_PIN,    INPUT_PULLUP);  // I2C Clock Pin.
    pinMode (   SDA_PIN,    INPUT);         // I2C Data Pin. Do NOT Enable Internal Pullup.
    pinMode (   TONE_PIN,   OUTPUT);        // PWM Audio Test Tone, Output.
    pinMode (   SD_CS_PIN,  OUTPUT);        // SD Card Chip Select, Output.

    digitalWrite (SD_CS_PIN, HIGH);

    // Initialize Tone Generator.
    TestTone.Init ();

    // delay(3000);                 // DEBUG ONLY, wait for Platformio's monitor terminal.

    // Initialize USB Serial.
    Serial.begin (115200, SERIAL_8N1);  // Open Serial-0 Port to Log System Messages.
    // Serial1.begin(baudRate, SERIAL_8N1, SER1_RXD, SER1_TXD); // Optional Serial-1 Port.
    // Serial1.println("\r\n\r\n");
    // Serial1.println("COM1 NOW ALIVE");

    // while (!Serial && !Serial.available ())
    // {}  // Wait for ESP32 Device Serial Port to be available.
    Serial.println ("\r\n\r\n");
    Serial.flush ();

    // init log
    Diagnostics.begin ();

    // Let's Start System Initialization
    Log.infoln ((String (F ("PixelRadio FM Transmitter ")) + F (AUTHOR_STR)).c_str ());
    Log.infoln (((String (F ("Version ")) + F (VERSION_STR) + F (", ") + F (BLD_DATE_STR))).c_str ());
    Log.infoln (F ("System is Starting ..."));

    // Initialize EEPROM Emulation.
    // initEprom();

    // Setup the File System.
    littlefsInit ();
    instalLogoImageFile ();

    if (checkEmergencyCredentials (CRED_FILE_NAME))
    {
        // Check for Emergency WiFi Credential File on SD Card.
        saveConfiguration (LITTLEFS_MODE, BACKUP_FILE_NAME);    // Save restored credentials to file system.
    }

    WiFiDriver.Begin ();
    ControllerMgr.begin ();

    // Restore System Settings from File System.
    restoreConfiguration (LITTLEFS_MODE, BACKUP_FILE_NAME);

    #ifdef OldWay
        resetControllerRdsValues ();            // Must be called after restoreConfiguration().

        digitalWrite (  MUX_PIN,    TONE_ON);   // Turn off Music (Mux) LED.

        digitalWrite (  MUX_PIN,    TONE_OFF);  // Turn on Music (Mux) LED, restore Line-In to external audio.
    #endif // def OldWay

    // Startup the I2C Devices).
    i2cScanner ();  // Scan the i2c bus and report all devices.
    Radio.begin ();

    // Startup the Web GUI. DO THIS LAST!
    Log.infoln (F ("Initializing Web UI ..."));
    startGUI ();
    Log.infoln (F ("-> Web UI Loaded."));

    Serial.flush ();
}

// *********************************************************************************************
// Main Loop.
void loop ()
{
    // _ DEBUG_START;

    // Background tasks
    WiFiDriver.Poll ();
    ControllerMgr.poll ();
    Radio.Poll ();
    PeakAudio.poll ();
    Diagnostics.Poll ();

    #ifdef OldWay
        rebootSystem ();    // Check to see if Reboot has been requested.

        #    ifdef HTTP_ENB
            processWebClient ();    // Process Any Available HTTP RDS commands.
        #    endif // ifdef HTTP_ENB

    #endif // def OldWay
    // _ DEBUG_END;
}

// *********************************************************************************************

/*
  *    // Hardware interrupts are not used in this project. This isr is a placeholder for future use.
  *    //  Warning: It is not possible to execute delay() or yield() from an ISR, or do blocking operations,
  *    // or operations that disable the interrupts. Code MUST be short & sweet, such as set a flag then exit.
  *    // Function protype must use ICACHE_RAM_ATTR type.
  *    IRAM_ATTR void isr() // interrupt service routine
  *    {
  *    }
  */

// EOF
