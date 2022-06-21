/*
   File: ControllerMessage.cpp
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
#include "ControllerMessage.h"
#include <map>
#include "../Language.h"
#include "../PixelRadio.h"

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

static uint16_t EspuiTitleMsgElementId          = Control::noParent;
static uint16_t EspuiDisplayDurationElementId   = Control::noParent;
static uint16_t EspuiEnabledElementId           = Control::noParent;
static uint16_t EspuiParentElementId            = Control::noParent;
static uint16_t EspuiRootElementId              = Control::noParent;
static uint16_t EspuiSeperatorMsgElementId      = Control::noParent;
static String   EmptyString = "";

// *********************************************************************************************
c_ControllerMessage::c_ControllerMessage()
{
   // DEBUG_START;
   // DEBUG_END;
} // c_ControllerMessage

// *********************************************************************************************
c_ControllerMessage::~c_ControllerMessage()
{
   // DEBUG_START;

   if (Control::noParent != EspuiMessageElementId)
   {
      // DEBUG_V(String("Remove Message: ") + Message);
      ESPUI.removeControl(EspuiMessageElementId);
      EspuiMessageElementId = Control::noParent;
   }

   // DEBUG_END;
} // c_ControllerMessage

// ************************************************************************************************
void c_ControllerMessage::Activate(bool value)
{
   // DEBUG_START;

   // DEBUG_V(String("Message: ") + Message);
   // DEBUG_V(String("  value: ") + value);

   Control *control = ESPUI.getControl(EspuiMessageElementId);
   if (control)
   {
      // DEBUG_V("Update Control");
      control->visible = value;
      ESPUI.updateControl(EspuiMessageElementId);
   }

   // DEBUG_END;

} // SelectControl

// ************************************************************************************************
void c_ControllerMessage::AddControls(uint16_t ctrlTab, uint16_t ParentElementId)
{
   // DEBUG_START;

   // DEBUG_V(String("ctrlTab: '") + String(ctrlTab) + "'");
   // DEBUG_V(String(" parent: '") + String(ParentElementId) + "'");

   EspuiParentElementId = ParentElementId;
   EspuiRootElementId = ctrlTab;

   if (Control::noParent == EspuiTitleMsgElementId)
   {
      // DEBUG_V(String("Add Title"));
      EspuiTitleMsgElementId = ESPUI.addControl(
          ControlType::Label,
          "Message Name",
          "Enable",
          ControlColor::Turquoise,
          EspuiRootElementId);
      ESPUI.setElementStyle(EspuiTitleMsgElementId, CSS_LABEL_STYLE_BLACK);
      
      // DEBUG_V(String("Add Enabled field"));
      EspuiEnabledElementId = ESPUI.addControl(
          ControlType::Switcher,
          EmptyString.c_str(),
          Enabled ? "1" : "0",
          ControlColor::Turquoise,
          EspuiTitleMsgElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessage *>(parm)->EnabledCb(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Seprator"));
      EspuiSeperatorMsgElementId = ESPUI.addControl(
          ControlType::Label,
          EmptyString.c_str(),
          "RDS Display Duration (SECS)",
          ControlColor::Turquoise,
          EspuiTitleMsgElementId);
      ESPUI.setElementStyle(EspuiSeperatorMsgElementId, CSS_LABEL_STYLE_BLACK);

      // DEBUG_V(String("Add Number field"));
      EspuiDisplayDurationElementId = ESPUI.addControl(
          ControlType::Number,
          EmptyString.c_str(),
          "0",
          ControlColor::Turquoise,
          EspuiTitleMsgElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessage *>(parm)->DurationCb(sender, type);
             }
          },
          nullptr);
      ESPUI.addControl(ControlType::Min, "Min", String(5), ControlColor::None, EspuiDisplayDurationElementId);
      ESPUI.addControl(ControlType::Max, "Max", String(900), ControlColor::None, EspuiDisplayDurationElementId);
   }

   if ((Control::noParent == EspuiMessageElementId) &&
       (Control::noParent != EspuiParentElementId ))
   {
      // DEBUG_V(String("              Message: ") + Message);
      // DEBUG_V(String("      ParentElementId: ") + EspuiParentElementId);
      EspuiMessageElementId = ESPUI.addControl(
            ControlType::Option,
            Message.c_str(),
            Message,
            ControlColor::Turquoise,
            EspuiParentElementId);
   }

   // DEBUG_END;

} // AddControls

// *********************************************************************************************
void c_ControllerMessage::DurationCb(Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("Message: ") + Message);
   // DEBUG_V(String("Value: ") + sender->value);

   DurationSec = atoi(sender->value.c_str());

   // DEBUG_END;
} // EnabledCb

// *********************************************************************************************
void c_ControllerMessage::EnabledCb(Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("Message: ") + Message);
   // DEBUG_V(String("Value: ") + sender->value);

   Enabled = sender->value == "1";
   // DEBUG_V(String("Enabled: ") + String(Enabled));

   // DEBUG_END;
} // EnabledCb

// *********************************************************************************************
void c_ControllerMessage::HideMenu(bool value)
{
   // DEBUG_START;

   Control *control = ESPUI.getControl(EspuiTitleMsgElementId);
   if(control)
   {
      // DEBUG_V("Set Visibility");
      control->visible = !value;
      ESPUI.updateControl(control);
   }

   // DEBUG_END;
} // HideMenu

// *********************************************************************************************
void c_ControllerMessage::RestoreConfig(ArduinoJson::JsonObject config)
{
   // DEBUG_START;

   if (config.containsKey(N_message))
   {
      // DEBUG_V("Read message");
      Message = (const char *)config[N_message];
      // DEBUG_V(String("Message: ") + Message);
   }

   if (config.containsKey(N_durationSec))
   {
      // DEBUG_V("Read Duration");
      DurationSec = config[N_durationSec];
      // DEBUG_V(String("DurationSec: ") + String(DurationSec));
   }

   if (config.containsKey(N_enabled))
   {
      // DEBUG_V("Read Enabled");
      Enabled = config[N_enabled];
      // DEBUG_V(String("Enabled: ") + String(Enabled));
   }

   // DEBUG_END;
} // RestoreConfig

// *********************************************************************************************
void c_ControllerMessage::SaveConfig(ArduinoJson::JsonObject config)
{
   // DEBUG_START;
   // DEBUG_V(String("Message: ") + Message);

   config[N_message]       = Message;
   config[N_durationSec]   = DurationSec;
   config[N_enabled]       = Enabled;

   // DEBUG_END;
} // SaveConfig

// ************************************************************************************************
void c_ControllerMessage::SelectMessage(bool value)
{
   // DEBUG_START;

   // DEBUG_V(String("Message: ") + Message);
   // DEBUG_V(String("  value: ") + value);

   do // once
   {
      if (false == value)
      {
         // DEBUG_V("Dont touch the controls");
         break;
      }

      // DEBUG_V("Set up Title");
      Control* control = ESPUI.getControl(EspuiTitleMsgElementId);
      if (control)
      {
         control->label = Message.c_str();
         control->user = this;
         ESPUI.updateControl(EspuiTitleMsgElementId);
      }

      // DEBUG_V("Set up Duration");
      control = ESPUI.getControl(EspuiDisplayDurationElementId);
      if (control)
      {
         control->value = String(DurationSec);
         control->user = this;
         ESPUI.updateControl(EspuiDisplayDurationElementId);
      }

      // DEBUG_V("Set up enabled CB");
      control = ESPUI.getControl(EspuiEnabledElementId);
      if (control)
      {
         control->value = (Enabled ? "1" : "0");
         control->user = this;
         ESPUI.updateControl(EspuiEnabledElementId);
      }
   } while (false);

   // DEBUG_END;

} // SelectControl

// *********************************************************************************************
void c_ControllerMessage::SetMessage(String &value)
{
   // DEBUG_START;

   // DEBUG_V(String("Message: ") + Message);
   // DEBUG_V(String("  value: ") + value);
   Message = value;

   if (Control::noParent != EspuiMessageElementId)
   {
      // DEBUG_V("Update message string in choice list")
      ESPUI.updateControlValue(EspuiMessageElementId, Message);
   }

   // DEBUG_END;
} // SetMessage

// *********************************************************************************************
// EOF
