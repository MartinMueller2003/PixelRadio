/*
   File: ControllerMgr.cpp
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
#include "ControllerMgr.h"
#include "ControllerCommon.h"
#include "ControllerNONE.h"
#include "ControllerFPPD.h"
#include "ControllerHTTP.h"
#include "ControllerLOCAL.h"
#include "ControllerMQTT.h"
#include "ControllerSERIAL.h"
#include "../language.h"

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

// *********************************************************************************************
c_ControllerMgr::c_ControllerMgr()
{
   // DEBUG_START;

   for (int index = 0; index < int(ControllerTypeId_t::NumControllerTypes); index++)
   {
      // DEBUG_V(String("init index: ") + String(index) + " Start");
      ListOfControllers[index].ControllerId = ControllerTypeId_t(index);
      switch(index)
      {
         case ControllerTypeId_t::NO_CNTRL:
         {
            // DEBUG_V("NO_CNTRL");
            ListOfControllers[index].pController = new c_ControllerNONE();
            break;
         }

         case ControllerTypeId_t::SERIAL_CNTRL:
         {
            // DEBUG_V("SERIAL_CNTRL");
            ListOfControllers[index].pController = new c_ControllerSERIAL();
            break;
         }

         case ControllerTypeId_t::MQTT_CNTRL:
         {
            // DEBUG_V("MQTT_CNTRL");
            ListOfControllers[index].pController = new c_ControllerMQTT();
            break;
         }

         case ControllerTypeId_t::FPPD_CNTRL:
         {
            // DEBUG_V("FPPD_CNTRL");
            ListOfControllers[index].pController = new c_ControllerFPPD();
            break;
         }

         case ControllerTypeId_t::HTTP_CNTRL:
         {
            // DEBUG_V("HTTP_CNTRL");
            ListOfControllers[index].pController = new c_ControllerHTTP();
            break;
         }

         case ControllerTypeId_t::LOCAL_CNTRL:
         {
            // DEBUG_V("LOCAL_CNTRL");
            ListOfControllers[index].pController = new c_ControllerLOCAL();
            break;
         } 
         
         default:
         {
            // DEBUG_V("default");
            ListOfControllers[index].pController = new c_ControllerNONE();
            // post an error
            break;
         }
      } // end switch(index)
      // DEBUG_V(String("init index: ") + String(index) + " Done");
   } // end for each controller type

   // DEBUG_END;

} // c_ControllerMgr

// *********************************************************************************************
c_ControllerMgr::~c_ControllerMgr()
{
   for (auto & CurrentController : ListOfControllers)
   {
      if (nullptr != CurrentController.pController)
      {
         delete CurrentController.pController;
         CurrentController.pController = nullptr;
      }
   }
}

// *********************************************************************************************
void c_ControllerMgr::begin()
{
   // DEBUG_START;
   for (auto &CurrentController : ListOfControllers)
   {
      // DEBUG_V();
      CurrentController.pController->begin();
   }
   // DEBUG_END;
} // begin

// *********************************************************************************************
c_ControllerCommon * c_ControllerMgr::GetControllerById(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController;
} // GetControllerById

// *********************************************************************************************
void c_ControllerMgr::Display(ControllerTypeId_t Id)
{
   extern uint16_t homeTextMsgID;
   ListOfControllers[Id].pController->Display(homeTextMsgID);
} // Display

// *********************************************************************************************
void c_ControllerMgr::SetActiveTextFlag(ControllerTypeId_t Id, bool value)
{
   ListOfControllers[Id].pController->SetActiveTextFlag(value);
} // SetActiveTextFlag

// *********************************************************************************************
bool c_ControllerMgr::GetActiveTextFlag(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetActiveTextFlag();
} // GetActiveTextFlag

// *********************************************************************************************
bool c_ControllerMgr::GetControllerEnabledFlag(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetControllerEnabledFlag();
} // GetControlEnabledFlag

// *********************************************************************************************
String c_ControllerMgr::GetName(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetName();
} // GetName

// *********************************************************************************************
void c_ControllerMgr::SetPiCode(ControllerTypeId_t Id, uint32_t NewPiCode)
{
   ListOfControllers[Id].pController->SetPiCode(NewPiCode);
} // SetPiCode

// *********************************************************************************************
uint32_t c_ControllerMgr::GetPiCode(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetPiCode();
} // SetPiCode

// *********************************************************************************************
void c_ControllerMgr::SetPtyCode(ControllerTypeId_t Id, uint32_t NewPiCode)
{
   ListOfControllers[Id].pController->SetPtyCode(NewPiCode);
} // SetPtyCode

// *********************************************************************************************
uint32_t c_ControllerMgr::GetPtyCode(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetPtyCode();
} // GetPtyCode

// *********************************************************************************************
void c_ControllerMgr::SetRdsProgramServiceName(ControllerTypeId_t Id, String value)
{
   ListOfControllers[Id].pController->SetRdsProgramServiceName(value);
} // SetRdsProgramServiceName

// *********************************************************************************************
String c_ControllerMgr::GetRdsProgramServiceName(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetRdsProgramServiceName();
} // GetRdsProgramServiceName

// *********************************************************************************************
void c_ControllerMgr::SetTextFlag(ControllerTypeId_t Id, bool value)
{
   ListOfControllers[Id].pController->SetTextFlag(value);
} // SetTextFlag

// *********************************************************************************************
bool c_ControllerMgr::GetTextFlag(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetTextFlag();
} // GetTextFlag

// *********************************************************************************************
void c_ControllerMgr::SetStopFlag(ControllerTypeId_t Id, bool value)
{
   ListOfControllers[Id].pController->SetStopFlag(value);
} // SetTextFlag

// *********************************************************************************************
bool c_ControllerMgr::GetStopFlag(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetStopFlag();
} // SetTextFlag

// *********************************************************************************************
void c_ControllerMgr::SetPayloadText(ControllerTypeId_t Id, String value)
{
   ListOfControllers[Id].pController->SetPayloadText(value);
} // SetPayloadText

// *********************************************************************************************
String c_ControllerMgr::GetPayloadText(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetPayloadText();
} // GetPayloadText

// *********************************************************************************************
void c_ControllerMgr::SetRdsMsgTime(ControllerTypeId_t Id, int32_t value)
{
   ListOfControllers[Id].pController->SetRdsMsgTime(value);
} // SetRdsMsgTime

// *********************************************************************************************
int32_t c_ControllerMgr::GetRdsMsgTime(ControllerTypeId_t Id)
{
   return ListOfControllers[Id].pController->GetRdsMsgTime();
} // GetRdsMsgTime

// *********************************************************************************************
bool c_ControllerMgr::IsControllerActive()
{
   bool response = false;

   for (ControllerInfo_t & CurrentController : ListOfControllers)
   {
      if (true == CurrentController.pController->GetActiveTextFlag())
      {
         response = true;
         break;
      }
   }

   return response;
} // IsControllerActive

// *********************************************************************************************
void c_ControllerMgr::poll()
{
   for (auto & currentController : ListOfControllers)
   {
      currentController.pController->poll();
   }
} // poll

// *********************************************************************************************
void c_ControllerMgr::AddControls(uint16_t ctrlTab)
{
   for (auto &currentController : ListOfControllers)
   {
      currentController.pController->AddControls(ctrlTab);
   }
} // AddControls

// *********************************************************************************************
void c_ControllerMgr::SaveConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   do // once
   {
      if (false == config.containsKey(N_controllers))
      {
         // DEBUG_V();
         config.createNestedObject(N_controllers);
      }
      // DEBUG_V();

      JsonObject ControllerConfigs = config[N_controllers];

      for (auto &currentController : ListOfControllers)
      {
         currentController.pController->SaveConfiguration(ControllerConfigs);
      }

   } while (false);

   // serializeJsonPretty(config, Serial);

   // DEBUG_END;
} // SaveConfiguration

// *********************************************************************************************
void c_ControllerMgr::RestoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   do // once
   {
      if (false == config.containsKey(N_controllers))
      {
         // DEBUG_V("No Config Found");
         break;
      }
      // DEBUG_V();

      JsonObject ControllerConfigs = config[N_controllers];

      for (auto &currentController : ListOfControllers)
      {
         currentController.pController->RestoreConfiguration(ControllerConfigs);
      }

   } while (false);

   // serializeJsonPretty(config, Serial);

   // DEBUG_END;
} // RestoreConfiguration

c_ControllerMgr ControllerMgr;

// *********************************************************************************************
// EOF
