/*
  *    File: backups.cpp (System Configuration Backup and Restore)
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T.Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T.Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *
  *    SD Card Emergency Credential Restoration Feature:
  *    -------------------------------------------------
  *    Credential Restoration allows the User to change WiFI credentials by SD Card.Reverts to DHCP WiFi.
  *    (1) File Creation:
  *     Create file named credentials.txt and add the following JSON formatted text to it (fill in your SSID and WPA_KEY):
  *     {"WIFI_SSID_STR":"SSID",
  *     "WIFI_WPA_KEY_STR":"WPA_KEY"}
  *    (2) Instructions:
  *        Install your prepared SD Card in PixelRadio.Reboot.Wait 30 secs, Remove card.
  *        Note: For Security the File is automatically deleted from card.
  */

// *************************************************************************************************************************
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <LittleFS.h>
#include <SD.h>
#include <SPI.h>

#include "PixelRadio.h"
#include "radio.hpp"
#include "WiFiDriver.hpp"
#include "LoginUser.hpp"
#include "LoginPassword.hpp"
#include "LoginPassword.hpp"
#include "Gpio19.hpp"
#include "Gpio23.hpp"
#include "Gpio33.hpp"
#include "Diagnostics.hpp"

#define PixelRadio_LittleFS LittleFS

// *************************************************************************************************************************
const uint16_t  JSON_CFG_SZ     = 10000;
const uint16_t  JSON_CRED_SIZE  = 300;

static const PROGMEM char * sdTypeStr [] =
{
    "Not Installed", "V1", "V2", "SDHC", "Unknown"
};
const uint8_t SD_TYPE_CNT = sizeof (sdTypeStr) / sizeof (sdTypeStr[0]);

// *************************************************************************************************************************
// checkEmergencyCredentials(): Restore credentials if credentials.txt is available.For use during boot.
//                              Return true if Emergency credentials were restored.
bool checkEmergencyCredentials (const char * fileName)
{
    bool    successFlg = true;
    File    file;
    SPIClass SPI2 (HSPI);

    SPI2.begin (SD_CLK_PIN, MISO_PIN, MOSI_PIN, SD_CS_PIN);
    pinMode (MISO_PIN, INPUT_PULLUP);   // MISO requires internal pull-up.
    SD.end ();                          // Reset interface (in case SD card had been swapped).
    spiSdCardShutDown ();

    if (!SD.begin (SD_CS_PIN, SPI2))    // SD Card Missing, nothing to do, exit.
    {
        SD.end ();
        spiSdCardShutDown ();

        return false;
    }

    if (SD.exists (fileName))   // Found Special Credential File.
    {
        Log.infoln (F ("Restoring WiFi Credentials From SD Card ..."));
    }
    else
    {
        SD.end ();
        spiSdCardShutDown ();

        return false;
    }

    Log.infoln (F ("-> SD Card Type: %s"), SD.cardType () < SD_TYPE_CNT ? sdTypeStr[SD.cardType ()] : "Error");
    file = SD.open (fileName, FILE_READ);

    DynamicJsonDocument doc (JSON_CRED_SIZE);
    DeserializationError error = deserializeJson (doc, file);

    file.close ();
    SD.remove (fileName);   // Erase File for security protection.
    SD.end ();
    // SPI2.end();
    spiSdCardShutDown ();

    if (error)
    {
        Log.errorln (F ("checkEmergencyCredentials: Deserialization Failed, Error:%s."), error.c_str ());
        spiSdCardShutDown ();

        return false;
    }

    if (((const char *)doc["WIFI_SSID_STR"] != NULL) && ((const char *)doc["WIFI_WPA_KEY_STR"] != NULL))
    {
        JsonObject root = doc.as <JsonObject>();
        WiFiDriver.restoreConfiguration (root);
        Log.warningln ( F ("-> User Provided New WiFi Credentials."));
        Log.warningln ( F ("-> Will Use DHCP Mode on this Session."));
        Log.verboseln (F ("-> Credentials JSON used %u Bytes."), doc.memoryUsage ());
        Log.infoln    (F ("-> Credentials Restore Complete."));
        Log.warningln (F ("-> For Your Security the Credential File Has Been Deleted."));
    }
    else
    {
        Log.errorln   (F ("-> Credential Restore Failed, Invalid / Incomplete File Contents."));
        Log.warningln (F ("-> The Credential File Has NOT Been Deleted.Please Secure Your Data."));
        successFlg = false;
    }

    // serializeJsonPretty(doc, Serial); // Debug Output
    // Serial.println();

    doc.clear ();

    return successFlg;
}

// *************************************************************************************************************************
// saveConfiguration(): Save the System Configuration to LittleFS or SD Card.
// SD Card Date Stamp is Jan-01-1980.Wasn't able to write actual time stamp because SDFat library conflicts with PixelRadio_LittleFS.h.
bool saveConfiguration (uint8_t saveMode, const char * fileName)
{
    bool    successFlg = false;
    File    file;
    SPIClass SPI2 (HSPI);

    // Log.infoln (String(F ("saveConfiguration: Start")).c_str());

    if (SystemBooting)
    {
        // ignore save requests while we are booting
        return true;
    }

    if (saveMode == LITTLEFS_MODE)
    {
        Log.infoln ((String (F ("Backup Configuration to LittleFS: '")) + String (fileName) + "'").c_str ());
        PixelRadio_LittleFS.begin (false);
        PixelRadio_LittleFS.remove (fileName);
        file = PixelRadio_LittleFS.open (fileName, FILE_WRITE);
    }
    else if (saveMode == SD_CARD_MODE)
    {
        Log.infoln ((String (F ("Backup Configuration to SD Card: '")) + String (fileName) + "'").c_str ());
        SPI2.begin (SD_CLK_PIN, MISO_PIN, MOSI_PIN, SD_CS_PIN);
        pinMode (MISO_PIN, INPUT_PULLUP);   // MISO requires internal pull-up.
        SD.end ();                          // Re-init Interface in case SD card had been swapped).

        if (!SD.begin (SD_CS_PIN, SPI2))
        {
            Log.errorln (F ("-> SD Card failed Initialization, Aborted."));
            SD.end ();
            spiSdCardShutDown ();

            return false;
        }

        // SD.remove(fileName);
        Log.infoln (F ("-> SD Card Type: %s"), SD.cardType () < SD_TYPE_CNT ? sdTypeStr[SD.cardType ()] : "Error");
        file = SD.open (fileName, FILE_WRITE);
    }
    else
    {
        Log.errorln (F ("saveConfiguration: Undefined Backup Mode, Aborted."));

        return false;
    }

    if (!file)
    {
        Log.errorln (F ("-> Failed to create file"));

        if (saveMode == SD_CARD_MODE)
        {
            Log.errorln (F ("-> Failed to create SD Card file."));
            SD.end ();
            spiSdCardShutDown ();
        }
        else
        {
            Log.errorln (F ("-> Failed to create LittleFS File."));
        }

        return false;
    }
    else
    {
        Log.verboseln (F ("-> Created file: %s"), fileName);
    }

    // *****************************************************************
    // Allocate a temporary JsonDocument
    // MUST set the capacity to match your requirements.
    // Use https://arduinojson.org/assistant to compute the capacity.

    DynamicJsonDocument doc (JSON_CFG_SZ);

    // *****************************************************************
    // Log.infoln ((String(F ("saveConfiguration:1 doc.capacity: ")) + String(doc.capacity())).c_str());
    JsonObject root = doc.to <JsonObject>();
    root["author"] = "MEM";

    // Log.infoln ((String(F ("saveConfiguration:1 doc.size: ")) + String(doc.size())).c_str());
    // serializeJsonPretty(root, Serial); // Debug Output
    // Serial.println();

    ControllerMgr.saveConfiguration (root);
    WiFiDriver.saveConfiguration (root);
    LoginUser.saveConfiguration (root);
    LoginPassword.saveConfiguration (root);
    Gpio19.saveConfiguration (root);
    Gpio23.saveConfiguration (root);
    Gpio33.saveConfiguration (root);
    Diagnostics.saveConfiguration (root);
    Radio.saveConfiguration (root);

    // Log.infoln ((String(F ("saveConfiguration: doc.overflowed: ")) + String(doc.overflowed())).c_str());
    // Log.infoln ((String(F ("saveConfiguration:2 doc.size: ")) + String(doc.size())).c_str());
    // Log.infoln ((String(F ("saveConfiguration:2 doc.capacity: ")) + String(doc.capacity())).c_str());
    // serializeJsonPretty(root, Serial); // Debug Output
    // Serial.println();

    // Serialize JSON to file
    if (serializeJson (root, file) == 0)
    {
        Log.errorln (F ("-> Failed to Save Configuration."));
    }
    else
    {
        successFlg = true;
        // Log.verboseln (F ("-> Configuration JSON used %u Bytes."), doc.memoryUsage ());
        Log.infoln (F ("-> Configuration Save Complete."));

        // serializeJsonPretty(doc, Serial); // Debug Output
        Serial.println ();
    }

    // Log.infoln (String(F ("saveConfiguration: 241")).c_str());

    // Close the file
    file.close ();

    if (saveMode == SD_CARD_MODE)
    {
        SD.end ();
        spiSdCardShutDown ();
    }

    doc.clear ();

    return successFlg;
}

// *************************************************************************************************************************
// restoreConfiguration(): Restore configuration from local file system (LittleFS).On exit, return true if successful.
bool restoreConfiguration (uint8_t restoreMode, const char * fileName)
{
    File file;
    SPIClass SPI2 (HSPI);

    if (restoreMode == LITTLEFS_MODE)
    {
        Log.infoln (F ("Restore Configuration From LittleFS ..."));
        PixelRadio_LittleFS.begin ();
        file = PixelRadio_LittleFS.open (fileName, FILE_READ);
    }
    else if (restoreMode == SD_CARD_MODE)
    {
        Log.infoln (F ("Restore Configuration From SD Card ..."));
        SPI2.begin (SD_CLK_PIN, MISO_PIN, MOSI_PIN, SD_CS_PIN);

        pinMode (MISO_PIN, INPUT_PULLUP);   // MISO requires internal pull-up.
        SD.end ();                          // Reset interface (in case SD card had been swapped).

        if (!SD.begin (SD_CS_PIN, SPI2))
        {
            Log.errorln (F ("-> SD Card failed Initialization, Aborted."));

            if (SD.cardType () == 0)
            {
                Log.warningln (F ("-> SD Card Missing."));
            }
            else
            {
                Log.errorln (F ("-> SD Card Unknown Error."));
            }

            SD.end ();
            spiSdCardShutDown ();

            return false;
        }

        Log.infoln (F ("-> SD Card Type: %s"), SD.cardType () < SD_TYPE_CNT ? sdTypeStr[SD.cardType ()] : "Error");
        file = SD.open (fileName, FILE_READ);
    }
    else
    {
        Log.infoln (F ("restoreConfiguration: Undefined Backup Mode, Abort."));

        return false;
    }

    if (!file)
    {
        Log.errorln (F ("-> Failed to Locate Configuration File (%s)."), fileName);
        Log.infoln (F ("-> Create the Missing File by Performing a \"Save Settings\" in the PixelRadio App."));

        if (restoreMode == SD_CARD_MODE)
        {
            SD.end ();
            spiSdCardShutDown ();
        }

        return false;
    }
    else
    {
        Log.verboseln (F ("-> Located Configuration File (%s)"), fileName);
    }

    // empirically Arduino Json needs 3.5 x the json text size to parse the file.
    uint32_t DocSize = uint32_t (file.size ()) * 4;
    // Serial.println(String("DocSize: ") + String(DocSize));

    DynamicJsonDocument raw_doc (DocSize);
    // Serial.println(String("capacity: ") + String(raw_doc.capacity()));

    DeserializationError error = deserializeJson (raw_doc, file);

    // Serial.println("PrettyPrint raw_doc");
    // serializeJsonPretty(raw_doc, Serial); // Debug Output
    // Serial.println("\nPrettyPrint raw_doc");

    JsonObject doc = raw_doc.as <JsonObject>();
    // Serial.println("PrettyPrint doc");
    // serializeJsonPretty(doc, Serial); // Debug Output
    // Serial.println("\nPrettyPrint doc");

    file.close ();

    if (restoreMode == SD_CARD_MODE)
    {
        SD.end ();
        spiSdCardShutDown ();
    }

    if (error)
    {
        Log.errorln ((String (F ("restoreConfiguration: Configure Deserialization Failed, Error: ")) + error.c_str ()).c_str ());

        return false;
    }

    ControllerMgr.restoreConfiguration (doc);
    WiFiDriver.restoreConfiguration (doc);
    Radio.restoreConfiguration (doc);
    LoginUser.restoreConfiguration (doc);
    LoginPassword.restoreConfiguration (doc);
    Gpio19.restoreConfiguration (doc);
    Gpio23.restoreConfiguration (doc);
    Gpio33.restoreConfiguration (doc);
    Diagnostics.saveConfiguration (doc);

    Log.verboseln (F ("-> Configuration JSON used %u Bytes."), doc.memoryUsage ());
    Log.infoln (F ("-> Configuration Restore Complete."));

    // serializeJsonPretty(doc, Serial); // Debug Output
    // Serial.println();

    doc.clear ();

    return true;
}
