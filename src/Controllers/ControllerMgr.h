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

protected:
   struct ControllerInfo_t
   {
      ControllerTypeId_t ControllerId = ControllerIdStart;
      c_ControllerCommon *pController = nullptr;
   };

   ControllerInfo_t ListOfControllers[ControllerTypeId_t::NumControllerTypes];

public:
   c_ControllerMgr();
   virtual ~c_ControllerMgr();
   void                 poll();
   void                 begin();
   c_ControllerCommon * GetControllerById(ControllerTypeId_t Id);
   void                 Display(ControllerTypeId_t Id);
   String               GetName(ControllerTypeId_t Id);
   void                 AddControls(uint16_t ctrlTab);
   void                 SaveConfiguration(ArduinoJson::JsonObject & config);
   void                 RestoreConfiguration(ArduinoJson::JsonObject & config);

   void                 SetPiCode(ControllerTypeId_t Id, uint32_t value);
   uint32_t             GetPiCode(ControllerTypeId_t Id);

   void                 SetPtyCode(ControllerTypeId_t Id, uint32_t value);
   uint32_t             GetPtyCode(ControllerTypeId_t Id);

   void                 SetRdsProgramServiceName(ControllerTypeId_t Id, String value);
   String               GetRdsProgramServiceName(ControllerTypeId_t Id);

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
