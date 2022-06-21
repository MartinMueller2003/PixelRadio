/*
   File: ControllerSERIAL.cpp
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
#include "ControllerSERIAL.h"
#include "../language.h"
#include <ArduinoLog.h>

#if __has_include("../memdebug.h")
#include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

       const PROGMEM char SERIAL_OFF_STR[] = "OFF";
static const PROGMEM char SERIAL_096_STR[] = "9600 Baud";
static const PROGMEM char SERIAL_192_STR[] = "19.2K Baud";
static const PROGMEM char SERIAL_576_STR[] = "57.6K Baud";
       const PROGMEM char SERIAL_115_STR[] = "115.2K Baud";

// ================================================================================================
c_ControllerSERIAL::c_ControllerSERIAL() : c_ControllerCommon("SERIAL", c_ControllerMgr::ControllerTypeId_t::SERIAL_CNTRL)
{
} // c_ControllerSERIAL
// ================================================================================================
c_ControllerSERIAL::~c_ControllerSERIAL() {}

// ================================================================================================
// ctrlSerialFlg(): Return true if Serial Controller is Enabled, else false;
bool c_ControllerSERIAL::ctrlSerialFlg(void)
{

   return ControllerEnabled;
} // ctrlSerialFlg

// ================================================================================================
void c_ControllerSERIAL::initSerialControl(void)
{
   cmdStr.reserve(40);   // Minimize memory re-allocations.
   paramStr.reserve(80); // Minimize memory re-allocations.

   Serial.flush();                       // Purge pending serial chars.
   serial_manager.start();               // Start Command Line Processor.
   serial_manager.setFlag(CMD_EOL_TERM); // EOL Termination character.
   serial_manager.setDelimiter('=');     // Parameter delimiter character.
   Serial.flush();                       // Repeat the flushing.
   Log.infoln(F("Serial Controller CLI is Enabled."));
}

// ================================================================================================
// gpioSerialControl(): Serial handler for GPIO Commands.
void c_ControllerSERIAL::gpioSerialControl(String paramStr, uint8_t pin)
{
   bool successFlg = true;
   String Response;

   // sprintf(charBuff, "Serial Controller: Received GPIO Pin-%d Command", pin);
   // Log.infoln(charBuff);

   successFlg = gpioCmd(paramStr, TypeId, pin);

   if (!successFlg)
   {
      Response = String(F("{\"")) + CMD_GPIO_STR + String(pin) + F("\": \"fail\"}");
   }
   else if (paramStr == CMD_GPIO_READ_STR)
   {
      Response = String(F("{\"")) + CMD_GPIO_STR + String(pin) + F("\": \"") + String(digitalRead(pin)) + F("\"}");
   }
   else
   {
      Response = String(F("{\"")) + CMD_GPIO_STR + String(pin) + F("\": \"ok\"}");
   }
   serial_manager.println(Response.c_str());
}

// ================================================================================================
// serialCommands(): Process the commands sent through the serial port.
// This is the Command Line Interface (CLI).
void c_ControllerSERIAL::serialCommands(void)
{
   extern String staNameStr;
  
   char printBuff[140 + sizeof(VERSION_STR) + STA_NAME_MAX_SZ];

   if (!ControllerEnabled)
   { // Serial Controller disabled, nothing to do. Exit.
      return;
   }

   if (serial_manager.onReceive())
   { // Process any serial commands from user (CLI).
      cmdStr = serial_manager.getCmd();
      cmdStr.trim();
      cmdStr.toLowerCase();

      // serial_manager.print("Raw CMD Parameter: "); // Debug message.
      // serial_manager.println(cmdStr);

      paramStr = serial_manager.getParam();
      paramStr.trim();

      // serial_manager.print("Raw CLI Parameter: "); // Debug message.
      // serial_manager.println(paramStr);

      if ((cmdStr == "?") || (cmdStr == "h") || (cmdStr == "help"))
      {
         serial_manager.println("");
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
         serial_manager.println("");
      }
      else if (cmdStr == CMD_AUDMODE_STR)
      {
         if (audioModeCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_AUDMODE_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_AUDMODE_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_FREQ_STR)
      {
         if (frequencyCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_FREQ_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_FREQ_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_GPIO19_STR)
      {
         gpioSerialControl(paramStr, GPIO19_PIN);
      }
      else if (cmdStr == CMD_GPIO23_STR)
      {
         gpioSerialControl(paramStr, GPIO23_PIN);
      }
      else if (cmdStr == CMD_GPIO33_STR)
      {
         gpioSerialControl(paramStr, GPIO33_PIN);
      }
      else if (cmdStr == CMD_INFO_STR)
      {
         if (infoCmd(paramStr, TypeId))
         {
            sprintf(printBuff,
                    "{\"%s\": \"ok\", \"version\": \"%s\", \"hostName\": \"%s\", \"ip\": \"%s\", \"rssi\": %d, \"status\": \"0x%02X\"}",
                    CMD_INFO_STR,
                    VERSION_STR,
                    staNameStr.c_str(),
                    WiFi.localIP().toString().c_str(),
                    WiFi.RSSI(),
                    getControllerStatus());
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_INFO_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == SERIAL_LOG_STR)
      {
         if (logCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", SERIAL_LOG_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", SERIAL_LOG_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_MUTE_STR)
      {
         if (muteCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_MUTE_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_MUTE_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_PICODE_STR)
      {
         if (piCodeCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PICODE_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PICODE_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_PSN_STR)
      {
         if (programServiceNameCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PSN_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PSN_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_RADIOTEXT_STR)
      {
         if (radioTextCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_RADIOTEXT_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_RADIOTEXT_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_START_STR)
      {
         if (startCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_START_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_START_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_STOP_STR)
      {
         if (stopCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_STOP_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_STOP_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_PERIOD_STR)
      {
         if (rdsTimePeriodCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_PERIOD_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_PERIOD_STR);
         }
         serial_manager.println(printBuff);
      }
      else if (cmdStr == CMD_REBOOT_STR)
      {
         if (rebootCmd(paramStr, TypeId))
         {
            sprintf(printBuff, "{\"%s\": \"ok\"}", CMD_REBOOT_STR);
         }
         else
         {
            sprintf(printBuff, "{\"%s\": \"fail\"}", CMD_REBOOT_STR);
         }
         serial_manager.println(printBuff);
      }
      else
      {
         if (cmdStr.equals(""))
         {                                              // Just a CR, no Text.
            sprintf(printBuff, "{\"cmd\": \"ready\"}"); // JSON Fmt.
         }
         else
         {
            sprintf(printBuff, "{\"cmd\": \"undefined\"}"); // JSON Fmt.
         }
         serial_manager.println(printBuff);
      }
      Serial.flush(); // Purge the serial controller to prevent conflicts with serialLog.
   }
}

// ************************************************************************************************
// serialCallback(): Set the Baud Rate for the Serial Controller. 
// The "Off" setting disables the controller.
void c_ControllerSERIAL::serialCallback(Control *sender, int type)
{
   // DEBUG_START;
   uint32_t OriginalBaudrate = BaudRate;

   // DEBUG_V(String("serialCallback ID: ") + String(sender->id) + ", Value: " + sender->value);

   do // once
   {
      if (sender->id != ControlerEnabledElementId)
      {
         // DEBUG_V();
         Log.errorln((String(F("serialCallback: ")) = BAD_SENDER_STR).c_str());
         extern uint16_t diagLogMsgID;
         ESPUI.print(diagLogMsgID, BAD_SENDER_STR); // Post warning message on Diagnostic Serial Log Panel.
         break;
      }

      if (false == SetBaudrate(sender->value))
      {
         // had an error
         Log.errorln((String(F("serialCallback: ")) + BAD_VALUE_STR).c_str());
         extern uint16_t diagLogMsgID;
         ESPUI.print(diagLogMsgID, ERROR_MSG_STR); // Post warning message on Diagnostic Serial Log Panel.
         // DEBUG_V();
      }
   } while (false);
   // DEBUG_V();

   if ((OriginalBaudrate != BaudRate) && (0 != BaudRate))
   {
      // DEBUG_V();
      Serial.end(); // Flush all characters in queue.
      Serial.begin(BaudRate);

      while (!Serial && !Serial.available())
      {
      }                 // Wait for Serial Port to be available.
      Serial.println(); // Push out any corrupted data due to baud change.
      // DEBUG_V();
   }
   // DEBUG_V();

   displaySaveWarning();
   Log.infoln((String(F("Serial Controller Set to: ")) + BaudRateStr).c_str());
   displayRdsText(); // Update RDS RadioText.

   // DEBUG_END;
} // serialCallback

// ************************************************************************************************
bool c_ControllerSERIAL::SetBaudrate(String NewRate)
{
   // DEBUG_START;
   bool Response = true;
   uint32_t OriginalBaudrate = BaudRate;

   do // once
   {
      if (NewRate == SERIAL_OFF_STR)
      {
         // DEBUG_V(SERIAL_OFF_STR);
         ControllerEnabled = false;
         BaudRateStr = SERIAL_OFF_STR;
         BaudRate = ESP_BAUD_DEF;
         break;
      }

      if (NewRate == SERIAL_096_STR)
      {
         // DEBUG_V(SERIAL_096_STR);
         ControllerEnabled = true;
         BaudRateStr = SERIAL_096_STR;
         BaudRate = 9600;
         break;
      }

      if (NewRate == SERIAL_192_STR)
      {
         // DEBUG_V(SERIAL_192_STR);
         ControllerEnabled = true;
         BaudRateStr = SERIAL_192_STR;
         BaudRate = 19200;
         break;
      }

      if (NewRate == SERIAL_576_STR)
      {
         // DEBUG_V(SERIAL_576_STR);
         ControllerEnabled = true;
         BaudRateStr = SERIAL_576_STR;
         BaudRate = 57600;
         break;
      }

      if (NewRate == SERIAL_115_STR)
      {
         // DEBUG_V(SERIAL_115_STR);
         ControllerEnabled = true;
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

// ************************************************************************************************
void c_ControllerSERIAL::AddControls(uint16_t ctrlTab)
{
   // DEBUG_START;

   EspuiParentElementId = ctrlTab;

   ControlerEnabledElementId = ESPUI.addControl(
       ControlType::Select,
       CTRL_SERIAL_STR,
       BaudRateStr,
       ControlColor::Turquoise,
       ctrlTab,
       [](Control *sender, int type, void *param)
       {
          reinterpret_cast<c_ControllerSERIAL *>(param)->serialCallback(sender, type);
       },
       this);

   // DEBUG_V(String("ControlerEnabledElementId: ") + String(ControlerEnabledElementId));
   ESPUI.addControl(ControlType::Option, SERIAL_OFF_STR, SERIAL_OFF_STR, ControlColor::None, ControlerEnabledElementId);
   ESPUI.addControl(ControlType::Option, SERIAL_096_STR, SERIAL_096_STR, ControlColor::None, ControlerEnabledElementId);
   ESPUI.addControl(ControlType::Option, SERIAL_192_STR, SERIAL_192_STR, ControlColor::None, ControlerEnabledElementId);
   ESPUI.addControl(ControlType::Option, SERIAL_576_STR, SERIAL_576_STR, ControlColor::None, ControlerEnabledElementId);
   ESPUI.addControl(ControlType::Option, SERIAL_115_STR, SERIAL_115_STR, ControlColor::None, ControlerEnabledElementId);
   // DEBUG_V();

   extern String logLevelStr;
   EspuiMsgId = ESPUI.addControl(
      ControlType::Label, 
      "SERIAL_MSG", 
      logLevelStr != DIAG_LOG_SILENT_STR ? CTLR_SERIAL_MSG_STR : "", 
      ControlColor::Turquoise, 
      ControlerEnabledElementId);

   ESPUI.setElementStyle(EspuiMsgId, CSS_LABEL_STYLE_BLACK);
   // DEBUG_END;
} // AddControls

// *********************************************************************************************
void c_ControllerSERIAL::RestoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::RestoreConfiguration(config);

   if(config.containsKey(N_Baudrate))
   {
      BaudRateStr = (const char *)config[N_Baudrate];
      SetBaudrate(BaudRateStr);
   }

   // DEBUG_END;
} // RestoreConfiguration

// *********************************************************************************************
void c_ControllerSERIAL::SaveConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::SaveConfiguration(config);

   config[N_Baudrate] = BaudRateStr;

   // DEBUG_END;
} // SaveConfiguration

// *********************************************************************************************
// EOF
