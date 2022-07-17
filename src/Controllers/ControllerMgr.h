/*
   File: ControllerMgr.h
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

#include "Arduino.h"
#include "ArduinoJson.h"

class c_ControllerCommon; // forward declaration

#define ControllerTypeId c_ControllerMgr::ControllerTypeId_t

#define LOCAL_CONTROLLER_ACTIVE_FLAG  0x0010
#define HTTP_CONTROLLER_ACTIVE_FLAG   0x0020
#define MQTT_CONTROLLER_ACTIVE_FLAG   0x0040
#define SERIAL_CONTROLLER_ACTIVE_FLAG 0x0080
#define FPPD_CONTROLLER_ACTIVE_FLAG   0x1000


class c_ControllerMgr
{
public:
   enum ControllerTypeId_t
   {
      SERIAL_CNTRL = 0,
      MQTT_CNTRL,
      FPPD_CNTRL,
      HTTP_CNTRL,
      LOCAL_CNTRL,
      NO_CNTRL,
      NumControllerTypes,
      ControllerIdStart = 0,
   };

struct CurrentRdsMsgInfo_t
{
   ControllerTypeId_t   ControllerId         = ControllerIdStart;
   String               MsgText;
   uint32_t             MsgStartTimeMillis   = 0;
   uint32_t             MsgDurationMilliSec  = 0;
   uint32_t             PiCode               = 0;
   uint32_t             PtyCode              = 0;
   String               ProgramServiceName;
};

protected : 
struct ControllerInfo_t
{
   ControllerTypeId_t ControllerId              = ControllerIdStart;
   uint16_t           ActiveBit                 = 0;
   uint16_t           SendingBit                = 0;
   c_ControllerCommon *pController              = nullptr;
};

   ControllerInfo_t ListOfControllers[ControllerTypeId_t::NumControllerTypes];
   CurrentRdsMsgInfo_t CurrentRdsMsgInfo;

public:
                        c_ControllerMgr();
   virtual              ~c_ControllerMgr();
   void                 poll();
   void                 begin();
   void                 AddControls(uint16_t ctrlTab);
   void                 Display(ControllerTypeId_t Id);
   c_ControllerCommon * GetControllerById(ControllerTypeId_t Id);
   String               GetName(ControllerTypeId_t Id);
   void                 RestoreConfiguration(ArduinoJson::JsonObject & config);
   void                 SaveConfiguration(ArduinoJson::JsonObject & config);
   uint16_t             getControllerStatusSummary();
   void                 GetCurrentRdsMsgInfo(CurrentRdsMsgInfo_t &MsgInfo) { MsgInfo = CurrentRdsMsgInfo; }

   bool                 GetNextRdsMsgToDisplay(CurrentRdsMsgInfo_t & CurrentMsgInfo);

   void                 ClearMsgHasBeenDisplayedFlag();

   void                 SetPiCode(ControllerTypeId_t Id, uint32_t value);
   uint32_t             GetPiCode(ControllerTypeId_t Id);

   void                 SetPtyCode(ControllerTypeId_t Id, uint32_t value);
   uint32_t             GetPtyCode(ControllerTypeId_t Id);

   void                 SetRdsProgramServiceName(ControllerTypeId_t Id, String value);
   String               GetRdsProgramServiceName(ControllerTypeId_t Id);

   // below are to be deleted
   bool                 CheckAnyRdsControllerEnabled(bool IncludeLocalController);
   bool                 CheckRdsTextAvailable(bool IncludeLocalController);
   bool                 checkControllerIsAvailable(c_ControllerMgr::ControllerTypeId_t controllerId);
   bool                 CheckAnyControllerIsDisplayingMessage(bool IncludeLocalController);

   void                 SetTextFlag(ControllerTypeId_t Id, bool value);
   bool                 GetTextFlag(ControllerTypeId_t Id);

   void                 SetStopFlag(ControllerTypeId_t Id, bool value);
   bool                 GetStopFlag(ControllerTypeId_t Id);

   void                 SetPayloadText(ControllerTypeId_t Id, String value);
   String               GetPayloadText(ControllerTypeId_t Id);

   void                 SetRdsMsgTime(ControllerTypeId_t Id, int32_t value);
   int32_t              GetRdsMsgTime(ControllerTypeId_t Id);

   void                 SetActiveTextFlag(ControllerTypeId_t Id, bool value);
   bool                 GetActiveTextFlag(ControllerTypeId_t Id);

   // void                 SetControlFlag(ControllerTypeId_t Id, bool value);
   bool                 GetControllerEnabledFlag(ControllerTypeId_t Id);
   bool                 IsControllerActive();

}; // c_ControllerMgr

#define CtypeId c_ControllerMgr::ControllerTypeId_t
#define LocalControllerId  CtypeId::LOCAL_CNTRL
#define HttpControllerId   CtypeId::HTTP_CNTRL
#define MqttControllerId   CtypeId::MQTT_CNTRL
#define FppdControllerId   CtypeId::FPPD_CNTRL
#define SerialControllerId CtypeId::SERIAL_CNTRL
#define NullControllerId   CtypeId::NO_CNTRL

extern c_ControllerMgr ControllerMgr;

// *********************************************************************************************
// EOF
