/*
   File: ControllerMessage.h
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
#include <Arduino.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <ESPUI.h>

class c_ControllerMessage
{
public:
            c_ControllerMessage();
   virtual  ~c_ControllerMessage();
   void     Activate(bool value);
   void     AddControls(uint16_t ctrlTab, uint16_t ParentElementId, uint16_t HiddenParentElementId = Control::noParent);
   void     CbDuration(Control* sender, int type);
   void     CbEnabled(Control* sender, int type);
   uint16_t GetElementId() { return EspuiMessageElementId; }
   String & GetMessage() { return Message; }
   void     HideMenu(bool value);
   void     RestoreConfig(ArduinoJson::JsonObject config);
   void     SaveConfig(ArduinoJson::JsonObject config);
   void     SelectMessage(bool value);
   void     SetMessage(String &value);

private:

   uint16_t    EspuiMessageElementId      = Control::noParent;

   String      Message;
   uint32_t    DurationSec                = 5;
   bool        Enabled                    = false;

}; // c_ControllerMessage

// *********************************************************************************************
// EOF
