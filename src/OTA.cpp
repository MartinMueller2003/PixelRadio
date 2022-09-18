/*
  *    File: OTA.cpp
  *    Project: RBDS/RDS FM Transmitter (QN8027)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *    Notes:
  *    1. To OTA bin file via Platformio, run powershell command:
  *         platformio run --target upload --upload-port <DEVICE IP ADDR HERE>
  *    2. To OTA LITTLEFS via Platformio, run powershell command:
  *         platformio run --target uploadfs --upload-port <DEVICE IP ADDR HERE>
  */

// *********************************************************************************************

#include "config.h"
#include "globals.h"
#include "PixelRadio.h"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <WiFi.h>

#ifdef OTA_ENB
    // *********************************************************************************************
    // Must call wifiConnect() BEFORE calling otaInit().
    void otaInit (String & mdnsname)
    {
        static int8_t   prog            = 0;    // Upload Progress, in percent.
        static int8_t   oldProg         = -1;   // Previous Upload Progress.
        static bool     updateLittleFS  = false;    // Flag for LITTLEFS OTA.

        ArduinoOTA.setPort (OTA_PORT);
        ArduinoOTA.setTimeout (OTA_TIMEOUT);    // Default is 1000mS, increased to prevent OTA timeouts.
        ArduinoOTA
        .onStart ([] ()
            {
                oldProg         = -1;
                String typeStr  = emptyString;

                if (ArduinoOTA.getCommand () == U_FLASH)
                {
                    typeStr         = F ("Sketch Firmware.");
                    updateLittleFS  = false;
                }
                else
                {
                    typeStr         = F ("LITTLEFS File System.");
                    updateLittleFS  = true;
                }

                char logBuff[50];
                sprintf (logBuff, "Starting OTA Update: %s", typeStr.c_str ());
                Log.infoln (logBuff);

                // WiFi.setTxPower(MAX_WIFI_PWR);
                // Log.infoln(" -> Set WiFi RF Power to Maximum.");

                if (updateLittleFS == true)
                {
                    LittleFS.end ();    // Important, Must Unmount LITTLEFS!
                    Log.infoln (" -> Unmounted LITTLEFS.");
                }
            })

        .onEnd ([] ()
            {
                delay (10);
                // WiFi.setTxPower(RUN_WIFI_PWR);
                // Log.infoln(" -> Restored Normal WiFi RF Power.\r\n");
                #    ifdef OTA_REBOOT_ENB
                    Log.fatalln ("OTA COMPLETE, SYSTEM REBOOTING ...\r\n");
                #    else // ifdef OTA_REBOOT_ENB
                    Log.warningln ("OTA COMPLETE, PLEASE REBOOT NOW.\r\n");

                    if (updateLittleFS == true)
                    {
                        littlefsInit ();
                        // Log.infoln(" -> Remounted LITTLEFS.");
                        Serial.flush ();
                    }

                #    endif // ifdef OTA_REBOOT_ENB
                Serial.flush ();
            })

        .onProgress ([] (unsigned int progress, unsigned int total)
            {
                prog = (progress / (total / 100)) % 2;  // Progress value changes in 2% increments.

                if ((prog == 0) && (prog != oldProg))
                {
                    Log.infoln (" -> Progress: %u", (progress / (total / 100)));    // Print progress to console.
                }

                oldProg = prog;
            })

        .onError ([] (ota_error_t error)
            {
                Log.warningln (" -> OTA Error[%u]: ", error);

                if (error == OTA_AUTH_ERROR)
                {
                    Log.errorln ("Auth Failed.");
                }
                else if (error == OTA_BEGIN_ERROR)
                {
                    Log.errorln ("Begin Failed.");
                }
                else if (error == OTA_CONNECT_ERROR)
                {
                    Log.errorln ("Connect Failed.");
                }
                else if (error == OTA_RECEIVE_ERROR)
                {
                    Log.errorln ("Receive Failed.");
                }
                else if (error == OTA_END_ERROR)
                {
                    Log.errorln ("End Failed.");
                }
                else
                {
                    Log.errorln ("Unknown Error.");
                }

                //    Log.warningln("OTA Exit, Rebooting Now!");
                //    Serial.flush();
                //    delay(100);
                //    ESP.restart();  // Reboot if OTA fails.

                // WiFi.setTxPower(RUN_WIFI_PWR);
                Log.infoln (" -> Restored Normal WiFi RF Power.");
                Log.errorln ("OTA Exit, Aborted. Please try again.");
                Serial.flush ();

                if (updateLittleFS == true)
                {
                    littlefsInit ();
                }
            });

        ArduinoOTA.setHostname (mdnsname.c_str ()); // OTA and mDNS must share the same Host name.
        #    ifdef OTA_REBOOT_ENB
            ArduinoOTA. setRebootOnSuccess (true);  // Enable reboot after OTA.
        #    else // ifdef OTA_REBOOT_ENB
            ArduinoOTA. setRebootOnSuccess (false); // Disable reboot after OTA.
        #    endif // ifndef OTA_REBOOT_ENB
        ArduinoOTA.begin ();
        Log.infoln ("OTA Server Initialized.");
    }

    // *********************************************************************************************
#endif // ifdef OTA_ENB
// EOF
