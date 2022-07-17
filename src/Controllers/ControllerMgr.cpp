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

struct ControllerDefinition_t
{
   c_ControllerMgr::ControllerTypeId_t Type;
   uint16_t                            ActiveBit;
   uint16_t                            SendingBit;
};

static const ControllerDefinition_t ControllerDefinitions[] = 
{
   {c_ControllerMgr::ControllerTypeId_t::SERIAL_CNTRL, SERIAL_CONTROLLER_ACTIVE_FLAG, 0x0008 },
   {c_ControllerMgr::ControllerTypeId_t::MQTT_CNTRL,   MQTT_CONTROLLER_ACTIVE_FLAG,   0x0004 },
   {c_ControllerMgr::ControllerTypeId_t::FPPD_CNTRL,   FPPD_CONTROLLER_ACTIVE_FLAG,   0x1000 },
   {c_ControllerMgr::ControllerTypeId_t::HTTP_CNTRL,   HTTP_CONTROLLER_ACTIVE_FLAG,   0x0002 },
   {c_ControllerMgr::ControllerTypeId_t::LOCAL_CNTRL,  LOCAL_CONTROLLER_ACTIVE_FLAG,  0x0001 },
   {c_ControllerMgr::ControllerTypeId_t::NO_CNTRL,     0,                             0x0000 },
};

// *********************************************************************************************
c_ControllerMgr::c_ControllerMgr()
{
   // DEBUG_START;

   for(auto& CurrentDefinition : ControllerDefinitions)
   {
      uint32_t index = int(CurrentDefinition.Type);
      ListOfControllers[index].ControllerId = CurrentDefinition.Type;
      ListOfControllers[index].ActiveBit    = CurrentDefinition.ActiveBit;
      ListOfControllers[index].SendingBit   = CurrentDefinition.SendingBit;
      
      switch(CurrentDefinition.Type)
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
   }

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
      // DEBUG_V(String("pController: 0x") + String(uint32_t(CurrentController.pController), HEX));
      CurrentController.pController->begin();
   }
   // DEBUG_END;
} // begin

// *********************************************************************************************
// Returns true if a new message is found
bool c_ControllerMgr::GetNextRdsMsgToDisplay(CurrentRdsMsgInfo_t & MsgInfo)
{
   DEBUG_START;

   bool Response = false;

   CurrentRdsMsgInfo.MsgText              = emptyString;
   CurrentRdsMsgInfo.MsgStartTimeMillis   = 0;
   CurrentRdsMsgInfo.MsgDurationMilliSec       = 0;
   CurrentRdsMsgInfo.ControllerId         = ControllerTypeId_t::NO_CNTRL;

   do // once
   {
      // pass through the controllers and grab the first message that is available to send.
      for(auto & CurrentController : ListOfControllers)
      {
         if(CurrentController.pController->GetNextRdsMsgToDisplay(CurrentRdsMsgInfo))
         {
            CurrentRdsMsgInfo.MsgStartTimeMillis = millis();
            CurrentRdsMsgInfo.ControllerId = CurrentController.ControllerId;

            Response = true;
            break;
         }
      }

   } while (false);

   // update the response
   MsgInfo = CurrentRdsMsgInfo;

   DEBUG_END;
   return Response;

} // GetNextMsgToDisplay

// *********************************************************************************************
void c_ControllerMgr::ClearMsgHasBeenDisplayedFlag()
{
   DEBUG_START;
   for (auto &CurrentController : ListOfControllers)
   {
      CurrentController.pController->ClearMsgHasBeenDisplayedFlag();
   }
   DEBUG_END;
} // ClearMsgHasBeenDisplayedFlag

// *********************************************************************************************
uint16_t c_ControllerMgr::getControllerStatusSummary()
{
   DEBUG_START;

   uint16_t Response = 0;

   for(auto& CurrentController : ListOfControllers)
   {
      if(CurrentController.pController->ControllerIsEnabled())
      {
         Response |= CurrentController.ActiveBit;
      }

      if(CurrentController.ControllerId == CurrentRdsMsgInfo.ControllerId)
      {
         Response |= CurrentController.SendingBit;
      }
   }

   DEBUG_END;
   return Response;
}

// *********************************************************************************************
// CheckAnyRdsControllerEnabled(): 
// Determine if any (Local, HTTP, MQTT, Serial, FPPD) RDS Controller is Enabled.
bool c_ControllerMgr::CheckAnyRdsControllerEnabled(bool IncludeLocalController)
{
   // DEBUG_START;

   bool Response = false;
   for (auto &currentController : ListOfControllers)
   {
      if((currentController.ControllerId == LocalControllerId) &&
         (!IncludeLocalController))
      {
         continue;
      }
      Response |= currentController.pController->ControllerIsEnabled();
   }

   // DEBUG_END;
   return Response;
} // CheckAnyRdsControllerEnabled

// *********************************************************************************************
bool c_ControllerMgr::CheckRdsTextAvailable(bool IncludeLocalController)
{
   // DEBUG_START;

   bool Response = false;
   for (auto &currentController : ListOfControllers)
   {
      if((currentController.ControllerId == LocalControllerId) &&
         (!IncludeLocalController))
      {
         continue;
      }
      Response |= currentController.pController->CheckRdsTextAvailable();
   }

   return Response;

   // DEBUG_END;
} // CheckRdsTextAvailable

// ************************************************************************************************
// checkControllerIsAvailable(): 
// Using the supplied Controller ID, check if any HIGHER priority
// controllers are in operation. If none are present, then the supplied controller ID won't 
// be blocked and is available to use.
// true = Controller is ready, false = higher priority controller is active.
bool c_ControllerMgr::checkControllerIsAvailable(c_ControllerMgr::ControllerTypeId_t controllerId) 
{
   // DEBUG_START;

   bool Response = true;
   for (auto &currentController : ListOfControllers)
   {
      // is this the target controller
      if(currentController.ControllerId == controllerId)
      {
         // DEBUG_V("Target controller is the highest priority with a message");
         break;
      }
      
      if(currentController.pController->CheckRdsTextAvailable())
      {
         // DEBUG_V("Found a higher priority controller with a message. Target controller can NOT output a message");
         Response = false;
         break;
      }
   }

   // DEBUG_END;
   return Response;
}

// *********************************************************************************************
bool c_ControllerMgr::CheckAnyControllerIsDisplayingMessage(bool IncludeLocalController)
{
   // DEBUG_START;

   bool Response = false;
   for (auto &currentController : ListOfControllers)
   {
      if((currentController.ControllerId == LocalControllerId) &&
         (!IncludeLocalController))
      {
         continue;
      }
      Response |= (currentController.ControllerId == CurrentRdsMsgInfo.ControllerId);
   }

   // DEBUG_END;
   return Response;
}

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
   return ListOfControllers[Id].pController->ControllerIsEnabled();
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
   bool Response = false;

   for (ControllerInfo_t & CurrentController : ListOfControllers)
   {
      Response |= CurrentController.pController->GetActiveTextFlag();
   }

   return Response;
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
         String ControlerName = currentController.pController->GetName();
         if (!ControllerConfigs.containsKey(ControlerName))
         {
            ControllerConfigs.createNestedObject(ControlerName);
         }
         JsonObject ControllerConfig = ControllerConfigs[ControlerName];

         currentController.pController->SaveConfiguration(ControllerConfig);
      }

   } while (false);

   serializeJsonPretty(config, Serial);

   // DEBUG_END;
} // SaveConfiguration

// *********************************************************************************************
void c_ControllerMgr::RestoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   // serializeJsonPretty(config, Serial);

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
         String ControlerName = currentController.pController->GetName();
         if (!ControllerConfigs.containsKey(ControlerName))
         {
            ControllerConfigs.createNestedObject(ControlerName);
         }
         JsonObject ControllerConfig = ControllerConfigs[ControlerName];
         currentController.pController->RestoreConfiguration(ControllerConfig);
      }

   } while (false);

   // DEBUG_END;
} // RestoreConfiguration

c_ControllerMgr ControllerMgr;

// *********************************************************************************************
// EOF
