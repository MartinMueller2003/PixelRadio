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
#include <map>
#include <ESPUI.h>
#include "ControllerMessage.h"
#include "PixelRadio.h"
#include "ControllerMessageSet.h"

class c_ControllerMessages
{
public:
            c_ControllerMessages();
   virtual  ~c_ControllerMessages();
   void     RestoreConfig(ArduinoJson::JsonObject & config);
   void     SaveConfig(ArduinoJson::JsonObject & config);
   void     SaveConfig(ArduinoJson::JsonObject & config, String& SetName);

   void     ActivateMessageSet(String MsgSetName);
   void     AddMessage(String MsgSetName, String MsgText);
   bool     AddMessageSet(String MsgSetName);
   void     AddControls(uint16_t ctrlTab);

   void     CbButtonCreate(Control *sender, int type);
   void     CbButtonDelete(Control *sender, int type);
   void     CbButtonUpdate(Control *sender, int type);
   void     CbChoiceList(Control *sender, int type);
   void     CbSwitchDisplayFseqName(Control *sender, int type);
   void     CbTextChange(Control *sender, int type);
   void     clear() { MessageSets.clear(); }
   bool     empty() { return MessageSets.empty(); }
   void     SetShowFseqNameSelection(bool value);
   void     GetNextRdsMessage(c_ControllerMgr::RdsMsgInfo_t &Response) {}

private:
   uint16_t ParentElementId = Control::noParent;
   uint16_t StatusMsgElementId             = Control::noParent;
   uint16_t TextEntryElementId             = Control::noParent;
   uint16_t DisplayFseqNameElementId       = Control::noParent;
   uint16_t DisplayFseqNameLabelElementId  = Control::noParent;
   uint16_t ButtonCreateElementId          = Control::noParent;
   uint16_t ButtonDeleteElementId          = Control::noParent;
   uint16_t ButtonUpdateElementId          = Control::noParent;
   uint16_t InstructionElementId           = Control::noParent;
   uint16_t SeperatorMsgElementId          = Control::noParent;
   c_ControllerMessage::MessageElementIds_t  MessageElementIds;

   String   Title;
   String   CurrentMsgSetName;
   bool     ShowFseqNameSelection = false;
   bool     DisplayFseqName         = false;

   std::map<String, c_ControllerMessageSet> MessageSets;
   
}; // c_ControllerMessages

// *********************************************************************************************
// EOF
