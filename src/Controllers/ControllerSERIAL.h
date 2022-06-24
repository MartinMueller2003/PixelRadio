/*
   File: ControllerSERIAL.h
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
#pragma once
#include "ControllerCommon.h"
#include "RBD_SerialManager.h"

#define SERIAL_DEF_STR SERIAL_115_STR;
extern const PROGMEM char SERIAL_OFF_STR[];
extern const PROGMEM char SERIAL_115_STR[];

class c_ControllerSERIAL : public c_ControllerCommon
{
public:
            c_ControllerSERIAL();
   virtual  ~c_ControllerSERIAL();
   void     begin() { initSerialControl(); }
   void     poll() { serialCommands(); }
   void     SaveConfiguration(ArduinoJson::JsonObject &config);
   void     RestoreConfiguration(ArduinoJson::JsonObject &config);
   bool     GetControlFlag() { return ctrlSerialFlg(); }

   void     gpioSerialControl(String paramStr, uint8_t pin); // Serial handler for GPIO Commands.
   void     AddControls(uint16_t ctrlTab);
   uint16_t GetMsgId() { return EspuiMsgId; }

private:
   bool     ctrlSerialFlg(void); // Return true if Serial Controller is Enabled, else false;
   void     initSerialControl(void);
   void     CbBaudrateControl(Control *sender, int type);
   void     serialCommands(void);
   bool     SetBaudrate(String NewRate);

   RBD::SerialManager serial_manager;

   String   cmdStr;   // Serial Port Commands from user (CLI).
   String   paramStr;
   String   BaudRateStr = SERIAL_DEF_STR; // Parameter string.
   uint32_t BaudRate = 0;

}; // c_ControllerSERIAL

// *********************************************************************************************
// EOF
