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
#include <ESPUI.h>
#include "config.h"
#include "globals.h"
#include "language.h"
#include "PixelRadio.h"
#include "webGUI.h"
#include "memdebug.h"

// ************************************************************************************************
void backupCallback (Control * sender, int type)
{
#ifdef OldWay
        static bool saveSuccessFlg      = true;
        static bool restoreSuccessFlg   = true;
        char    logBuff[60];
        char    fileName[20];

        // sprintf(logBuff, "dhcpCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
        // Log.verboseln(logBuff);

        sprintf (fileName, "%s", BACKUP_FILE_NAME); // Create filename for backup.

        if (sender->id == backupSaveID)
        {
            if (type == B_DOWN)
            {
                ESPUI.  print ( backupSaveMsgID,    "");    // Erase Message.
                ESPUI.  print ( backupRestoreMsgID, "");    // Erase Message.
                saveSuccessFlg = saveConfiguration (SD_CARD_MODE, fileName);

                if (saveSuccessFlg)
                {
                    ESPUI.print (backupSaveMsgID, BACKUP_SAV_PASS_STR);
                    Log.infoln ("-> Backup Save Successful.");
                }
                else
                {
                    ESPUI.print (backupSaveMsgID, BACKUP_SAV_FAIL_STR);
                    Log.errorln ("-> Backup Save Failed.");
                }
            }
            else if (type == B_UP)
            {}
            else
            {
                sprintf (logBuff, "backupCallback: %s.", BAD_VALUE_STR);
                Log.errorln (logBuff);
            }
        }
        else if (sender->id == backupRestoreID)
        {
            if (type == B_DOWN)
            {
                ESPUI.  print ( backupSaveMsgID,    "");    // Erase Message.
                ESPUI.  print ( backupRestoreMsgID, "");    // Erase Message.

                restoreSuccessFlg = restoreConfiguration (SD_CARD_MODE, fileName);

                if (restoreSuccessFlg)
                {
                    restoreSuccessFlg = saveConfiguration (LITTLEFS_MODE, fileName);    // Save restored SD backup to file system.
                }

                if (restoreSuccessFlg)
                {
                    ESPUI.print (backupRestoreMsgID, BACKUP_RES_PASS_STR);
                    Log.infoln ("-> Backup Restore Successful. Reboot Required to Apply Settings.");
                }
                else
                {
                    ESPUI.print (backupRestoreMsgID, BACKUP_RES_FAIL_STR);
                    Log.errorln ("-> Backup Restore Failed.");
                }
            }
            else if (type == B_UP)
            {}
            else
            {
                sprintf (logBuff, "backupCallback: %s.", BAD_VALUE_STR);
                Log.errorln (logBuff);
            }
        }
        else
        {
            sprintf (logBuff, "backupCallback: %s.", BAD_SENDER_STR);
            Log.errorln (logBuff);
        }
#endif // def OldWay
}

// ************************************************************************************************
// diagBootCallback(): Reboot ESP32
//                     Must hold button for several seconds, will reboot upon release.
void diagBootCallback (Control * sender, int type)
{
    char logBuff[60];
    uint32_t currentMillis      = millis ();    // Snapshot of System Timer.
    static uint32_t oldMillis   = 0;

    // sprintf(logBuff, "diagBootCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == diagBootID)
    {
        switch (type)
        {
            case B_DOWN:
            {
                oldMillis   = millis ();
                tempStr     = "color: ";
                tempStr     += COLOR_RED_STR;
                tempStr     += ";";
                ESPUI.setPanelStyle (diagBootID, tempStr);  // Red Panel Name.
                ESPUI.print (diagBootMsgID, DIAG_BOOT_MSG1_STR);
                Log.infoln ("diagBootCallback: Reboot Button Pressed.");
                break;
            }

            case B_UP:
            {
                if (currentMillis > oldMillis + 5000)
                {
                    tempStr = "background-color: ";
                    tempStr += COLOR_RED_STR;
                    tempStr += ";";
                    ESPUI.setPanelStyle (diagBootID, tempStr);  // Red Panel Body
                    ESPUI.print (diagBootMsgID, DIAG_BOOT_MSG2_STR);
                    Log.warningln ("diagBootCallback: Reboot Activated. Goodbye!");
                    Serial.flush ();
                    rebootFlg = true;   // Tell Main Loop to reboot.
                }
                else
                {
                    tempStr = "color: ";
                    tempStr += COLOR_BLK_STR;
                    tempStr += ";";
                    ESPUI.setPanelStyle (diagBootID, tempStr);                      // White (default) Panel Name.
                    ESPUI.getControl (diagBootID)->color = ControlColor::Sunflower; // Restore orig panel color
                    ESPUI.updateControl (diagBootID);                               // Apply changes to control.
                    ESPUI.print (diagBootMsgID, "");
                    Log.infoln ("diagBootCallback: Reboot Button Released (Canceled).");
                }
                break;
            }

            default:
            {
                sprintf (logBuff, "diagBootCallback: %s.", BAD_VALUE_STR);
                Log.errorln (logBuff);
                break;
            }
        }   // switch
    }
    else
    {
        sprintf (logBuff, "diagBootCallback: %s.", BAD_SENDER_STR);
        Log.errorln (logBuff);
    }
}

// ************************************************************************************************
void saveSettingsCallback (Control * sender, int type)
{
#ifdef OldWay
        // DEBUG_START;

        bool    saveSuccessFlg = true;
        char    logBuff[60];
        char    fileName[sizeof (BACKUP_FILE_NAME) + 1];
        String passMsg;

        passMsg = "<p style=\"color:yellow ;\">";
        passMsg += BACKUP_SAV_PASS_STR;
        passMsg += "</p>";

        // sprintf(logBuff, "serialCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
        // Log.verboseln(logBuff);

        if ((sender->id == adjSaveID) ||
            (sender->id == ctrlSaveID) ||
            (sender->id == backupSaveSetID) ||
            (sender->id == gpioSaveID) ||
            (sender->id == radioSaveID) ||
            (sender->id == rdsSaveID) ||
            (sender->id == wifiSaveID))
        {
            // DEBUG_V();
            switch (type)
            {
                case B_DOWN:
                {
                    Log.infoln ("Saving Settings to File System ...");
                    // DEBUG_V();
                    // SAVE SETTINGS HERE
                    sprintf (fileName, "%s", BACKUP_FILE_NAME);
                    saveSuccessFlg = saveConfiguration (LITTLEFS_MODE, fileName);

                    if (saveSuccessFlg)
                    {
                        ESPUI.  print ( adjSaveMsgID,       passMsg);
                        ESPUI.  print ( backupSaveSetMsgID, passMsg);
                        ESPUI.  print ( ctrlSaveMsgID,      passMsg);
                        ESPUI.  print ( gpioSaveMsgID,      passMsg);
                        ESPUI.  print ( radioSaveMsgID,     passMsg);
                        ESPUI.  print ( rdsSaveMsgID,       passMsg);
                        ESPUI.  print ( wifiSaveMsgID,      passMsg);

                        Log.infoln ("-> Configuration Save Successful.");
                    }
                    else
                    {
                        ESPUI.  print ( adjSaveMsgID,       BACKUP_SAV_FAIL_STR);
                        ESPUI.  print ( backupSaveSetMsgID, BACKUP_SAV_FAIL_STR);
                        ESPUI.  print ( ctrlSaveMsgID,      BACKUP_SAV_FAIL_STR);
                        ESPUI.  print ( gpioSaveMsgID,      BACKUP_SAV_FAIL_STR);
                        ESPUI.  print ( radioSaveMsgID,     BACKUP_SAV_FAIL_STR);
                        ESPUI.  print ( rdsSaveMsgID,       BACKUP_SAV_FAIL_STR);
                        ESPUI.  print ( wifiSaveMsgID,      BACKUP_SAV_FAIL_STR);

                        Log.errorln ("-> Configuration Save Failed.");
                    }
                    break;
                }

                case B_UP:
                {
                    break;
                }

                default:
                {
                    sprintf (logBuff, "saveSettingsCallback: %s.", BAD_VALUE_STR);
                    Log.errorln (logBuff);
                    break;
                }
            }   // switch
        }
        else
        {
            sprintf (logBuff, "saveSettingsCallback: %s.", BAD_SENDER_STR);
            Log.errorln (logBuff);
        }
        // DEBUG_END;
#endif // def OldWay
}
