/*
   File: ControllerLOCAL.cpp
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
#include "ControllerLOCAL.h"
#include "../language.h"

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

// *********************************************************************************************
// class c_ControllerLOCAL : public c_ControllerCommon

// *********************************************************************************************
c_ControllerLOCAL::c_ControllerLOCAL() : c_ControllerCommon("LOCAL", c_ControllerMgr::ControllerTypeId_t::LOCAL_CNTRL)
{

} // c_ControllerLOCAL

// *********************************************************************************************
c_ControllerLOCAL::~c_ControllerLOCAL() {}

// *********************************************************************************************
void c_ControllerLOCAL::AddControls(uint16_t ParentElementId)
{
   DEBUG_START;

   EspuiParentElementId = ParentElementId;

   ESPUI.addControl(ControlType::Separator,
                    "LOCAL CONTROL SETTINGS",
                    emptyString,
                    ControlColor::None,
                    EspuiParentElementId);

   ControlerEnabledElementId = ESPUI.addControl(
      ControlType::Switcher,
      "LOCAL CONTROL",
      ControllerEnabled ? "1" : "0",
      ControlColor::Turquoise,
      EspuiParentElementId,
      [](Control *sender, int type, void *param)
      {
         if(param)
         {
            reinterpret_cast<c_ControllerLOCAL*>(param)->CbControllerEnabled(sender, type);
         }
      },
      this);

   // Messages.SetTitle(Name + " " + N_Messages);
   Messages.AddControls(EspuiParentElementId);
   Messages.ActivateMessageSet(Name);
}

// ************************************************************************************************
void c_ControllerLOCAL::CbControllerEnabled(Control *sender, int type)
{
   // DEBUG_START;

   ControllerEnabled = (type == S_ACTIVE);

   // DEBUG_V();
   displaySaveWarning();
   displayRdsText(); // Update RDS RadioText.
   Log.infoln((String(F("LOCAL Controller Set to: ")) + String(ControllerEnabled ? "On" : "Off")).c_str());

   // DEBUG_END;
} // CbControllerEnabled

// *********************************************************************************************
void c_ControllerLOCAL::CreateDefaultMsgSet()
{
   DEBUG_START;

   Messages.clear();
   Messages.AddMessage(F("LOCAL"), F("Msg1"));
   Messages.AddMessage(F("LOCAL"), F("Msg2"));
   Messages.AddMessage(F("LOCAL"), F("Msg3"));

   DEBUG_END;
}

// *********************************************************************************************
void c_ControllerLOCAL::RestoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::RestoreConfiguration(config);
   Messages.RestoreConfig(config);

   // do we need to create a set of default messages?
   if(Messages.empty())
   {
      CreateDefaultMsgSet();
   }
   
   // DEBUG_END;
} // RestoreConfiguration

// *********************************************************************************************
void c_ControllerLOCAL::SaveConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::SaveConfiguration(config);
   Messages.SaveConfig(config);

   // DEBUG_END;
} // SaveConfiguration

// *********************************************************************************************
// EOF
