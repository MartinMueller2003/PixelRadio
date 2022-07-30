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
#include "Controllers/ControllerSERIAL.h"

#if __has_include("memdebug.h")
#include "memdebug.h"
#endif //  __has_include("memdebug.h")

// ************************************************************************************************
void adjFmFreqCallback(Control *sender, int type) {
    char logBuff[60];

    // sprintf(logBuff, "adjFmFreqCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == adjFreqID) {
        if (type == P_LEFT_DOWN) {       // Decr
            fmFreqX10 -= FM_FREQ_SKP_KHZ;
        }
        else if (type == P_RIGHT_DOWN) { // Incr
            fmFreqX10 += FM_FREQ_SKP_KHZ;
        }
        else if (type == P_BACK_DOWN) {  // Decr
            fmFreqX10 -= FM_FREQ_SKP_MHZ;
        }
        else if (type == P_FOR_DOWN) {   // Incr
            fmFreqX10 += FM_FREQ_SKP_MHZ;
        }
        else if ((type == P_LEFT_UP) || (type == P_RIGHT_UP) || (type == P_BACK_UP) || (type == P_FOR_UP)) {
            Log.verboseln("FM Frequency Pad Button Released.");
            return;
        }
        else {
            sprintf(logBuff, "adjFmFreqCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
            return;
        }

        if (fmFreqX10 > FM_FREQ_MAX_X10) {
            fmFreqX10 = FM_FREQ_MAX_X10;
        }
        else if (fmFreqX10 < FM_FREQ_MIN_X10) {
            fmFreqX10 = FM_FREQ_MIN_X10;
        }

        newFreqFlg = true; // Tell main loop we have a new setting to update.
        float tempFloat = float(fmFreqX10) / 10.0f;
        tempStr  = String(tempFloat, 1);
        tempStr += UNITS_MHZ_STR;

        updateUiFrequency(true);
        displaySaveWarning();
        sprintf(logBuff, "FM Frequency Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "adjFmFreqCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// audioCallback(): Update the Stereo / Mono Audio modes.
void audioCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "audioCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioAudioID) {
        switch (type) {
          case S_ACTIVE:
              stereoEnbFlg = true;
              tempStr      = "Stereo";

              // ESPUI.print(radioAudioMsgID, RADIO_STEREO_STR);
              updateUiAudioMode(true);
              break;
          case S_INACTIVE:
              stereoEnbFlg = false;
              tempStr      = "Mono";

              // ESPUI.print(radioAudioMsgID, RADIO_MONO_STR);
              updateUiAudioMode(false);
              break;
          default:
              tempStr = BAD_VALUE_STR;
              break;
        }
        newAudioModeFlg = true; // Tell Main Loop we Have new Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "Radio Audio Mode Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "audioCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
void backupCallback(Control *sender, int type)
{
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
        uint16_t ctrlSerialMsgID = static_cast<c_ControllerSERIAL *>(ControllerMgr.GetControllerById(SerialControllerId))->GetMsgId();
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
// gainAdjust(): Adjust Digital (USB) or Analog Input Gain. Sets String.
void gainAdjustCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "gainAdjustCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioDgainID) {
        if (sender->value == DIG_GAIN0_STR) {
            digitalGainStr = DIG_GAIN0_STR;
            tempStr        = DIG_GAIN0_STR;
        }
        else if (sender->value == DIG_GAIN1_STR) {
            digitalGainStr = DIG_GAIN1_STR;
            tempStr        = DIG_GAIN1_STR;
        }
        else if (sender->value == DIG_GAIN2_STR) {
            digitalGainStr = DIG_GAIN2_STR;
            tempStr        = DIG_GAIN2_STR;
        }
        else {
            tempStr = BAD_VALUE_STR;
        }
        newDigGainFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "Digital Gain Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else if (sender->id == radioVgaGainID) {
        if (sender->value == VGA_GAIN0_STR) {
            vgaGainStr = VGA_GAIN0_STR;
            tempStr    = VGA_GAIN0_STR;
        }
        else if (sender->value == VGA_GAIN1_STR) {
            vgaGainStr = VGA_GAIN1_STR;
            tempStr    = VGA_GAIN1_STR;
        }
        else if (sender->value == VGA_GAIN2_STR) {
            vgaGainStr = VGA_GAIN2_STR;
            tempStr    = VGA_GAIN2_STR;
        }
        else if (sender->value == VGA_GAIN3_STR) {
            vgaGainStr = VGA_GAIN3_STR;
            tempStr    = VGA_GAIN3_STR;
        }
        else if (sender->value == VGA_GAIN4_STR) {
            vgaGainStr = VGA_GAIN4_STR;
            tempStr    = VGA_GAIN4_STR;
        }
        else if (sender->value == VGA_GAIN5_STR) {
            vgaGainStr = VGA_GAIN5_STR;
            tempStr    = VGA_GAIN5_STR;
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "gainAdjust: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }

        newVgaGainFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();

        tempStr  = getAudioGain();
        tempStr += " dB";
        ESPUI.print(radioGainID, tempStr);

        sprintf(logBuff, "Analog Input Gain Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "gainAdjust: %s.", BAD_SENDER_STR);
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
// impAdjustCallback(): Adjust Audio Input Impedance. Sets String.
void impAdjustCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "impAdjustCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioImpID) {
        if (sender->value == INP_IMP05K_STR) {
            inpImpedStr = INP_IMP05K_STR;
            tempStr     = INP_IMP05K_STR;
        }
        else if (sender->value == INP_IMP10K_STR) {
            inpImpedStr = INP_IMP10K_STR;
            tempStr     = INP_IMP10K_STR;
        }
        else if (sender->value == INP_IMP20K_STR) {
            inpImpedStr = INP_IMP20K_STR;
            tempStr     = INP_IMP20K_STR;
        }
        else if (sender->value == INP_IMP40K_STR) {
            inpImpedStr = INP_IMP40K_STR;
            tempStr     = INP_IMP40K_STR;
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "impAdjust: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        newInpImpFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();

        tempStr  = getAudioGain();
        tempStr += " dB";
        ESPUI.print(radioGainID, tempStr);

        sprintf(logBuff, "Input Impedance Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "impAdjustCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// muteCallback(): Turn audio on/off (true = Mute the audio).
void muteCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "muteCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == adjMuteID) {
        if (type == S_ACTIVE) {
            ESPUI.setElementStyle(adjMuteID, "background: red;");
            muteFlg = true;
            tempStr = "On";
        }
        else if (type == S_INACTIVE) {
            ESPUI.setElementStyle(adjMuteID, "background: #bebebe;");
            muteFlg = false;
            tempStr = "Off";
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "muteCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        newMuteFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "Audio Mute Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "muteCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// radioEmphasisCallback(): Set Radio pre-emphasis to North America or European. Sets String.
void radioEmphasisCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "radioEmphasisCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioEmphID) {
        if (sender->value == PRE_EMPH_USA_STR) {
            preEmphasisStr = PRE_EMPH_USA_STR;
            tempStr        = PRE_EMPH_USA_STR;
        }
        else if (sender->value == PRE_EMPH_EUR_STR) {
            preEmphasisStr = PRE_EMPH_EUR_STR;
            tempStr        = PRE_EMPH_EUR_STR;
        }
        else {
            // preEmphasisStr = PRE_EMPH_DEF_STR;
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "radioEmphasisCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        newPreEmphFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "Pre-Emphasis Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "radioEmphasisCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// rdsDisplayTimeCallback(): Allow user to change the Local RDS RadioText display time.
void rdsDisplayTimeCallback(Control *sender, int type)
{
    uint16_t timerVal = 0;

    // char logBuff[60];
    // sprintf(logBuff, "rdsDisplayTimeCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    timerVal = sender->value.toInt();

    if (timerVal < RDS_DSP_TM_MIN) {
        timerVal = RDS_DSP_TM_MIN;
        ESPUI.print(rdsDspTmID, String(timerVal));
    }
    else if (timerVal > RDS_DSP_TM_MAX) {
        timerVal = RDS_DSP_TM_MAX;
        ESPUI.print(rdsDspTmID, String(timerVal));
    }
    displaySaveWarning();
    Log.infoln("Local RDS Message Time Set to: %u.", timerVal);
    // ControllerMgr.SetRdsMsgTime(LocalControllerId, uint32_t(timerVal) * 1000); // Convert Secs to mSecs.
}

// ************************************************************************************************
// rdsRstCallback(): Reset all RDS Fields to default values.
void rdsRstCallback(Control *sender, int type)
{
    Log.infoln("RDS Settings Have Been Reset to Default Values.");
}

// ************************************************************************************************
// rdsTextCallback(): RDS RadioText Editing Handler. Provides Lock state (prevents edits) if RDS field is currently
// Enabled.
//                    Currently there are three RDS RadioTexts for local use.
void rdsTextCallback(Control *sender, int type)
{
    char logBuff[RDS_TEXT_MAX_SZ + 30];

    // sprintf(logBuff, "rdsTextCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == rdsProgNameID) {
        String rdsLocalPsnStr = sender->value;
        rdsLocalPsnStr.trim();

        if (rdsLocalPsnStr.length() > RDS_PSN_MAX_SZ) {
            rdsLocalPsnStr = rdsLocalPsnStr.substring(0, RDS_PSN_MAX_SZ);
        }
        ControllerMgr.SetRdsProgramServiceName(LocalControllerId, rdsLocalPsnStr); // Program Service Name.
        ESPUI.print(rdsProgNameID, rdsLocalPsnStr);
        displaySaveWarning();
        sprintf(logBuff, "RDS Program Identification Set to: %s.", rdsLocalPsnStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rdsTextCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// rfAutoOff(): Control Sound Activated RF Carrier Shutdown Feature. Sets Boolean.

/*
   void rfAutoCallback(Control *sender, int type)
   {
    char logBuff[70];

    // sprintf(logBuff, "rdAutoCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioAutoID) {
        if (sender->value == RF_AUTO_DIS_STR) {
            rfAutoFlg = false;
            tempStr   = RF_AUTO_DIS_STR;
        }
        else if (sender->value == RF_AUTO_ENB_STR) {
            rfAutoFlg = true;
            tempStr   = RF_AUTO_ENB_STR;
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "rfAutoOff: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        newAutoRfFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "RF Carrier Auto Shutdown Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rfAutoOff: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
   }
 */

// ************************************************************************************************
// rfCarrierCallback(): Controls RF Carrier, On/Off.
void rfCarrierCallback(Control *sender, int type)
{
    char logBuff[70];

    // sprintf(logBuff, "rfCarrierCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioRfEnbID) {
        switch (type) {
          case S_ACTIVE:
              rfCarrierFlg = true;

              if (fmRadioTestCode == FM_TEST_FAIL) {
                  Log.errorln("rfCarrierCallback: RADIO MODULE FAILURE!");
                  tempStr = "On, Warning: Radio Module Failure";
                  ESPUI.print(homeOnAirID, RADIO_FAIL_STR); // Update homeTab panel.
              }
              else if (fmRadioTestCode == FM_TEST_VSWR) {
                  Log.errorln("rfCarrierCallback: RADIO MODULE HAS HIGH VSWR!");
                  tempStr = "On, Warning: Radio Module RF-Out has High VSWR";
                  ESPUI.print(homeOnAirID, RADIO_VSWR_STR); // Update homeTab panel.
              }
              else if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX)) {
                  Log.errorln("rfCarrierCallback: RF PA HAS INCORRECT VOLTAGE!");
                  tempStr = "On, Warning: RF PA has Incorrect Voltage";
                  ESPUI.print(homeOnAirID, RADIO_VOLT_STR); // Update homeTab panel.
              }
              else {
                  tempStr = "On";
                  ESPUI.print(homeOnAirID, RADIO_ON_AIR_STR); // Update homeTab panel.
              }
              break;
          case S_INACTIVE:
              rfCarrierFlg = false;
              tempStr      = "Off";
              ESPUI.print(homeOnAirID, RADIO_OFF_AIR_STR); // Update homeTab panel.
              break;
          default:
              tempStr = BAD_VALUE_STR;
              sprintf(logBuff, "rfCarrierCallback: %s.", BAD_VALUE_STR);
              Log.errorln(logBuff);
              break;
        }
        newCarrierFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "RF Carrier Enable Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rfCarrierCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// rfPower(): Sets RF Power.
void rfPowerCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "rfPowerCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == radioPwrID) {
        if (sender->value == RF_PWR_LOW_STR) {
            rfPowerStr = RF_PWR_LOW_STR;
            tempStr    = RF_PWR_LOW_STR;
        }
        else if (sender->value == RF_PWR_MED_STR) {
            rfPowerStr = RF_PWR_MED_STR;
            tempStr    = RF_PWR_MED_STR;
        }
        else if (sender->value == RF_PWR_HIGH_STR) {
            rfPowerStr = RF_PWR_HIGH_STR;
            tempStr    = RF_PWR_HIGH_STR;
        }
        else {
            sprintf(logBuff, "rfPower: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
            tempStr = BAD_VALUE_STR;
        }
        newRfPowerFlg = true; // Tell Main Loop we Have New Setting to Update.
        displaySaveWarning();
        sprintf(logBuff, "RF Power Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "rfPower: %s.", BAD_SENDER_STR);
        Log.infoln(logBuff);
    }
}

// ************************************************************************************************
void saveSettingsCallback(Control *sender, int type)
{
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
}

// ************************************************************************************************
void setPiCodeCallback(Control *sender, int type)
{
    char logBuff[50];
    char piBuff[10];
    uint32_t tempPiCode = 0;
    String   piStr;

    piStr.reserve(20);

    // sprintf(logBuff, "setPiCodeCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == rdsPiID) {
        piStr = sender->value;
        piStr.trim();

        if (piStr.length() == 0) {                 // Missing Data Entry.
            tempPiCode = RDS_PI_CODE_DEF;          // Use default PI Code.
        }
        else if (piStr.length() > CMD_PI_MAX_SZ) { // Improperly Formatted Entry.
            tempPiCode = ControllerMgr.GetPiCode(LocalControllerId); // Bad value, re-use old PI Code.
        }
        else {
            tempPiCode = strtol(piStr.c_str(), NULL, HEX);
        }

        if ((tempPiCode < RDS_PI_CODE_MIN) || (tempPiCode > RDS_PI_CODE_MAX)) { // Value Out of Range.
            tempPiCode = ControllerMgr.GetPiCode(LocalControllerId); // Bad value, re-use old PI Code.
        }

        ControllerMgr.SetPiCode(LocalControllerId, tempPiCode);
        sprintf(piBuff, "0x%04X", tempPiCode);
        ESPUI.print(rdsPiID, piBuff);
        displaySaveWarning();

        sprintf(logBuff, "RDS PI Code Set to: \"%s\"", piBuff);
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "setPiCodeCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************

void setPtyCodeCallback(Control *sender, int type)
{
    char logBuff[50];
    char ptyBuff[10];
    int16_t tempPtyCode = 0;
    String  ptyStr;

    ptyStr.reserve(10);

    // sprintf(logBuff, "setPtyCodeCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == rdsPtyID) {
        ptyStr = sender->value;
        ptyStr.trim();
        tempPtyCode = ptyStr.toInt();

        if ((tempPtyCode < RDS_PTY_CODE_MIN) ||
            (tempPtyCode > RDS_PTY_CODE_MAX) ||
            (ptyStr.length() == 0)) {
            tempPtyCode = ControllerMgr.GetPtyCode(LocalControllerId); // Error, Use old value.
            sprintf(ptyBuff, "%0u", tempPtyCode);
            ESPUI.print(rdsPtyID, ptyBuff);
            sprintf(logBuff, "setPtyCodeCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
            return;
        }

        ControllerMgr.SetPtyCode(LocalControllerId, tempPtyCode);
        displaySaveWarning();

        sprintf(logBuff, "RDS PTY Code Set to: %u", tempPtyCode);
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "setPtyCodeCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// testModeCallback(): Audio Test Tone Mode Control (true = Audio Test Mode On).
void testModeCallback(Control *sender, int type)
{
    char logBuff[60];

    // sprintf(logBuff, "testModeCallback: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == adjTestModeID) {
        if (type == S_ACTIVE) {
            ESPUI.setElementStyle(adjTestModeID, "background: red;");
            testModeFlg = true;
            tempStr     = "On";
            updateTestTones(true); // Reset Test Tone Elasped Timer.
        }
        else if (type == S_INACTIVE) {
            ESPUI.setElementStyle(adjTestModeID, "background: #bebebe;");
            testModeFlg = false;
            tempStr     = "Off";
        }
        else {
            tempStr = BAD_VALUE_STR;
            sprintf(logBuff, "testModeCallback: %s.", BAD_VALUE_STR);
            Log.errorln(logBuff);
        }
        sprintf(logBuff, "Test Mode Set to: %s.", tempStr.c_str());
        Log.infoln(logBuff);
    }
    else {
        sprintf(logBuff, "testModeCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
}

// ************************************************************************************************
// volAdjustCallback(): Adjust USB or Analog Volume. Sets Int8_t.

/*
   void volAdjustCallback(Control *sender, int type)
   {
    char logBuff[60];
    int16_t limitLo, limitHi;

    // sprintf(logBuff, "volAdjustCallback ID: %d, Value: %s", sender->id, sender->value.c_str());
    // Log.verboseln(logBuff);

    if (sender->id == adjAvolID) {
        analogVol = sender->value.toInt();
        limitLo   = (String(ANA_VOL_MIN_STR)).toInt();
        limitHi   = (String(ANA_VOL_MAX_STR)).toInt();

        if (analogVol < limitLo) {
            analogVol = limitLo;
        }
        else if (analogVol > limitHi) {
            analogVol = limitHi;
        }
        displaySaveWarning();
        Log.infoln("Analog Volume Set to: %u", analogVol);
    }
    else if (sender->id == adjUvolID) {
        usbVol  = sender->value.toInt();
        limitLo = (String(USB_VOL_MIN_STR)).toInt();
        limitHi = (String(USB_VOL_MAX_STR)).toInt();

        if (usbVol < limitLo) {
            usbVol = limitLo;
        }
        else if (usbVol > limitHi) {
            usbVol = limitHi;
        }
        displaySaveWarning();
        Log.infoln("USB Volume Set to: %u", usbVol);
    }
    else {
        sprintf(logBuff, "volAdjustCallback: %s.", BAD_SENDER_STR);
        Log.errorln(logBuff);
    }
   }
 */
