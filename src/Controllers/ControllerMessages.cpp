/*
   File: ControllerMessages.cpp
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
#include "ControllerMessages.h"
#include <map>
#include "../Language.h"

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

static uint16_t EspuiParentElementId                = Control::noParent;
static uint16_t EspuiActiveChoiceListElementId      = Control::noParent;
static uint16_t EspuiHiddenChoiceListElementId      = Control::noParent;
static uint16_t EspuiStatusMsgElementId             = Control::noParent;
static uint16_t EspuiTextEntryElementId             = Control::noParent;
static uint16_t EspuiDisplayFseqNameElementId       = Control::noParent;
static uint16_t EspuiDisplayFseqNameLabelElementId  = Control::noParent;
static uint16_t EspuiButtonCreateElementId          = Control::noParent;
static uint16_t EspuiButtonDeleteElementId          = Control::noParent;
static uint16_t EspuiButtonUpdateElementId          = Control::noParent;

static const String DefaultTextFieldValue = F("Type New Message Here");
static const String DefaultTextWarningMsg = F("WARN: Instruction text cannot be in the message");
static const String EmptyMsgWarning       = F("WARN: Empty message is not allowed");

static c_ControllerMessage EmptyControlMessage;

// *********************************************************************************************
c_ControllerMessages::c_ControllerMessages()
{
   // DEBUG_START;

   // DEBUG_END;
} // c_ControllerMessages

// *********************************************************************************************
c_ControllerMessages::~c_ControllerMessages()
{
   // DEBUG_START;

   // cause the messages to delete themselves
   Messages.clear();

   // DEBUG_END;
} // c_ControllerMessages

// ************************************************************************************************
void c_ControllerMessages::Activate(bool value)
{
   // DEBUG_START;
   // DEBUG_V(String("       Title: '") + Title + "'");
   // DEBUG_V(String("       value: '") + String(value) + "'");

   do // once
   {
      if (Control::noParent == EspuiActiveChoiceListElementId)
      {
         // DEBUG_V("No Element IDs available. Not setting the messages");
         break;
      }

      // DEBUG_V("Set the individual Messages");
      for (auto &CurrentMessage : Messages)
      {
         CurrentMessage.Activate(value);
      }
      // DEBUG_V();

      if (false == value)
      {
         // DEBUG_V("Deactivating.");
         EmptyControlMessage.HideMenu(true);
         break;
      }
      // DEBUG_V("Activating.");

      HomeControl(EspuiActiveChoiceListElementId);
      HomeControl(EspuiStatusMsgElementId);
      HomeControl(EspuiTextEntryElementId);
      HomeControl(EspuiDisplayFseqNameElementId);
      HomeControl(EspuiDisplayFseqNameLabelElementId);
      HomeControl(EspuiButtonCreateElementId);
      HomeControl(EspuiButtonDeleteElementId);
      HomeControl(EspuiButtonUpdateElementId);

      Control *ChoiceListControl = ESPUI.getControl(EspuiActiveChoiceListElementId);
      if (nullptr == ChoiceListControl)
      {
         // DEBUG_V("Cannot locate the control");
         break;
      }

      // DEBUG_V(String("Set title: '") + Title + "'");
      ChoiceListControl->label = Title.c_str();
      if (Messages.size())
      {
         // DEBUG_V(String("Update the choice list to: '") + Messages.front().GetMessage() + "'");
         ChoiceListControl->value = Messages.front().GetMessage();
         Messages.front().SelectMessage(true);
         ESPUI.updateText(EspuiTextEntryElementId, ChoiceListControl->value);
         EmptyControlMessage.HideMenu(false);
      }
      else
      {
         // DEBUG_V("No Messages to update");
         ChoiceListControl->value = emptyString;
         ESPUI.print(EspuiStatusMsgElementId, emptyString);
         ESPUI.updateText(EspuiTextEntryElementId, DefaultTextFieldValue);
         EmptyControlMessage.HideMenu(true);
      }

      // DEBUG_V("Update Choice list control");
      ESPUI.updateControl(ChoiceListControl);
      CbTextChange(nullptr, 0);

      // DEBUG_V();
   } while (false);

   // DEBUG_END;

} // Activate

// ************************************************************************************************
void c_ControllerMessages::AddControls(uint16_t ctrlTab)
{
   // DEBUG_START;

   EspuiParentElementId = ctrlTab;

   if (Control::noParent == EspuiActiveChoiceListElementId)
   {
      // DEBUG_V(String("Add Select"));
      EspuiHiddenChoiceListElementId = ESPUI.addControl(
          ControlType::Select,
          emptyString.c_str(),
          emptyString,
          ControlColor::Turquoise,
          ctrlTab);

      // DEBUG_V(String("Add Select"));
      EspuiActiveChoiceListElementId = ESPUI.addControl(
          ControlType::Select,
          Title.c_str(),
          emptyString,
          ControlColor::Turquoise,
          ctrlTab,
          [](Control *sender, int type, void *parm)
          {
             reinterpret_cast<c_ControllerMessages *>(parm)->CbChoiceList(sender, type);
          },
          this);

      // DEBUG_V(String("Add Text"));
      EspuiTextEntryElementId = ESPUI.addControl(
          ControlType::Text,
          emptyString.c_str(),
          DefaultTextFieldValue,
          ControlColor::None,
          EspuiActiveChoiceListElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessages *>(parm)->CbTextChange(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Size"));
      ESPUI.addControl(
         ControlType::Max, 
         emptyString.c_str(), 
         "64", 
         ControlColor::None, 
         EspuiTextEntryElementId);

      // DEBUG_V(String("Add Create"));
      EspuiButtonCreateElementId = ESPUI.addControl(
          ControlType::Button,
          emptyString.c_str(),
          " Create ",
          ControlColor::None,
          EspuiActiveChoiceListElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessages *>(parm)->CbButtonCreate(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Delete"));
      EspuiButtonDeleteElementId = ESPUI.addControl(
          ControlType::Button,
          emptyString.c_str(),
          " Delete ",
          ControlColor::None,
          EspuiActiveChoiceListElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessages *>(parm)->CbButtonDelete(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Update"));
      EspuiButtonUpdateElementId = ESPUI.addControl(
          ControlType::Button,
          emptyString.c_str(),
          " Update ",
          ControlColor::None,
          EspuiActiveChoiceListElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessages *>(parm)->CbButtonUpdate(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Label 1"));
      EspuiStatusMsgElementId = ESPUI.addControl(
          ControlType::Label,
          emptyString.c_str(),
          emptyString,
          ControlColor::Carrot,
          EspuiActiveChoiceListElementId);
      ESPUI.setElementStyle(EspuiStatusMsgElementId, CSS_LABEL_STYLE_BLACK);

      if (ShowFseqNameSelection)
      {
         // DEBUG_V(String("Add Label 2"));
         EspuiDisplayFseqNameLabelElementId = ESPUI.addControl(
             ControlType::Label,
             emptyString.c_str(),
             "Display FSEQ File Name",
             ControlColor::Turquoise,
             EspuiActiveChoiceListElementId);
         ESPUI.setElementStyle(EspuiDisplayFseqNameLabelElementId,
                               CSS_LABEL_STYLE_BLACK);

         // DEBUG_V(String("Add Display fseq name Switcher"));
         // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName));
         EspuiDisplayFseqNameElementId = ESPUI.addControl(
             ControlType::Switcher,
             emptyString.c_str(),
             DisplayFseqName ? "1" : "0",
             ControlColor::None,
             EspuiActiveChoiceListElementId,
             [](Control *sender, int type, void *parm)
             {
                if (nullptr != parm)
                {
                   reinterpret_cast<c_ControllerMessages *>(parm)->CbSwitchDisplayFseqName(sender, type);
                }
             },
             nullptr);
      }
   }
   else
   {
      // DEBUG_V(String("NOT Adding Controls"));
   }

   // DEBUG_V();
   if(Messages.empty())
   {
      // DEBUG_V("Create message control set");
      EmptyControlMessage.AddControls(ctrlTab, Control::noParent);
      EmptyControlMessage.HideMenu(true);
   }

   // DEBUG_V();
   for (auto &CurrentMessage : Messages)
   {
      // DEBUG_V();
      CurrentMessage.AddControls(ctrlTab, EspuiActiveChoiceListElementId, EspuiHiddenChoiceListElementId);
   }
   // DEBUG_V();

   if (Messages.size())
   {
      // DEBUG_V();

      ESPUI.updateSelect(EspuiActiveChoiceListElementId, Messages.front().GetMessage());
   }
   // DEBUG_V();
   CbTextChange(nullptr, 0);

   // DEBUG_END;

} // AddControls

// ************************************************************************************************
void c_ControllerMessages::CbButtonCreate(Control *sender, int type)
{
   // DEBUG_START;

   Control *TextControl = ESPUI.getControl(EspuiTextEntryElementId);

   do // once
   {
      if (type != B_DOWN)
      {
         // DEBUG_V("Ignore unwanted button action");
         break;
      }

      // DEBUG_V("Create a new message");

      Messages.push_back(EmptyControlMessage);
      Messages.back().SetMessage(TextControl->value);
      Messages.back().AddControls(EspuiParentElementId,
                                  EspuiActiveChoiceListElementId,
                                  EspuiHiddenChoiceListElementId);
      Messages.back().SelectMessage(true);
      Messages.back().HideMenu(false);
      ESPUI.print(EspuiStatusMsgElementId, emptyString);
      ESPUI.updateSelect(EspuiActiveChoiceListElementId, TextControl->value);
      CbTextChange(nullptr, 0);
      displaySaveWarning();
      ESPUI.jsonDom(0);

   } while (false);

   // DEBUG_END;

} // ButtonCreateCb

// ************************************************************************************************
void c_ControllerMessages::CbButtonDelete(Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("       Title: '") + Title + "'");

   Control *ChoiceControl = ESPUI.getControl(EspuiActiveChoiceListElementId);
   // DEBUG_V(String("Choice value: '") + ChoiceControl->value + "'");

   do // once
   {
      if (type != B_DOWN)
      {
         // DEBUG_V("Ignore unwanted button action");
         break;
      }

      // DEBUG_V(String("Find message to delete out of ") + String(Messages.size()) + " messages");

      for (std::list<c_ControllerMessage>::iterator CurrentMessageIterator = Messages.begin();
           CurrentMessageIterator != Messages.end();
           ++CurrentMessageIterator)
      {
         String CurrentMessage = CurrentMessageIterator->GetMessage();
         // DEBUG_V(String("Message: ") + CurrentMessage);

         if (ChoiceControl->value.equals(CurrentMessage))
         {
            // DEBUG_V("Found message to delete");
            // DEBUG_V(String("ElementId: ") + CurrentMessageIterator->GetElementId());

            CurrentMessageIterator->Activate(false);

            // DEBUG_V(String("count: ") + Messages.size());
            Messages.erase(CurrentMessageIterator);
            CurrentMessageIterator = Messages.end();
            // DEBUG_V(String("count: ") + Messages.size());

            if (!Messages.empty())
            {
               // DEBUG_V("Set up for the next message");
               ESPUI.updateSelect(EspuiActiveChoiceListElementId, Messages.front().GetMessage());
               ESPUI.updateText(EspuiTextEntryElementId, Messages.front().GetMessage());
               Messages.front().Activate(true);
               Messages.front().SelectMessage(true);
            }
            else
            {
               // DEBUG_V("Deleted last message");
               ESPUI.updateSelect(EspuiActiveChoiceListElementId, emptyString);
               ESPUI.updateText(EspuiTextEntryElementId, DefaultTextFieldValue);
               EmptyControlMessage.HideMenu(true);
            }
            // DEBUG_V();
            ESPUI.print(EspuiStatusMsgElementId, emptyString);
            // DEBUG_V();
            break;
         }
      } // for each message
      // DEBUG_V();

      CbTextChange(nullptr, 0);
      // DEBUG_V();
      displaySaveWarning();
      // DEBUG_V();
      ESPUI.jsonDom(0);

   } while (false);

   // DEBUG_END;
} // ButtonDeleteCb

// ************************************************************************************************
void c_ControllerMessages::CbButtonUpdate(Control *sender, int type)
{
   // DEBUG_START;

   // DEBUG_V(String("       Title: '") + Title + "'");

   Control *TextControl = ESPUI.getControl(EspuiTextEntryElementId);
   // DEBUG_V(String("  Text value: '") + TextControl->value + "'");

   Control *ChoiceControl = ESPUI.getControl(EspuiActiveChoiceListElementId);
   // DEBUG_V(String("Choice value: '") + ChoiceControl->value + "'");

   do // once
   {
      if (type != B_DOWN)
      {
         // DEBUG_V("Ignore unwanted button actions");
         break;
      }

      // DEBUG_V("Find the old message");
      c_ControllerMessage *Message = FindMessageByText(ChoiceControl->value);
      if (nullptr == Message)
      {
         // DEBUG_V("Could not find the original Message in the list");
         break;
      }

      // DEBUG_V("Set the new value");
      Message->SetMessage(TextControl->value);
      // DEBUG_V("Update Select control");
      ESPUI.updateSelect(EspuiActiveChoiceListElementId, TextControl->value);
      CbTextChange(nullptr, 0);
      displaySaveWarning();
      ESPUI.jsonDom(0);

   } while (false);

   // DEBUG_END;
} // ButtonUpdateCb

// ************************************************************************************************
void c_ControllerMessages::CbChoiceList(Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("       Title: '") + Title + "'");

   String CurrentSeletedMessageName = sender->value;
   // DEBUG_V(String("Selected: '") + CurrentSeletedMessageName + "'");

   if (CurrentSeletedMessageName.equals(emptyString))
   {
      ESPUI.updateText(EspuiTextEntryElementId, DefaultTextFieldValue);
   }
   else
   {
      ESPUI.updateText(EspuiTextEntryElementId, sender->value);

      for(auto& CurrentMessage : Messages)
      {
         if (CurrentSeletedMessageName.equals(CurrentMessage.GetMessage()))
         {
            CurrentMessage.SelectMessage(true);
         }
         else
         {
            CurrentMessage.SelectMessage(false);
         }
      }
   }

   displaySaveWarning();
   displayRdsText(); // Update RDS RadioText.
   Log.infoln((String(F("FPPD Changed to: ")) + CurrentSeletedMessageName).c_str());

   // Update the warning and text fields
   CbTextChange(nullptr, 0);

   // DEBUG_END;

} // CustomMessageCb

// ************************************************************************************************
void c_ControllerMessages::CbSwitchDisplayFseqName(Control *sender, int type)
{
   // DEBUG_START;

   DisplayFseqName = (S_ACTIVE == type);
   // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName))

   displaySaveWarning();
   displayRdsText(); // Update RDS RadioText.
   Log.infoln((String(F("FPPD Display fseq Name Set to: ")) + String(DisplayFseqName ? "On" : "Off")).c_str());

   // DEBUG_END;

} // DisplayFseqNameCb

// ************************************************************************************************
void c_ControllerMessages::CbTextChange(Control *, int)
{
   // DEBUG_START;
   // DEBUG_V(String("       Title: '") + Title + "'");

   Control *ChoiceList  = ESPUI.getControl(EspuiActiveChoiceListElementId);
   Control *TextControl = ESPUI.getControl(EspuiTextEntryElementId);

   // DEBUG_V(String("  Text value: '") + TextControl->value + "'");
   // DEBUG_V(String("Choice value: '") + ChoiceList->value + "'");

   bool EnableCreate = true;
   bool EnableDelete = true;
   bool EnableUpdate = true;

   do // once
   {
      if (ChoiceList->value.equals(emptyString) || Messages.empty())
      {
         // DEBUG_V("Disable delete/update buttons");
         EnableDelete = false;
         EnableUpdate = false;
      }

      if (TextControl->value.equals(DefaultTextFieldValue))
      {
         // DEBUG_V("User did not remove the default text");
         EnableCreate = false;
         EnableUpdate = false;
         ESPUI.print(EspuiStatusMsgElementId, emptyString);
         break;
      }

      if (-1 != TextControl->value.indexOf(DefaultTextFieldValue))
      {
         // DEBUG_V("User did not remove the default text");
         EnableCreate = false;
         EnableUpdate = false;
         ESPUI.print(EspuiStatusMsgElementId, DefaultTextWarningMsg);
         break;
      }

      if (TextControl->value.equals(emptyString) && Messages.empty())
      {
         // DEBUG_V("Cant update an entry without a message");
         EnableCreate = false;
         EnableUpdate = false;
         ESPUI.updateText(EspuiTextEntryElementId, DefaultTextFieldValue);
         ESPUI.print(EspuiStatusMsgElementId, EmptyMsgWarning);
         break;
      }

      if (TextControl->value.equals(ChoiceList->value))
      {
         // DEBUG_V("No Change In Text");
         EnableCreate = false;
         EnableUpdate = false;
         ESPUI.print(EspuiStatusMsgElementId, emptyString);
         break;
      }

      // DEBUG_V("valid text that could be used for the existing Message");
      ESPUI.print(EspuiStatusMsgElementId, emptyString);

   } while (false);

   // DEBUG_V("Update Buttons");
   ESPUI.setEnabled(EspuiButtonCreateElementId, EnableCreate);
   ESPUI.setEnabled(EspuiButtonDeleteElementId, EnableDelete);
   ESPUI.setEnabled(EspuiButtonUpdateElementId, EnableUpdate);

   // DEBUG_END;

} // TextChangeCb

// ************************************************************************************************
c_ControllerMessage *c_ControllerMessages::FindMessageByText(String &text)
{
   // DEBUG_START;

   // DEBUG_V(String("text: ") + text);

   c_ControllerMessage *Message = nullptr;

   for (auto &CurrentMessage : Messages)
   {
      // DEBUG_V(String("Msg ID: ") + String(CurrentMessage.GetElementId()));
      if (text.equals(CurrentMessage.GetMessage()))
      {
         Message = &CurrentMessage;
         // DEBUG_V(String("Found Message: ") + CurrentMessage.GetMessage());

         break;
      }
   }

   // DEBUG_END;
   return Message;
} // FindMessageByElementId

// ************************************************************************************************
void c_ControllerMessages::HomeControl(uint16_t ControlId)
{
   // DEBUG_START;

   Control *control = ESPUI.getControl(ControlId);
   if(control)
   {
      // DEBUG_V();
      control->user = this;
      ESPUI.updateControl(ControlId);
   }

   // DEBUG_END;

} // HomeControl

// *********************************************************************************************
void c_ControllerMessages::RestoreConfig(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   // serializeJsonPretty(config, Serial);

   if (ShowFseqNameSelection)
   {
      if (config.containsKey(N_DisplayFseqName))
      {
         DisplayFseqName = config[N_DisplayFseqName];
         // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName));
      }
   }

   if (config.containsKey(N_name))
   {
      Title = (const char *)config[N_name];
   }

   // DEBUG_V();

   if (!config.containsKey(N_list))
   {
      // DEBUG_V();
      config.createNestedObject(N_list);
   }

   // DEBUG_V();
   JsonObject ListOfMessages = config[N_list];
   for (JsonPair CurrentMessageConfig : ListOfMessages)
   {
      String Key = CurrentMessageConfig.key().c_str();
      // DEBUG_V(String("Key: ") + Key);

      JsonObject MessageConfig = CurrentMessageConfig.value();

      // DEBUG_V(String("Add ") + Key);
      Messages.push_back(EmptyControlMessage);

      // DEBUG_V("Configure the message");
      Messages.back().RestoreConfig(MessageConfig);
   }

   // DEBUG_END;
} // RestoreConfig

// *********************************************************************************************
void c_ControllerMessages::SaveConfig(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   config[N_name] = Title;

   if (ShowFseqNameSelection)
   {
      config[N_DisplayFseqName] = DisplayFseqName;
      // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName));
   }

   if (!config.containsKey(N_list))
   {
      // DEBUG_V("Create List");
      config.createNestedObject(N_list);
   }
   JsonObject MessageConfigs = config[N_list];

   // DEBUG_V();

   uint16_t index = 0;
   for (auto &CurrentMessage : Messages)
   {
      if (!MessageConfigs.containsKey(String(index)))
      {
         // DEBUG_V(String("Create Message entry") + CurrentMessage.GetMessage());
         MessageConfigs.createNestedObject(String(index));
      }
      JsonObject MessageConfig = MessageConfigs[String(index)];
      CurrentMessage.SaveConfig(MessageConfig);
   }

   // DEBUG_V("Final");
   // serializeJsonPretty(config, Serial);

   // DEBUG_END;
} // SaveConfig

// *********************************************************************************************
void c_ControllerMessages::SetTitle(String &value)
{
   // DEBUG_START;

   Title = value;
   // DEBUG_V(String("Title: ") + Title);

   // DEBUG_END;
} // SetTitle

// *********************************************************************************************
// EOF
