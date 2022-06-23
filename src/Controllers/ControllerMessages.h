/*
   File: ControllerMessages.h
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
#include <list>
#include <ESPUI.h>
#include "ControllerMessage.h"
#include "../PixelRadio.h"

class c_ControllerMessagesUiControlIds
{
public:
            c_ControllerMessagesUiControlIds() {}
   virtual  ~c_ControllerMessagesUiControlIds() {}


private:
}; // c_ControllerMessagesUiControls

class c_ControllerMessages
{
public:
            c_ControllerMessages();
   virtual  ~c_ControllerMessages();
   void     RestoreConfig(ArduinoJson::JsonObject & config);
   void     SaveConfig(ArduinoJson::JsonObject & config);

   void     AddControls(uint16_t ctrlTab);

   void     CbButtonCreate(Control *sender, int type);
   void     CbButtonDelete(Control *sender, int type);
   void     CbButtonUpdate(Control *sender, int type);
   void     CbChoiceList(Control *sender, int type);
   void     CbSwitchDisplayFseqName(Control *sender, int type);
   void     CbTextChange(Control *sender, int type);

   void     clear() { Messages.clear(); }
   void     SetTitle(String & value);
   void     Activate(bool value);
   void     SetShowFseqNameSelection(bool value) { ShowFseqNameSelection = value; }

private:
   void HomeControl(uint16_t ControlId);

   c_ControllerMessage *FindMessageByText(String & text);

   String   Title                   = F("Messages");
   bool     ShowFseqNameSelection   = false;
   bool     DisplayFseqName         = false;

   std::list<c_ControllerMessage> Messages;

   
}; // c_ControllerMessages

// *********************************************************************************************
// EOF
