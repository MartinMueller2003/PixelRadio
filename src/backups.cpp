/*
   File: backups.cpp (System Configuration Backup and Restore)
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.


   SD Card Emergency Credential Restoration Feature:
   -------------------------------------------------
   Credential Restoration allows the User to change WiFI credentials by SD Card. Reverts to DHCP WiFi.
   (1) File Creation:
    Create file named credentials.txt and add the following JSON formatted text to it (fill in your SSID and WPA_KEY):
    {"WIFI_SSID_STR":"SSID",
    "WIFI_WPA_KEY_STR":"WPA_KEY"}
   (2) Instructions:
       Install your prepared SD Card in PixelRadio. Reboot. Wait 30 secs, Remove card.
       Note: For Security the File is automatically deleted from card.
 */

// *************************************************************************************************************************
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <SPI.h>
#include <SD.h>
#include "PixelRadio.h"
#include "globals.h"


// *************************************************************************************************************************
const uint16_t JSON_CFG_SZ    = 6500;
const uint16_t JSON_CRED_SIZE = 300;

const char* sdTypeStr[] = {"Not Installed", "V1", "V2", "SDHC", "Unknown"};
const uint8_t SD_TYPE_CNT = sizeof(sdTypeStr) / sizeof(sdTypeStr[0]);

// *************************************************************************************************************************
// checkEmergencyCredentials(): Restore credentials if credentials.txt is available. For use during boot.
//                              Return true if Emergency credentials were restored.
bool checkEmergencyCredentials(const char *fileName)
{
    bool successFlg = true;
    File file;
    SPIClass SPI2(HSPI);

    SPI2.begin(SD_CLK_PIN, MISO_PIN, MOSI_PIN, SD_CS_PIN);
    pinMode(MISO_PIN, INPUT_PULLUP); // MISO requires internal pull-up.
    SD.end();                        // Reset interface (in case SD card had been swapped).
    spiSdCardShutDown();

    if (!SD.begin(SD_CS_PIN, SPI2)) {      // SD Card Missing, nothing to do, exit.
        SD.end();
        spiSdCardShutDown();
        return false;
    }

    if (SD.exists(fileName)) { // Found Special Credential File.
        Log.infoln("Restoring WiFi Credentials From SD Card ...");
    }
    else {
        SD.end();
        spiSdCardShutDown();
        return false;
    }
    Log.infoln("-> SD Card Type: %s", SD.cardType() < SD_TYPE_CNT ? sdTypeStr[SD.cardType()] : "Error");
    file = SD.open(fileName, FILE_READ);

    DynamicJsonDocument doc(JSON_CRED_SIZE);
    DeserializationError error = deserializeJson(doc, file);

    file.close();
    SD.remove(fileName); // Erase File for security protection.
    SD.end();
    //SPI2.end();
    spiSdCardShutDown();

    if (error) {
        Log.errorln("checkEmergencyCredentials: Deserialization Failed, Error:%s.", error.c_str());
        spiSdCardShutDown();
        return false;
    }

    if (((const char *)doc["WIFI_SSID_STR"] != NULL) && ((const char *)doc["WIFI_WPA_KEY_STR"] != NULL)) {
        wifiSSIDStr   = (const char *)doc["WIFI_SSID_STR"];
        wifiWpaKeyStr = (const char *)doc["WIFI_WPA_KEY_STR"];
        wifiDhcpFlg   = true;
        successFlg    = true;
        Log.warningln("-> User Provided New WiFi Credentials.");
        Log.warningln("-> Will Use DHCP Mode on this Session.");
        Log.verboseln("-> Credentials JSON used %u Bytes.", doc.memoryUsage());
        Log.infoln("-> Credentials Restore Complete.");
        Log.warningln("-> For Your Security the Credential File Has Been Deleted.");
    }
    else {
        Log.errorln("-> Credential Restore Failed, Invalid / Incomplete File Contents.");
        Log.warningln("-> The Credential File Has NOT Been Deleted. Please Secure Your Data.");
        successFlg = false;
    }

    // serializeJsonPretty(doc, Serial); // Debug Output
    // Serial.println();

    doc.clear();

    return successFlg;
}

// *************************************************************************************************************************
//saveConfiguration(): Save the System Configuration to LittleFS or SD Card.
// SD Card Date Stamp is Jan-01-1980. Wasn't able to write actual time stamp because SDFat library conflicts with LITTLEFS.h.
bool saveConfiguration(uint8_t saveMode, const char *fileName)
{
    bool successFlg = false;
    File file;
    SPIClass SPI2(HSPI);

    if (saveMode == LITTLEFS_MODE) {
        Log.infoln("Backup Configuration to LittleFS ...");
        LittleFS.remove(fileName);
        file = LittleFS.open(fileName, FILE_WRITE);
    }
    else if (saveMode == SD_CARD_MODE) {
        Log.infoln("Backup Configuration to SD Card ...");
        SPI2.begin(SD_CLK_PIN, MISO_PIN, MOSI_PIN, SD_CS_PIN);
        pinMode(MISO_PIN, INPUT_PULLUP); // MISO requires internal pull-up.
        SD.end();                        // Re-init Interface in case SD card had been swapped).

        if (!SD.begin(SD_CS_PIN, SPI2)) {
            Log.errorln("-> SD Card failed Initialization, Aborted.");
            SD.end();
            spiSdCardShutDown();
            return false;
        }
        //SD.remove(fileName);
        Log.infoln("-> SD Card Type: %s", SD.cardType() < SD_TYPE_CNT ? sdTypeStr[SD.cardType()] : "Error");
        file = SD.open(fileName, FILE_WRITE);
    }
    else {
        Log.infoln("saveConfiguration: Undefined Backup Mode, Aborted.");
        return false;
    }


    if (!file) {
        Log.errorln("-> Failed to create file");
        if (saveMode == SD_CARD_MODE) {
            Log.errorln("-> Failed to create SD Card file.");
            SD.end();
            spiSdCardShutDown();
        }
        else {
            Log.errorln("-> Failed to create LittleFS File.");
        }
        return false;
    }
    else {
        Log.verboseln("-> Created file: %s", fileName);
    }

    // *****************************************************************
    // Allocate a temporary JsonDocument
    // MUST set the capacity to match your requirements.
    // Use https://arduinojson.org/assistant to compute the capacity.
    DynamicJsonDocument doc(JSON_CFG_SZ);
    // *****************************************************************

    doc["USER_NAME_STR"] = userNameStr;
    doc["USER_PW_STR"]   = userPassStr;

    doc["STA_NAME_STR"]     = staNameStr;
    doc["MDNS_NAME_STR"]    = mdnsNameStr;
    doc["AP_NAME_STR"]      = apNameStr;
    doc["AP_IP_ADDR_STR"]   = apIpAddrStr;
    doc["AP_FALLBACK_FLAG"] = apFallBackFlg;

    JsonObject root = doc.as<JsonObject>();
    ControllerMgr.SaveConfiguration(root);

    doc["WIFI_SSID_STR"]    = wifiSSIDStr;
    doc["WIFI_WPA_KEY_STR"] = wifiWpaKeyStr;
    doc["WIFI_IP_ADDR_STR"] = staticIpStr;
    doc["WIFI_GATEWAY_STR"] = wifiGatewayStr;
    doc["WIFI_SUBNET_STR"]  = subNetStr;
    doc["WIFI_DNS_STR"]     = wifiDnsStr;
    doc["WIFI_DHCP_FLAG"]   = wifiDhcpFlg;
    doc["WIFI_REBOOT_FLAG"] = WiFiRebootFlg;

    doc["RDS_PI_CODE"]        = ControllerMgr.GetPiCode(LocalControllerId); // Use radio.setPiCode() when restoring this hex value.
    doc["RDS_PTY_CODE"]       = ControllerMgr.GetPtyCode(LocalControllerId);
    doc["RDS_LOCAL_MSG_TIME"] = ControllerMgr.GetRdsMsgTime(LocalControllerId);
    doc["RDS_PROG_SERV_STR"]  = ControllerMgr.GetRdsProgramServiceName(LocalControllerId);
    doc["RADIO_FM_FREQ"]      = fmFreqX10;      // Use radio.setFrequency(MHZ) when restoring this uint16 value.
    doc["RADIO_MUTE_FLAG"]    = muteFlg;        // Use radio.mute(0/1) when restoring this uint8 value. 1=MuteOn
    doc["RADIO_AUTO_FLAG"]    = rfAutoFlg;      // Use radio.radioNoAudioAutoOFF(0/1) when restoring this uint8 Value.
    doc["RADIO_RF_CARR_FLAG"] = rfCarrierFlg;   // Use radio.RDS(0/1) when restoring this uint8 value. 1=CarrierOn.
    doc["RADIO_STEREO_FLAG"]  = stereoEnbFlg;   // Use radio.MonoAudio(0/1) when restoring this uint8 value. 0=Stereo.
    doc["RADIO_PRE_EMPH_STR"] = preEmphasisStr; // Use radio.setPreEmphTime50(0/1), uint8 value. Not working?
    doc["RADIO_POWER_STR"]    = rfPowerStr;     // Use radio.setTxPower(20-75) when restoring this uint8 value.

    doc["ANALOG_VOLUME"]    = analogVol;        // Requires custom function, not written yet.
    doc["ANALOG_GAIN_STR"]  = vgaGainStr;       // Use radio.setTxInputBufferGain(0-5) when restoring this Int value.
    doc["USB_VOLUME"]       = usbVol;           // Use Serial Control, "VOL=0" to "VOL=30".
    doc["DIGITAL_GAIN_STR"] = digitalGainStr;   // Use radio.setTxDigitalGain(0/1/2) when restoring this Int value.
    doc["INPUT_IMPED_STR"]  = inpImpedStr;      // Use radio.setAudioInpImp(5/10/20/40) when restoring this Int value.

	doc["GPIO19_STR"] = gpio19BootStr;
	doc["GPIO23_STR"] = gpio23BootStr;
	doc["GPIO33_STR"] = gpio33BootStr;

	doc["LOG_LEVEL_STR"] = logLevelStr;

	// Serialize JSON to file
	if (serializeJson(doc, file) == 0) {
    	Log.errorln("-> Failed to Save Configuration.");
    }
    else {
        successFlg = true;
        Log.verboseln("-> Configuration JSON used %u Bytes.", doc.memoryUsage());
        Log.infoln("-> Configuration Save Complete.");

        // serializeJsonPretty(doc, Serial); // Debug Output
        // Serial.println();
    }

    // Close the file
    file.close();
    if (saveMode == SD_CARD_MODE) {
        SD.end();
        spiSdCardShutDown();
    }

    doc.clear();

    return successFlg;
}

// *************************************************************************************************************************
// restoreConfiguration(): Restore configuration from local file system (LittleFS). On exit, return true if successful.
bool restoreConfiguration(uint8_t restoreMode, const char *fileName)
{
    File file;
    SPIClass SPI2(HSPI);

    if (restoreMode == LITTLEFS_MODE) {
        Log.infoln("Restore Configuration From LittleFS ...");
        file = LittleFS.open(fileName, FILE_READ);
    }
    else if (restoreMode == SD_CARD_MODE) {
        Log.infoln("Restore Configuration From SD Card ...");
        SPI2.begin(SD_CLK_PIN, MISO_PIN, MOSI_PIN, SD_CS_PIN);

        pinMode(MISO_PIN, INPUT_PULLUP); // MISO requires internal pull-up.
        SD.end();                        // Reset interface (in case SD card had been swapped).

        if (!SD.begin(SD_CS_PIN, SPI2)) {
            Log.errorln("-> SD Card failed Initialization, Aborted.");
            if(SD.cardType() == 0) {
                Log.warningln("-> SD Card Missing.");
            }
            else {
                Log.errorln("-> SD Card Unknown Error.");\
            }
            SD.end();
            spiSdCardShutDown();
            return false;
        }

        Log.infoln("-> SD Card Type: %s", SD.cardType() < SD_TYPE_CNT ? sdTypeStr[SD.cardType()] : "Error");
        file = SD.open(fileName, FILE_READ);
    }
    else {
        Log.infoln("restoreConfiguration: Undefined Backup Mode, Abort.");
        return false;
    }

    if (!file) {
        Log.errorln("-> Failed to Locate Configuration File (%s).", fileName);
        Log.infoln("-> Create the Missing File by Performing a \"Save Settings\" in the PixelRadio App.");
        if (restoreMode == SD_CARD_MODE) {
            SD.end();
            spiSdCardShutDown();
        }
        return false;
    }
    else {
        Log.verboseln("-> Located Configuration File (%s)", fileName);
    }

    DynamicJsonDocument raw_doc(JSON_CFG_SZ);

    DeserializationError error = deserializeJson(raw_doc, file);

    // serializeJsonPretty(doc, Serial); // Debug Output

    file.close();
    if (restoreMode == SD_CARD_MODE) {
        SD.end();
        spiSdCardShutDown();
    }

    if (error) {
        Log.errorln("restoreConfiguration: Configure Deserialization Failed, Error:%s.", error.c_str());
        return false;
    }
    JsonObject doc = raw_doc.as <JsonObject>();
    ControllerMgr.RestoreConfiguration(doc);

    if ((const char *)doc["USER_NAME_STR"] != NULL) {
        userNameStr = (const char *)doc["USER_NAME_STR"];
    }

    if ((const char *)doc["USER_PW_STR"] != NULL) {
        userPassStr = (const char *)doc["USER_PW_STR"];
    }

    if ((const char *)doc["STA_NAME_STR"] != NULL) {
        staNameStr = (const char *)doc["STA_NAME_STR"];
    }

    if ((const char *)doc["MDNS_NAME_STR"] != NULL) {
        mdnsNameStr = (const char *)doc["MDNS_NAME_STR"];
    }

    if ((const char *)doc["AP_NAME_STR"] != NULL) {
        apNameStr = (const char *)doc["AP_NAME_STR"];
    }

    if ((const char *)doc["AP_IP_ADDR_STR"] != NULL) {
        apIpAddrStr = (const char *)doc["AP_IP_ADDR_STR"];
    }

    if ((const char *)doc["WIFI_SSID_STR"] != NULL) {
        wifiSSIDStr = (const char *)doc["WIFI_SSID_STR"];
    }

    if ((const char *)doc["WIFI_WPA_KEY_STR"] != NULL) {
        wifiWpaKeyStr = (const char *)doc["WIFI_WPA_KEY_STR"];
    }

    if ((const char *)doc["WIFI_IP_ADDR_STR"] != NULL) {
        staticIpStr = (const char *)doc["WIFI_IP_ADDR_STR"];
    }

    if ((const char *)doc["WIFI_GATEWAY_STR"] != NULL) {
        wifiGatewayStr = (const char *)doc["WIFI_GATEWAY_STR"];
    }

    if ((const char *)doc["WIFI_SUBNET_STR"] != NULL) {
        subNetStr = (const char *)doc["WIFI_SUBNET_STR"];
    }

    if ((const char *)doc["WIFI_DNS_STR"] != NULL) {
        wifiDnsStr = (const char *)doc["WIFI_DNS_STR"];
    }

//    if ((const char *)doc["RDS_PROG_SERV_STR"] != NULL) {
//        String Temp = doc["RDS_PROG_SERV_STR"];
//        ControllerMgr.SetRdsProgramServiceName(LocalControllerId, Temp);
//    }

    hotSpotIP.fromString(apIpAddrStr);
    staticIP.fromString(staticIpStr);
    subNet.fromString(subNetStr);
    wifiDNS.fromString(wifiDnsStr);
    wifiGateway.fromString(wifiGatewayStr);


    if (doc.containsKey("AP_FALLBACK_FLAG")) {
        apFallBackFlg = doc["AP_FALLBACK_FLAG"];
    }

    if (doc.containsKey("WIFI_DHCP_FLAG")) {
        wifiDhcpFlg = doc["WIFI_DHCP_FLAG"];
    }

    if (doc.containsKey("WIFI_REBOOT_FLAG")) {
        WiFiRebootFlg = doc["WIFI_REBOOT_FLAG"];
    }

    if (doc.containsKey("RDS_PI_CODE")) {
        ControllerMgr.SetPiCode(LocalControllerId, doc["RDS_PI_CODE"]); // Use radio.setPiCode() when restoring this hex value.
    }

    if (doc.containsKey("RDS_PTY_CODE")) {
        ControllerMgr.SetPtyCode(LocalControllerId, doc["RDS_PTY_CODE"]);
    }
/*
    if (doc.containsKey("RDS_LOCAL_MSG_TIME")) {
        ControllerMgr.SetRdsMsgTime(LocalControllerId, doc["RDS_LOCAL_MSG_TIME"]);
    }
    if (doc.containsKey("RADIO_FM_FREQ")) {
        fmFreqX10 = doc["RADIO_FM_FREQ"]; // Use radio.setFrequency(MHZ) when restoring this float value.
    }

    if (doc.containsKey("RADIO_MUTE_FLAG")) {
        muteFlg = doc["RADIO_MUTE_FLAG"]; // Use radio.mute(0/1) when restoring this uint8 value. 1=MuteOn
    }

    if (doc.containsKey("RADIO_AUTO_FLAG")) {
        rfAutoFlg = doc["RADIO_AUTO_FLAG"]; // Use radio.radioNoAudioAutoOFF(0/1) when restoring this uint8 Value.
    }
*/
    if (doc.containsKey("RADIO_RF_CARR_FLAG")) {
        rfCarrierFlg = doc["RADIO_RF_CARR_FLAG"]; // Use radio.RDS(0/1) when restoring this uint8 value. 1=CarrierOn.
    }

    if (doc.containsKey("RADIO_STEREO_FLAG")) {
        stereoEnbFlg = doc["RADIO_STEREO_FLAG"]; // Use radio.MonoAudio(0/1) when restoring this uint8 value. 0=Stereo.
    }

    if ((const char *)doc["RADIO_PRE_EMPH_STR"] != NULL) {
        preEmphasisStr = (const char *)doc["RADIO_PRE_EMPH_STR"]; // Use radio.setPreEmphTime50(0/1), uint8 value. Not working?
    }

    if ((const char *)doc["RADIO_POWER_STR"] != NULL) {
        rfPowerStr = (const char *)doc["RADIO_POWER_STR"]; // Use radio.setTxPower(20-75) when restoring this uint8 value.
    }

    if (doc.containsKey("ANALOG_VOLUME")) {
        analogVol = doc["ANALOG_VOLUME"]; // Requires custom function, not written yet.
    }

    if (doc.containsKey("USB_VOLUME")) {
        usbVol = doc["USB_VOLUME"]; // Use Serial Control, "VOL=0" to "VOL=30".
    }

    if ((const char *)doc["ANALOG_GAIN_STR"] != NULL) {
        vgaGainStr = (const char *)doc["ANALOG_GAIN_STR"]; // Use radio.setTxInputBufferGain(0-5) when restoring this Int value.
    }

    if ((const char *)doc["DIGITAL_GAIN_STR"] != NULL) {
        digitalGainStr = (const char *)doc["DIGITAL_GAIN_STR"]; // Use radio.setTxDigitalGain(0/1/2) when restoring this Int value.
    }

    if ((const char *)doc["INPUT_IMPED_STR"] != NULL) {
        inpImpedStr = (const char *)doc["INPUT_IMPED_STR"]; // Use radio.setAudioInpImp(5/10/20/40) when restoring this Int value.
    }

    if ((const char *)doc["GPIO19_STR"] != NULL) {
        gpio19BootStr = (const char *)doc["GPIO19_STR"];
    }

    if ((const char *)doc["GPIO23_STR"] != NULL) {
        gpio23BootStr = (const char *)doc["GPIO23_STR"];
    }

    if ((const char *)doc["GPIO33_STR"] != NULL) {
        gpio33BootStr = (const char *)doc["GPIO33_STR"];
    }

    if ((const char *)doc["LOG_LEVEL_STR"] != NULL) {
        logLevelStr = (const char *)doc["LOG_LEVEL_STR"];
    }

    Log.verboseln("-> Configuration JSON used %u Bytes.", doc.memoryUsage());
    Log.infoln("-> Configuration Restore Complete.");

    // serializeJsonPretty(doc, Serial); // Debug Output
    // Serial.println();

    doc.clear();

    return true;
}
