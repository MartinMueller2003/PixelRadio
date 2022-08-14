/*
   File: callBacks.cpp (ESPUI Web Page Callback Functions)
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

#include <ArduinoLog.h>
#include "PixelRadio.h"
#include "config.h"
#include "ESPUI.h"
#include "globals.h"
#include "language.h"
#include "webGUI.h"
#include "Controllers/ControllerUsbSERIAL.h"

#if __has_include("memdebug.h")
#include "memdebug.h"
#endif //  __has_include("memdebug.h")

// ************************************************************************************************
void backupCallback(Control *sender, int type)
{
#ifdef OldWay
    static bool saveSuccessFlg    = true;
    static bool restoreSuccessFlg = true;
    char logBuff[60];
    char fileName[20];

    // sprintf(logBuff, "dhcpCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    sprintf(fileName, "%s", BACKUP_FILE_NAME); // Create filename for backup.

    if (sender->id == backupSaveID) {
        if (type == B_DOWN) {
            ESPUI.print(backupSaveMsgID,    ""); // Erase Message.
            ESPUI.print(backupRestoreMsgID, ""); // Erase Message.
            saveSuccessFlg = saveConfiguration(SD_CARD_MODE, fileName);

            if (saveSuccessFlg) {
                ESPUI.print(backupSaveMsgID, BACKUP_SAV_PASS_STR);
                Log.infoln("-> Backup Save Successful.");
            }
            else {
                ESPUI.print(backupSaveMsgID, BACKUP_SAV_FAIL_STR);
                Log.errorln("-> Backup Save Failed.");
            }
        }
        else if (type == B_UP) {}
        else {
            sprintf(logBuff, "backupCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
    }
    else if (sender->id == backupRestoreID) {
        if (type == B_DOWN) {
            ESPUI.print(backupSaveMsgID,    ""); // Erase Message.
            ESPUI.print(backupRestoreMsgID, ""); // Erase Message.

            restoreSuccessFlg = restoreConfiguration(SD_CARD_MODE, fileName);

            if (restoreSuccessFlg) {
                restoreSuccessFlg = saveConfiguration(LITTLEFS_MODE, fileName); // Save restored SD backup to file system.
            }

            if (restoreSuccessFlg) {
                ESPUI.print(backupRestoreMsgID, BACKUP_RES_PASS_STR);
                Log.infoln("-> Backup Restore Successful. Reboot Required to Apply Settings.");
            }
            else {
                ESPUI.print(backupRestoreMsgID, BACKUP_RES_FAIL_STR);
                Log.errorln("-> Backup Restore Failed.");
            }
        }
        else if (type == B_UP) {}
        else {
            sprintf(logBuff, "backupCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
    }
    else {
        sprintf(logBuff, "backupCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
#endif // def OldWay
}

// ************************************************************************************************
// diagBootCallback(): Reboot ESP32
//                     Must hold button for several seconds, will reboot upon release.
void diagBootCallback(Control *sender, int type)
{
    char logBuff[60];
    uint32_t currentMillis    = millis(); // Snapshot of System Timer.
    static uint32_t oldMillis = 0;

    // sprintf(logBuff, "diagBootCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == diagBootID) {
        switch (type) {
          case B_DOWN:
              oldMillis = millis();
              tempStr   = "color: ";
              tempStr  += COLOR_RED_STR;
              tempStr  += ";";
              ESPUI.setPanelStyle(diagBootID, tempStr); // Red Panel Name.
              ESPUI.print(diagBootMsgID, DIAG_BOOT_MSG1_STR);
              Log.infoln("diagBootCallback: Reboot Button Pressed.");
              break;
          case B_UP:

              if (currentMillis > oldMillis + 5000) {
                  tempStr  = "background-color: ";
                  tempStr += COLOR_RED_STR;
                  tempStr += ";";
                  ESPUI.setPanelStyle(diagBootID, tempStr); // Red Panel Body
                  ESPUI.print(diagBootMsgID, DIAG_BOOT_MSG2_STR);
                  Log.warningln("diagBootCallback: Reboot Activated. Goodbye!");
                  Serial.flush();
                  rebootFlg = true; // Tell Main Loop to reboot.
              }
              else {
                  tempStr  = "color: ";
                  tempStr += COLOR_BLK_STR;
                  tempStr += ";";
                  ESPUI.setPanelStyle(diagBootID, tempStr);                      // White (default) Panel Name.
                  ESPUI.getControl(diagBootID)->color = ControlColor::Sunflower; // Restore orig panel color
                  ESPUI.updateControl(diagBootID);                               // Apply changes to control.
                  ESPUI.print(diagBootMsgID, "");
                  Log.infoln("diagBootCallback: Reboot Button Released (Canceled).");
              }
              break;
          default:
              sprintf(logBuff, "diagBootCallback: %s.", BAD_VALUE_STR);
              Log.errorln(logBuff);
              break;
        }
    }
    else {
        sprintf(logBuff, "diagBootCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// diagLogCallback() Set the Serial Log Level. Used for system debugging.
void diagLogCallback(Control *sender, int type)
{
    char   logBuff[60];
    String serialMsg;

    // sprintf(logBuff, "diagLogCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == diagLogID) {
        if (sender->value == DIAG_LOG_SILENT_STR) {
            // Log.begin(LOG_LEVEL_SILENT, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_SILENT_STR;
            tempStr     = logLevelStr;
            serialMsg   = ""; // Erase warning message on Serial Controller Panel (on ctrlTab).
        }
        else if (sender->value == DIAG_LOG_FATAL_STR) {
            // Log.begin(LOG_LEVEL_FATAL, &Serial);
            // Log.setShowLevel(false);           // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_FATAL_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR; // Warning message on Serial Controller Panel (on ctrlTab).
        }
        else if (sender->value == DIAG_LOG_ERROR_STR) {
            // Log.begin(LOG_LEVEL_ERROR, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_ERROR_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else if (sender->value == DIAG_LOG_WARN_STR) {
            // Log.begin(LOG_LEVEL_WARNING, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_WARN_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else if (sender->value == DIAG_LOG_INFO_STR) {
            // Log.begin(LOG_LEVEL_INFO, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_INFO_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else if (sender->value == DIAG_LOG_TRACE_STR) {
            // Log.begin(LOG_LEVEL_TRACE, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_TRACE_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else if (sender->value == DIAG_LOG_VERB_STR) {
            // Log.begin(LOG_LEVEL_VERBOSE, &Serial);
            // Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
            logLevelStr = DIAG_LOG_VERB_STR;
            tempStr     = logLevelStr;
            serialMsg   = CTLR_SERIAL_MSG_STR;
        }
        else {
            tempStr   = BAD_VALUE_STR;
            serialMsg = ERROR_MSG_STR;
            sprintf(logBuff, "diagLogCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        initSerialLog(false); // Set new log level.
        uint16_t ctrlSerialMsgID = static_cast<c_ControllerUsbSERIAL *>(ControllerMgr.GetControllerById(UsbSerialControllerId))->GetMsgId();
        ESPUI.print(ctrlSerialMsgID, serialMsg);
        displaySaveWarning();
        sprintf(logBuff, "Serial Log Set to: %s.", tempStr.c_str());

        // Log.warningln(logBuff);
        tempStr  = "--:--:--:--- {SERIAL } ";
        tempStr += logBuff;
        Serial.println(tempStr); // Faux log message.
    }
    else {
        sprintf(logBuff, "diagLogCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// gpioCallback(): GPIO Pin Configuration
void gpioCallback(Control *sender, int type)
{
    char logBuff[60];
    uint8_t gpioPin = 0;

    if (sender->id == gpio19ID) {
        newGpio19Flg = true; // Tell Main Loop we Have New Setting to Update.
        gpioPin      = 19;

        if (sender->value == GPIO_INP_FT_STR) {
            gpio19BootStr = GPIO_INP_FT_STR;
            tempStr       = GPIO_INP_FT_STR;
        }
        else if (sender->value == GPIO_INP_PD_STR) {
            gpio19BootStr = GPIO_INP_PD_STR;
            tempStr       = GPIO_INP_PD_STR;
        }
        else if (sender->value == GPIO_INP_PU_STR) {
            gpio19BootStr = GPIO_INP_PU_STR;
            tempStr       = GPIO_INP_PU_STR;
        }
        else if (sender->value == GPIO_OUT_LO_STR) {
            gpio19BootStr = GPIO_OUT_LO_STR;
            tempStr       = GPIO_OUT_LO_STR;
        }
        else if (sender->value == GPIO_OUT_HI_STR) {
            gpio19BootStr = GPIO_OUT_HI_STR;
            tempStr       = GPIO_OUT_HI_STR;
        }
        else {
            gpio19BootStr = GPIO_OUT_HI_STR;
            tempStr       = "UNKNOWN";
        }
        ESPUI.print(gpio19MsgID, " "); // Erase GPIO Message Posted by any Controller.
    }
    else if (sender->id == gpio23ID) {
        newGpio23Flg = true;           // Tell Main Loop we Have New Setting to Update.
        gpioPin      = 23;

        if (sender->value == GPIO_INP_FT_STR) {
            gpio23BootStr = GPIO_INP_FT_STR;
            tempStr       = GPIO_INP_FT_STR;
        }
        else if (sender->value == GPIO_INP_PD_STR) {
            gpio23BootStr = GPIO_INP_PD_STR;
            tempStr       = GPIO_INP_PD_STR;
        }
        else if (sender->value == GPIO_INP_PU_STR) {
            gpio23BootStr = GPIO_INP_PU_STR;
            tempStr       = GPIO_INP_PU_STR;
        }
        else if (sender->value == GPIO_OUT_LO_STR) {
            gpio23BootStr = GPIO_OUT_LO_STR;
            tempStr       = GPIO_OUT_LO_STR;
        }
        else if (sender->value == GPIO_OUT_HI_STR) {
            gpio23BootStr = GPIO_OUT_HI_STR;
            tempStr       = GPIO_OUT_HI_STR;
        }
        else {
            gpio23BootStr = GPIO_OUT_HI_STR;
            tempStr       = "UNKNOWN";
        }
        ESPUI.print(gpio23MsgID, " "); // Erase GPIO Message Posted by any Controller.
    }
    else if (sender->id == gpio33ID) {
        newGpio33Flg = true;           // Tell Main Loop we Have New Setting to Update.
        gpioPin      = 33;

        if (sender->value == GPIO_INP_FT_STR) {
            gpio33BootStr = GPIO_INP_FT_STR;
            tempStr       = GPIO_INP_FT_STR;
        }
        else if (sender->value == GPIO_INP_PD_STR) {
            gpio33BootStr = GPIO_INP_PD_STR;
            tempStr       = GPIO_INP_PD_STR;
        }
        else if (sender->value == GPIO_INP_PU_STR) {
            gpio33BootStr = GPIO_INP_PU_STR;
            tempStr       = GPIO_INP_PU_STR;
        }
        else if (sender->value == GPIO_OUT_LO_STR) {
            gpio33BootStr = GPIO_OUT_LO_STR;
            tempStr       = GPIO_OUT_LO_STR;
        }
        else if (sender->value == GPIO_OUT_HI_STR) {
            gpio33BootStr = GPIO_OUT_HI_STR;
            tempStr       = GPIO_OUT_HI_STR;
        }
        else {
            gpio33BootStr = GPIO_OUT_HI_STR;
            tempStr       = "UNKNOWN";
        }
        ESPUI.print(gpio33MsgID, " "); // Erase GPIO Message Posted by any Controller.
    }
    else {
        sprintf(logBuff, "gpioCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
        return;
    }

    if (gpioPin != 0) {
        setGpioBootPins(); // Update the GPIO Pins with New Pin Configuration.
        displaySaveWarning();
        sprintf(logBuff, "GPIO-%d Set to: %s.", gpioPin, tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        tempStr = BAD_VALUE_STR;
        sprintf(logBuff, "gpioCallback: %s.", BAD_VALUE_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
void saveSettingsCallback(Control *sender, int type)
{
#ifdef OldWay
    // DEBUG_START;

    bool   saveSuccessFlg = true;
    char   logBuff[60];
    char   fileName[sizeof(BACKUP_FILE_NAME) + 1];
    String passMsg;

    passMsg  = "<p style=\"color:yellow ;\">";
    passMsg += BACKUP_SAV_PASS_STR;
    passMsg += "</p>";

    // sprintf(logBuff, "serialCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if ((sender->id == adjSaveID)       ||
        (sender->id == ctrlSaveID)      ||
        (sender->id == backupSaveSetID) ||
        (sender->id == gpioSaveID)      ||
        (sender->id == radioSaveID)     ||
        (sender->id == rdsSaveID)       ||
        (sender->id == wifiSaveID)) 
        {
        // DEBUG_V();
        switch (type) {
          case B_DOWN:
              Log.infoln("Saving Settings to File System ...");
              // DEBUG_V();
              // SAVE SETTINGS HERE
              sprintf(fileName, "%s", BACKUP_FILE_NAME);
              saveSuccessFlg = saveConfiguration(LITTLEFS_MODE, fileName);

              if (saveSuccessFlg) {
                  ESPUI.print(adjSaveMsgID,       passMsg);
                  ESPUI.print(backupSaveSetMsgID, passMsg);
                  ESPUI.print(ctrlSaveMsgID,      passMsg);
                  ESPUI.print(gpioSaveMsgID,      passMsg);
                  ESPUI.print(radioSaveMsgID,     passMsg);
                  ESPUI.print(rdsSaveMsgID,       passMsg);
                  ESPUI.print(wifiSaveMsgID,      passMsg);

                  Log.infoln("-> Configuration Save Successful.");
              }
              else {
                  ESPUI.print(adjSaveMsgID,       BACKUP_SAV_FAIL_STR);
                  ESPUI.print(backupSaveSetMsgID, BACKUP_SAV_FAIL_STR);
                  ESPUI.print(ctrlSaveMsgID,      BACKUP_SAV_FAIL_STR);
                  ESPUI.print(gpioSaveMsgID,      BACKUP_SAV_FAIL_STR);
                  ESPUI.print(radioSaveMsgID,     BACKUP_SAV_FAIL_STR);
                  ESPUI.print(rdsSaveMsgID,       BACKUP_SAV_FAIL_STR);
                  ESPUI.print(wifiSaveMsgID,      BACKUP_SAV_FAIL_STR);

                  Log.errorln("-> Configuration Save Failed.");
              }
              break;
          case B_UP:
              break;
          default:
              sprintf(logBuff, "saveSettingsCallback: %s.", BAD_VALUE_STR);
              Log.errorln(logBuff);
              break;
        }
    }
    else {
        sprintf(logBuff, "saveSettingsCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }

    // DEBUG_END;
#endif // def OldWay
}
