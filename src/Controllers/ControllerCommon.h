/*
   File: ControllerCommon.h
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

   Pure virtual base class to define the controller API and provide common functionality.
 */

// *********************************************************************************************
#pragma once

#include "Arduino.h"
#include "../PixelRadio.h"
#include <ESPUI.h>
#include <ArduinoJson.h>
#include "ControllerMgr.h"

class c_ControllerCommon
{
private:
   virtual void SaveControllerConfiguration(ArduinoJson::JsonObject &config) {}
   virtual void RestoreControllerConfiguration(ArduinoJson::JsonObject &config) {}

protected:
   String      Name; // Name of this controller
   CtypeId     TypeId                           = NullControllerId;
   uint32_t    PiCode                           = 0;
   uint32_t    PtyCode                          = 0;
   String      ProgramServiceName;
   String      PayloadTest;
   bool        TextFlag                         = false;
   bool        StopFlag                         = false;
   int32_t     RdsMsgTime                       = 0;
   bool        activeTextFlag                   = false;
   bool        ControllerEnabled                = false;
   // String      ControlStr;
   uint16_t    EspuiParentElementId             = 0;
   uint16_t    ControlerEnabledElementId        = 0;
   uint16_t    EspuiMsgId                       = 0;
   IPAddress   RemoteIp                         = INADDR_NONE;
   bool        OnlineFlag                       = false;
public:
                     c_ControllerCommon(String MyName, c_ControllerMgr::ControllerTypeId_t MyId);
   virtual           ~c_ControllerCommon();
   virtual void      begin() {}
   virtual void      poll() {}
   virtual void      AddControls(uint16_t ctrlTab) {}
           void      SaveConfiguration(ArduinoJson::JsonObject &config);
           void      RestoreConfiguration(ArduinoJson::JsonObject &config);

   String            GetName() { return Name; }

   virtual void      Display(uint16_t msgId) { ESPUI.print(msgId, String(F("Source: ")) + Name + String(F(" Controller"))); }
   
   virtual void      SetPiCode(uint32_t value) { PiCode = value; }
   virtual uint32_t  GetPiCode() { return PiCode; }

   virtual void      SetPtyCode(uint32_t value) { PtyCode = value; }
   virtual uint32_t  GetPtyCode() { return PtyCode; }

   virtual void      SetRdsProgramServiceName(String &value) { ProgramServiceName = value; }
   virtual String    GetRdsProgramServiceName() { return ProgramServiceName; }

   virtual void      SetTextFlag(bool value) { TextFlag = value; }
   virtual bool      GetTextFlag() { return TextFlag; }

   virtual void      SetStopFlag(bool value) { StopFlag = value; }
   virtual bool      GetStopFlag() { return StopFlag; }

   virtual void      SetPayloadText(String &value) { PayloadTest = value; }
   virtual String    GetPayloadText() { return PayloadTest; }

   virtual void      SetRdsMsgTime(int32_t value) { RdsMsgTime = value; }
   virtual int32_t   GetRdsMsgTime() { return RdsMsgTime; }

   virtual void      SetActiveTextFlag(bool value) { activeTextFlag = value; }
   virtual bool      GetActiveTextFlag() { return activeTextFlag; }

   virtual bool      GetControllerEnabledFlag() { return ControllerEnabled; }

}; // c_ControllerCommon

// *********************************************************************************************
// EOF
