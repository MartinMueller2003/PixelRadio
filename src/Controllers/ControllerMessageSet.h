/*
   File: ControllerMessageSet.h
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

class c_ControllerMessageSet
{
public:
            c_ControllerMessageSet();
   virtual  ~c_ControllerMessageSet();
   void     RestoreConfig(ArduinoJson::JsonObject & config);
   void     SaveConfig(ArduinoJson::JsonObject & config);

   void     Activate(bool value);
   void     ActivateMessage(String MsgName);
   void     AddMessage(String MsgText);
   void     AddControls(c_ControllerMessage::MessageElementIds_t MessageElementIds);
   bool     empty() { return Messages.empty(); }
   void     EraseMsg(String MsgTxt);
   bool     HasMsg(String & MsgTxt) { return (Messages.end() != Messages.find(MsgTxt)); }
   void     SetName(String & value) { MsgSetName = value; }
   void     UpdateMsgText(String& OriginalMessageText, String& NewMessageText);
   void     GetNextRdsMessage(c_ControllerMgr::RdsMsgInfo_t &Response);

private:
   void     ShowMsgDetailsPane(bool value);

   c_ControllerMessage::MessageElementIds_t MessageElementIds;

   String MsgSetName;
   String CurrentMsgName;

   std::map < String, c_ControllerMessage > Messages;
   #define nullMessagesIterator static_cast<std::map < String, c_ControllerMessage >::iterator>(nullptr) 
   std::map < String, c_ControllerMessage >::iterator MessagesIterator = nullMessagesIterator;
   SemaphoreHandle_t MessagesSemaphore = NULL;

}; // c_ControllerMessageSet

// *********************************************************************************************
// EOF
