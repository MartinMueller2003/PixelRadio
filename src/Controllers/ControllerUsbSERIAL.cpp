/*
   File: ControllerUsbSERIAL.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Apr-06-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
   absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

    Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
 */

// *********************************************************************************************
#include "ControllerUsbSERIAL.h"
#include "language.h"
#include <ArduinoLog.h>

#if __has_include("memdebug.h")
#include "memdebug.h"
#endif //  __has_include("memdebug.h")

// ================================================================================================
c_ControllerUsbSERIAL::c_ControllerUsbSERIAL() : c_ControllerCommon("USB SERIAL", c_ControllerMgr::ControllerTypeId_t::USB_SERIAL_CNTRL)
{
} // c_ControllerUsbSERIAL

// ================================================================================================
c_ControllerUsbSERIAL::~c_ControllerUsbSERIAL() 
{
}

// ************************************************************************************************
void c_ControllerUsbSERIAL::AddControls(uint16_t ctrlTab)
{
   // DEBUG_START;

   c_ControllerCommon::AddControls(ctrlTab);

   uint16_t LabelId;
   LabelId = ESPUI.addControl(
                     ControlType::Label,
                     emptyString.c_str(),
                     String(N_BAUDRATE),
                     ControlColor::Turquoise,
                     ControlLabelElementId);
   ESPUI.setElementStyle(LabelId, CSS_LABEL_STYLE_BLACK);

   ControlerEnabledElementId = ESPUI.addControl(
                     ControlType::Select,
                     emptyString.c_str(),
                     BaudRateStr,
                     ControlColor::Turquoise,
                     ControlLabelElementId,
                     [](Control *sender, int type, void *param)
                     {
                        if(param)
                        {
                           reinterpret_cast<c_ControllerUsbSERIAL *>(param)->CbBaudrateControl(sender, type);
                        }
                     },
                     this);

   // DEBUG_V(String("ControlerEnabledElementId: ") + String(ControlerEnabledElementId));
   // ESPUI.addControl(ControlType::Option, SERIAL_OFF_STR, SERIAL_OFF_STR, ControlColor::None, ControlerEnabledElementId);
   ESPUI.addControl(ControlType::Option, SERIAL_096_STR, SERIAL_096_STR, ControlColor::None, ControlerEnabledElementId);
   ESPUI.addControl(ControlType::Option, SERIAL_192_STR, SERIAL_192_STR, ControlColor::None, ControlerEnabledElementId);
   ESPUI.addControl(ControlType::Option, SERIAL_576_STR, SERIAL_576_STR, ControlColor::None, ControlerEnabledElementId);
   ESPUI.addControl(ControlType::Option, SERIAL_115_STR, SERIAL_115_STR, ControlColor::None, ControlerEnabledElementId);
   // DEBUG_V();
#ifdef OldWay
   extern String logLevelStr;
   EspuiMsgId = ESPUI.addControl(
                     ControlType::Label, 
                     "SERIAL_MSG", 
                     (logLevelStr.equals(F(DIAG_LOG_SILENT_STR))) ? F(CTLR_SERIAL_MSG_STR) : emptyString,
                     ControlColor::Turquoise, 
                     ControlLabelElementId);
   ESPUI.setElementStyle(EspuiMsgId, CSS_LABEL_STYLE_BLACK);
#endif // def OldWay

   // DEBUG_END;
} // AddControls

// ================================================================================================
void c_ControllerUsbSERIAL::initSerialControl(void)
{
   // DEBUG_START;

   cmdStr.reserve(40);   // Minimize memory re-allocations.
   paramStr.reserve(80); // Minimize memory re-allocations.

   Serial.flush();                       // Purge pending serial chars.
   Serial.end();
   serial_manager.start();               // Start Command Line Processor.
   serial_manager.setFlag(CMD_EOL_TERM); // EOL Termination character.
   serial_manager.setDelimiter('=');     // Parameter delimiter character.
   Serial.flush();                       // Repeat the flushing.
   // Log.infoln(String(F("Serial Controller CLI is Enabled.")).c_str());

   // DEBUG_END;
}

// ================================================================================================
// gpioSerialControl(): Serial handler for GPIO Commands.
void c_ControllerUsbSERIAL::gpioSerialControl(String paramStr, uint8_t pin)
{
   DEBUG_START;

   bool successFlg = false;
   String Response;

   DEBUG_V(String("Serial Controller: Received GPIO Pin-") + String(pin) + " Command");
   // Log.infoln(charBuff);

#ifdef OldWay
   successFlg = gpioCmd(paramStr, TypeId, pin);
#endif // def OldWay

   Response = String(F("{\"")) + CMD_GPIO_STR + String(pin) + F("\": \"");
   if (!successFlg)
   {
      Response += F("fail\"}");
   }
   else if (paramStr == CMD_GPIO_READ_STR)
   {
      Response += String(digitalRead(pin)) + F("\"}");
   }
   else
   {
      Response += F("ok\"}");
   }
   serial_manager.println(Response.c_str());
   DEBUG_END;
}

// ************************************************************************************************
// CbBaudrateControl(): Set the Baud Rate for the Serial Controller. 
// The "Off" setting disables the controller.
void c_ControllerUsbSERIAL::CbBaudrateControl(Control *sender, int type)
{
   // DEBUG_START;
   uint32_t OriginalBaudrate = BaudRate;

   // DEBUG_V(String("CbBaudrateControl ID: ") + String(sender->id) + ", Value: " + sender->value);

   do // once
   {
      if (false == SetBaudrate(sender->value))
      {
         // had an error
         Log.errorln((String(F("serialCallback: ")) + F(BAD_VALUE_STR)).c_str());
         extern uint16_t diagLogMsgID;
         ESPUI.print(diagLogMsgID, ERROR_MSG_STR); // Post warning message on Diagnostic Serial Log Panel.
         // DEBUG_V();
      }
   } while (false);
   // DEBUG_V();

   displaySaveWarning();
   Log.infoln((Name + F(": Baudrate Set to: ") + BaudRateStr).c_str());

   // DEBUG_END;
} // CbBaudrateControl

// ================================================================================================
// serialCommands(): Process the commands sent through the serial port.
// This is the Command Line Interface (CLI).
void c_ControllerUsbSERIAL::serialCommands(void)
{
   /// DEBUG_START;

   do // once
   {
      if (!ControllerEnabled)
      {
         // DEBUG_V("Serial Controller disabled, nothing to do. Exit.");
         break;
      }

      while (serial_manager.onReceive())
      { // Process any serial commands from user (CLI).
         cmdStr = serial_manager.getCmd();
         cmdStr.trim();
         cmdStr.toLowerCase();
         serial_manager.println((String(F("Raw CMD Parameter: '")) + cmdStr + "'").c_str());

         paramStr = serial_manager.getParam();
         paramStr.trim();
         serial_manager.println((String(F("Raw CLI Parameter: '")) + paramStr + "'").c_str());
   #ifdef OldWay

         if ((cmdStr == "?") || (cmdStr == "h") || (cmdStr == "help")) {
            serial_manager.println(emptyString);
            serial_manager.println(F("========================================="));
            serial_manager.println(F("**  SERIAL CONTROLLER COMMAND SUMMARY  **"));
            serial_manager.println(F("========================================="));
            serial_manager.println(F(" AUDIO MODE      : aud=mono : stereo"));

            sprintf(printBuff, " FREQUENCY X10   : freq=%d <-> %d", FM_FREQ_MIN_X10, FM_FREQ_MAX_X10);
            serial_manager.println(printBuff);

            serial_manager.println(F(" GPIO-19 CONTROL : gpio19=read : outhigh : outlow"));
            serial_manager.println(F(" GPIO-23 CONTROL : gpio23=read : outhigh : outlow"));
            serial_manager.println(F(" GPIO-33 CONTROL : gpio33=read : outhigh : outlow"));

            serial_manager.println(F(" INFORMATION     : info=system"));
            serial_manager.println(F(" MUTE AUDIO      : mute=on : off"));

            sprintf(printBuff, " PROG ID CODE    : pic=0x%04X <-> 0x%04X", RDS_PI_CODE_MIN, RDS_PI_CODE_MAX);
            serial_manager.println(printBuff);

            sprintf(printBuff, " PROG SERV NAME  : psn=[%d char name]", CMD_PSN_MAX_SZ);
            serial_manager.println(printBuff);

            sprintf(printBuff, " RADIOTXT MSG    : rtm=[%d char message]", CMD_RT_MAX_SZ);
            serial_manager.println(printBuff);

            sprintf(printBuff, " RADIOTXT PERIOD : rtper=5 <-> %d secs", RDS_DSP_TM_MAX);
            serial_manager.println(printBuff);

            serial_manager.println(F(" REBOOT SYSTEM   : reboot=system"));
            serial_manager.println(F(" START RDS       : start=rds"));
            serial_manager.println(F(" STOP RDS        : stop=rds"));
            serial_manager.println(F(" LOG CONTROL     : log=silent : restore"));
            serial_manager.println(F("========================================="));
            serial_manager.println(emptyString);
         }
         else if (cmdStr == CMD_AUDMODE_STR) {
            if (audioModeCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_AUDMODE_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_AUDMODE_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_FREQ_STR) {
            if (frequencyCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_FREQ_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_FREQ_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_GPIO19_STR) {
            gpioSerialControl(paramStr, GPIO19_PIN);
         }
         else if (cmdStr == CMD_GPIO23_STR) {
            gpioSerialControl(paramStr, GPIO23_PIN);
         }
         else if (cmdStr == CMD_GPIO33_STR) {
            gpioSerialControl(paramStr, GPIO33_PIN);
         }
         else if (cmdStr == CMD_INFO_STR) {
            if (infoCmd(paramStr, TypeId)) {
                  sprintf(printBuff,
                        "{\"%s\": \"ok\", \"version\": \"%s\", \"hostName\": \"%s\", \"ip\": \"%s\", \"rssi\": %d, \"status\": \"0x%04X\"}",
                        CMD_INFO_STR,
                        VERSION_STR,
                        HostName.c_str(),
                        WiFi.localIP().toString().c_str(),
                        WiFi.RSSI(),
                        ControllerMgr.getControllerStatusSummary());
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_INFO_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == SERIAL_LOG_STR) {
            if (logCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", SERIAL_LOG_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", SERIAL_LOG_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_MUTE_STR) {
            if (muteCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_MUTE_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_MUTE_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_PICODE_STR) {
            if (piCodeCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PICODE_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PICODE_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_PTYCODE_STR) {
            if (ptyCodeCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PTYCODE_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PTYCODE_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_PSN_STR) {
            if (programServiceNameCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PSN_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PSN_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_RADIOTEXT_STR) {
            if (radioTextCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_RADIOTEXT_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_RADIOTEXT_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_RF_CARRIER_STR) {
            if (rfCarrierCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_RF_CARRIER_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_RF_CARRIER_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_START_STR) {
            if (startCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_START_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_START_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_STOP_STR) {
            if (stopCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_STOP_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_STOP_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_PERIOD_STR) {
            if (rdsTimePeriodCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PERIOD_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PERIOD_STR);
            }
            serial_manager.println(printBuff);
         }
         else if (cmdStr == CMD_REBOOT_STR) {
            if (rebootCmd(paramStr, TypeId)) {
                  sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_REBOOT_STR);
            }
            else {
                  sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_REBOOT_STR);
            }
            serial_manager.println(printBuff);
         }
         else {
            if (cmdStr == "") {                                 // Just a CR, no Text.
                  sprintf(printBuff, "{\"cmd\": \"ready\"}");     // JSON Fmt.
            }
            else {
                  sprintf(printBuff, "{\"cmd\": \"undefined\"}"); // JSON Fmt.
            }
            serial_manager.println(printBuff);
         }
         Serial.flush(); // Purge the serial controller to prevent conflicts with serialLog.
   #endif // def OldWay
      }

   } while (false);

   /// DEBUG_END;
}

// ************************************************************************************************
bool c_ControllerUsbSERIAL::SetBaudrate(String NewRate)
{
   // DEBUG_START;
   bool Response = true;
   uint32_t OriginalBaudrate = BaudRate;

   do // once
   {
      if (NewRate == SERIAL_096_STR)
      {
         // DEBUG_V(SERIAL_096_STR);
         BaudRateStr = SERIAL_096_STR;
         BaudRate = 9600;
         break;
      }

      if (NewRate == SERIAL_192_STR)
      {
         // DEBUG_V(SERIAL_192_STR);
         BaudRateStr = SERIAL_192_STR;
         BaudRate = 19200;
         break;
      }

      if (NewRate == SERIAL_576_STR)
      {
         // DEBUG_V(SERIAL_576_STR);
         BaudRateStr = SERIAL_576_STR;
         BaudRate = 57600;
         break;
      }

      if (NewRate == SERIAL_115_STR)
      {
         // DEBUG_V(SERIAL_115_STR);
         BaudRateStr = SERIAL_115_STR;
         BaudRate = 115200;
         break;
      }

      // DEBUG_V("Not a valid string");
      Response = false;
      ControllerEnabled = false;

      // DEBUG_V();

   } while (false);
   // DEBUG_V();

   if ((OriginalBaudrate != BaudRate) && (ControllerEnabled))
   {
      // DEBUG_V("Change Baudrate");
      Serial.end(); // Flush all characters in queue.
      Serial.begin(BaudRate);

      while (!Serial && !Serial.available())
      {
      }                 // Wait for Serial Port to be available.
      Serial.println(); // Push out any corrupted data due to baud change.
      // DEBUG_V("Change Baudrate - Done");
   }

   // DEBUG_END;
   return Response;

} // SetBaudrate

// *********************************************************************************************
void c_ControllerUsbSERIAL::restoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::restoreConfiguration(config);

   if(config.containsKey(N_Baudrate))
   {
      BaudRateStr = (const char *)config[N_Baudrate];
      SetBaudrate(BaudRateStr);
   }

   // DEBUG_END;
} // restoreConfiguration

// *********************************************************************************************
void c_ControllerUsbSERIAL::saveConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::saveConfiguration(config);

   config[N_Baudrate] = BaudRateStr;

   // DEBUG_END;
} // saveConfiguration

// *********************************************************************************************
// EOF
