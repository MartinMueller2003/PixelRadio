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

protected:
   // values stored in configuration file
   bool        ControllerEnabled                = false;
   uint32_t    PiCode                           = 0;
   uint32_t    PtyCode                          = 0;
   String      ProgramServiceName;
   String      PayloadTest;
   // uint32_t    RdsMsgTime                       = 0;

   // operational data
   String      Name; // Name of this controller
   CtypeId     TypeId                           = NullControllerId;
   bool        TextFlag                         = false;
   bool        StopFlag                         = false;
   bool        activeTextFlag                   = false;
   bool        OnlineFlag                       = false;
   uint32_t    RdsMsgTime                       = 0;

   bool MsgHasBeenDisplayedFlag = false;

   // ESPUI control IDs
   uint16_t    EspuiParentElementId             = Control::noParent;
   uint16_t    ControlerEnabledElementId        = Control::noParent;
   uint16_t    EspuiMsgId                       = Control::noParent;

public:
                     c_ControllerCommon(String MyName, c_ControllerMgr::ControllerTypeId_t MyId);
   virtual           ~c_ControllerCommon();
   virtual void      begin() {}
   virtual void      poll() {}
   virtual void      AddControls(uint16_t ctrlTab) {}
   virtual void      RestoreConfiguration(ArduinoJson::JsonObject &config);
   virtual void      SaveConfiguration(ArduinoJson::JsonObject &config);
   String &          GetName() { return Name; }

   virtual void      Display(uint16_t msgId) { ESPUI.print(msgId, String(F("Source: ")) + Name + String(F(" Controller"))); }

   virtual bool      GetNextRdsMsgToDisplay(c_ControllerMgr::CurrentRdsMsgInfo_t &RdsMsgInfo) { return false; }
   virtual void      ClearMsgHasBeenDisplayedFlag() { MsgHasBeenDisplayedFlag = false; }

   virtual void      SetPiCode(uint32_t value) { PiCode = value; }
   virtual uint32_t  GetPiCode() { return PiCode; }

   virtual void      SetPtyCode(uint32_t value) { PtyCode = value; }
   virtual uint32_t  GetPtyCode() { return PtyCode; }

   virtual void      SetRdsProgramServiceName(String &value) { ProgramServiceName = value; }
   virtual String    GetRdsProgramServiceName() { return ProgramServiceName; }

   // Start of to be deleted
   virtual void      SetTextFlag(bool value) { TextFlag = value; }
   virtual bool      GetTextFlag() { return TextFlag; }

   virtual void      SetStopFlag(bool value) { StopFlag = value; }
   virtual bool      GetStopFlag() { return StopFlag; }

   virtual void      SetPayloadText(String &value) { PayloadTest = value; }
   virtual String &  GetPayloadText() { return PayloadTest; }

   virtual void      SetRdsMsgTime(uint32_t value) { RdsMsgTime = value; }
   virtual uint32_t  GetRdsMsgTime() { return RdsMsgTime; }

   virtual void      SetActiveTextFlag(bool value) { activeTextFlag = value; }
   virtual bool      GetActiveTextFlag() { return activeTextFlag; }

   virtual bool      ControllerIsEnabled() { return ControllerEnabled; }

   virtual bool      CheckRdsTextAvailable() { return false; }
   // End of to be deleted

}; // c_ControllerCommon

// *********************************************************************************************
// EOF
