/*
   File: ControllerUsbSERIAL.h
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
#include "ControllerMessages.h"

#define SERIAL_DEF_STR SERIAL_115_STR;
extern const PROGMEM char SERIAL_115_STR[];

class c_ControllerUsbSERIAL : public c_ControllerCommon
{
public:
            c_ControllerUsbSERIAL();
   virtual  ~c_ControllerUsbSERIAL();
   void     begin() { initSerialControl(); }
   void     poll() { serialCommands(); }
   void     saveConfiguration(ArduinoJson::JsonObject &config);
   void     restoreConfiguration(ArduinoJson::JsonObject &config);

   void     gpioSerialControl(String paramStr, uint8_t pin); // Serial handler for GPIO Commands.
   void     AddControls(uint16_t ctrlTab);
   uint16_t GetMsgId() { return EspuiMsgId; }
   void     GetNextRdsMessage(c_ControllerMgr::RdsMsgInfo_t &Response) { if(ControllerEnabled){ Messages.GetNextRdsMessage(Response); }}
   void     CbControllerEnabled(Control *sender, int type);

private:
   void     initSerialControl(void);
   void     CbBaudrateControl(Control *sender, int type);
   void     serialCommands(void);
   bool     SetBaudrate(String NewRate);

   RBD::SerialManager serial_manager;

   String   cmdStr;   // Serial Port Commands from user (CLI).
   String   paramStr;

   String   BaudRateStr = SERIAL_DEF_STR; // Parameter string.
   uint32_t BaudRate = 115200;
   c_ControllerMessages Messages;

}; // c_ControllerUsbSERIAL

// *********************************************************************************************
// EOF
