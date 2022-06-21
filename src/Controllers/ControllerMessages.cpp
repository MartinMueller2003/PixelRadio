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

static c_ControllerMessagesUiControls ControllerMessagesUiControls;
static const String EmptyString = "";
static const String DefaultTextFieldValue = F("Type New Message Here");
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

   // Delete the controls
/*
   if (Control::noParent != ControllerMessagesUiControls.EspuiChoiceListElementId)
   {
      // DEBUG_V("Remove Messages Elements");
      ESPUI.removeControl(ControllerMessagesUiControls.EspuiChoiceListElementId);
      ESPUI.removeControl(ControllerMessagesUiControls.EspuiStatusMsgElementId);
      ESPUI.removeControl(ControllerMessagesUiControls.EspuiTextEntryElementId);
      ESPUI.removeControl(ControllerMessagesUiControls.EspuiButtonCreateElementId);
      ESPUI.removeControl(ControllerMessagesUiControls.EspuiButtonDeleteElementId);
      ESPUI.removeControl(ControllerMessagesUiControls.EspuiButtonUpdateElementId);
      ESPUI.removeControl(ControllerMessagesUiControls.EspuiDisplayFseqNameElementId);
      ESPUI.removeControl(ControllerMessagesUiControls.EspuiDisplayFseqNameLabelElementId);
   }
*/
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
      if (Control::noParent == ControllerMessagesUiControls.EspuiChoiceListElementId)
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

      HomeControl(ControllerMessagesUiControls.EspuiParentElementId);
      HomeControl(ControllerMessagesUiControls.EspuiChoiceListElementId);
      HomeControl(ControllerMessagesUiControls.EspuiStatusMsgElementId);
      HomeControl(ControllerMessagesUiControls.EspuiTextEntryElementId);
      HomeControl(ControllerMessagesUiControls.EspuiDisplayFseqNameElementId);
      HomeControl(ControllerMessagesUiControls.EspuiDisplayFseqNameLabelElementId);
      HomeControl(ControllerMessagesUiControls.EspuiButtonCreateElementId);
      HomeControl(ControllerMessagesUiControls.EspuiButtonDeleteElementId);
      HomeControl(ControllerMessagesUiControls.EspuiButtonUpdateElementId);

      Control *ChoiceListControl = ESPUI.getControl(ControllerMessagesUiControls.EspuiChoiceListElementId);
      if (nullptr == ChoiceListControl)
      {
         // DEBUG_V("Cannot locate the message control");
         break;
      }

      // DEBUG_V(String("Set title: '") + Title + "'");
      ChoiceListControl->label = Title.c_str();
      if (Messages.size())
      {
         // DEBUG_V(String("Update the choice list to: '") + Messages.front().GetMessage() + "'");
         ChoiceListControl->value = Messages.front().GetMessage();
         Messages.front().SelectMessage(true);
         ESPUI.updateText(ControllerMessagesUiControls.EspuiTextEntryElementId,
                          ChoiceListControl->value);
         EmptyControlMessage.HideMenu(false);
      }
      else
      {
         // DEBUG_V("No Messages to update");
         ChoiceListControl->value = EmptyString;
         ESPUI.updateText(ControllerMessagesUiControls.EspuiStatusMsgElementId,
                          EmptyString);
         ESPUI.updateText(ControllerMessagesUiControls.EspuiTextEntryElementId,
                          DefaultTextFieldValue);
         EmptyControlMessage.HideMenu(true);
      }

      // DEBUG_V("Update Choice list control");
      ESPUI.updateControl(ChoiceListControl->id);
      TextChangeCb(nullptr, 0);

      // DEBUG_V();
   } while (false);

   // DEBUG_END;

} // Activate

// ************************************************************************************************
void c_ControllerMessages::AddControls(uint16_t ctrlTab)
{
   // DEBUG_START;

   ControllerMessagesUiControls.EspuiParentElementId = ctrlTab;

   if (Control::noParent == ControllerMessagesUiControls.EspuiChoiceListElementId)
   {
      // DEBUG_V(String("Add Select"));
      ControllerMessagesUiControls.EspuiChoiceListElementId = ESPUI.addControl(
          ControlType::Select,
          Title.c_str(),
          EmptyString,
          ControlColor::Turquoise,
          ctrlTab,
          [](Control *sender, int type, void *parm)
          {
             reinterpret_cast<c_ControllerMessages *>(parm)->ChoiceListCb(sender, type);
          },
          this);

      // DEBUG_V(String("Add Text"));
      ControllerMessagesUiControls.EspuiTextEntryElementId = ESPUI.addControl(
          ControlType::Text,
          EmptyString.c_str(),
          DefaultTextFieldValue,
          ControlColor::None,
          ControllerMessagesUiControls.EspuiChoiceListElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessages *>(parm)->TextChangeCb(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Size"));
      ESPUI.addControl(
         ControlType::Max, 
         EmptyString.c_str(), 
         "64", 
         ControlColor::None, 
         ControllerMessagesUiControls.EspuiTextEntryElementId);

      // DEBUG_V(String("Add Create"));
      ControllerMessagesUiControls.EspuiButtonCreateElementId = ESPUI.addControl(
          ControlType::Button,
          EmptyString.c_str(),
          " Create ",
          ControlColor::None,
          ControllerMessagesUiControls.EspuiChoiceListElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessages *>(parm)->ButtonCreateCb(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Delete"));
      ControllerMessagesUiControls.EspuiButtonDeleteElementId = ESPUI.addControl(
          ControlType::Button,
          EmptyString.c_str(),
          " Delete ",
          ControlColor::None,
          ControllerMessagesUiControls.EspuiChoiceListElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessages *>(parm)->ButtonDeleteCb(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Update"));
      ControllerMessagesUiControls.EspuiButtonUpdateElementId = ESPUI.addControl(
          ControlType::Button,
          EmptyString.c_str(),
          " Update ",
          ControlColor::None,
          ControllerMessagesUiControls.EspuiChoiceListElementId,
          [](Control *sender, int type, void *parm)
          {
             if (nullptr != parm)
             {
                reinterpret_cast<c_ControllerMessages *>(parm)->ButtonUpdateCb(sender, type);
             }
          },
          nullptr);

      // DEBUG_V(String("Add Label 1"));
      ControllerMessagesUiControls.EspuiStatusMsgElementId = ESPUI.addControl(
          ControlType::Label,
          EmptyString.c_str(),
          EmptyString,
          ControlColor::Carrot,
          ControllerMessagesUiControls.EspuiChoiceListElementId);
      ESPUI.setElementStyle(ControllerMessagesUiControls.EspuiStatusMsgElementId, 
                            CSS_LABEL_STYLE_BLACK);

      if (EnableDisplayFseqName)
      {
         // DEBUG_V(String("Add Label 2"));
         ControllerMessagesUiControls.EspuiDisplayFseqNameLabelElementId = ESPUI.addControl(
             ControlType::Label,
             EmptyString.c_str(),
             "Display FSEQ File Name",
             ControlColor::Turquoise,
             ControllerMessagesUiControls.EspuiChoiceListElementId);
         ESPUI.setElementStyle(ControllerMessagesUiControls.EspuiDisplayFseqNameLabelElementId,
                               CSS_LABEL_STYLE_BLACK);

         // DEBUG_V(String("Add Display fseq name Switcher"));
         ControllerMessagesUiControls.EspuiDisplayFseqNameElementId = ESPUI.addControl(
             ControlType::Switcher,
             EmptyString.c_str(),
             DisplayFseqName ? "1" : "0",
             ControlColor::None,
             ControllerMessagesUiControls.EspuiChoiceListElementId,
             [](Control *sender, int type, void *parm)
             {
                if (nullptr != parm)
                {
                   reinterpret_cast<c_ControllerMessages *>(parm)->DisplayFseqNameCb(sender, type);
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
      CurrentMessage.AddControls(ctrlTab, ControllerMessagesUiControls.EspuiChoiceListElementId);
   }
   // DEBUG_V();

   if (Messages.size())
   {
      // DEBUG_V();

      ESPUI.updateSelect(ControllerMessagesUiControls.EspuiChoiceListElementId, Messages.front().GetMessage());
   }
   // DEBUG_V();
   TextChangeCb(nullptr, 0);

   // DEBUG_END;

} // AddControls

// ************************************************************************************************
void c_ControllerMessages::ButtonCreateCb(Control *sender, int type)
{
   // DEBUG_START;

   Control *TextControl = ESPUI.getControl(ControllerMessagesUiControls.EspuiTextEntryElementId);

   do // once
   {
      if (type != B_DOWN)
      {
         // DEBUG_V("Ignore unwanted button action");
         break;
      }

      // DEBUG_V("Create a Message");

      Messages.push_back(EmptyControlMessage);
      Messages.back().SetMessage(TextControl->value);
      Messages.back().AddControls(ControllerMessagesUiControls.EspuiParentElementId,
                                  ControllerMessagesUiControls.EspuiChoiceListElementId);
      Messages.back().Activate(true);
      Messages.back().SelectMessage(true);
      Messages.back().HideMenu(false);
      ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiStatusMsgElementId, TextControl->value);
      ESPUI.updateSelect(ControllerMessagesUiControls.EspuiChoiceListElementId, TextControl->value);
      TextChangeCb(nullptr, 0);
      ESPUI.jsonReload();

   } while (false);

   // DEBUG_END;

} // ButtonCreateCb

// ************************************************************************************************
void c_ControllerMessages::ButtonDeleteCb(Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("       Title: '") + Title + "'");

   Control *ChoiceControl = ESPUI.getControl(ControllerMessagesUiControls.EspuiChoiceListElementId);
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
            // DEBUG_V(String("count: ") + Messages.size());

            if (!Messages.empty())
            {
               // DEBUG_V("Set up for the next message");
               ESPUI.updateSelect(ControllerMessagesUiControls.EspuiChoiceListElementId, Messages.front().GetMessage());
               ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiTextEntryElementId, Messages.front().GetMessage());
               Messages.front().Activate(true);
               Messages.front().SelectMessage(true);
            }
            else
            {
               // DEBUG_V("Deleted last message");
               ESPUI.updateSelect(ControllerMessagesUiControls.EspuiChoiceListElementId, EmptyString);
               ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiTextEntryElementId, DefaultTextFieldValue);
               EmptyControlMessage.HideMenu(true);
            }
            ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiStatusMsgElementId, EmptyString);
            break;
         }
      } // for each message
      // DEBUG_V();

      TextChangeCb(nullptr, 0);
      ESPUI.jsonReload();

   } while (false);

   // DEBUG_END;
} // ButtonDeleteCb

// ************************************************************************************************
void c_ControllerMessages::ButtonUpdateCb(Control *sender, int type)
{
   // DEBUG_START;

   // DEBUG_V(String("       Title: '") + Title + "'");

   Control *TextControl = ESPUI.getControl(ControllerMessagesUiControls.EspuiTextEntryElementId);
   // DEBUG_V(String("  Text value: '") + TextControl->value + "'");

   Control *ChoiceControl = ESPUI.getControl(ControllerMessagesUiControls.EspuiChoiceListElementId);
   // DEBUG_V(String("Choice value: '") + ChoiceControl->value + "'");

   do // once
   {
      if (type != B_DOWN)
      {
         // DEBUG_V("Ignore unwanted button actions");
         break;
      }

      // DEBUG_V("Set the new value");
      c_ControllerMessage *Message = FindMessageByText(ChoiceControl->value);
      if (nullptr == Message)
      {
         // DEBUG_V("Could not find the original Message in the list");
         break;
      }

      // DEBUG_V("Set the new value");
      Message->SetMessage(TextControl->value);
      // DEBUG_V();
      ESPUI.updateSelect(ControllerMessagesUiControls.EspuiChoiceListElementId, TextControl->value);
      TextChangeCb(nullptr, 0);
      ESPUI.jsonReload();

   } while (false);

   // DEBUG_END;
} // ButtonUpdateCb

// ************************************************************************************************
void c_ControllerMessages::ChoiceListCb(Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("       Title: '") + Title + "'");

   String CurrentSeletedMessageName = sender->value;
   // DEBUG_V(String("Selected: '") + CurrentSeletedMessageName + "'");

   if (CurrentSeletedMessageName.equals(EmptyString))
   {
      ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiTextEntryElementId, DefaultTextFieldValue);
   }
   else
   {
      ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiTextEntryElementId, sender->value);

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

   TextChangeCb(nullptr, 0);

   // DEBUG_END;

} // CustomMessageCb

// ************************************************************************************************
void c_ControllerMessages::DisplayFseqNameCb(Control *sender, int type)
{
   // DEBUG_START;
   // DEBUG_V(String("Title: '") + Title + "'");
   // DEBUG_V(String(" Type: '") + type + "'");
   // DEBUG_V(String("value: '") + sender->value + "'");

   DisplayFseqName = (S_ACTIVE == type);
   // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName))

   // DEBUG_END;

} // DisplayFseqNameCb

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
   if (config.containsKey(N_EnableDisplayFseqName))
   {
      EnableDisplayFseqName = config[N_EnableDisplayFseqName];
   }

   if (config.containsKey(N_DisplayFseqName))
   {
      DisplayFseqName = config[N_DisplayFseqName];
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
   config[N_EnableDisplayFseqName] = EnableDisplayFseqName;
   config[N_DisplayFseqName] = DisplayFseqName;

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
void c_ControllerMessages::SetEnableDisplayFseqName(bool value)
{
   EnableDisplayFseqName = value;
} // SetEnableDisplayFseqName

// *********************************************************************************************
void c_ControllerMessages::SetTitle(String &value)
{
   // DEBUG_START;

   Title = value;
   // DEBUG_V(String("Title: ") + Title);

   // DEBUG_END;
} // SetTitle

// ************************************************************************************************
void c_ControllerMessages::TextChangeCb(Control *, int)
{
   // DEBUG_START;
   // DEBUG_V(String("       Title: '") + Title + "'");

   Control *ChoiceList  = ESPUI.getControl(ControllerMessagesUiControls.EspuiChoiceListElementId);
   Control *TextControl = ESPUI.getControl(ControllerMessagesUiControls.EspuiTextEntryElementId);

   // DEBUG_V(String("  Text value: '") + TextControl->value + "'");
   // DEBUG_V(String("Choice value: '") + ChoiceList->value + "'");

   bool EnableCreate = true;
   bool EnableDelete = true;
   bool EnableUpdate = true;

   do // once
   {
      if (ChoiceList->value.equals(EmptyString) || Messages.empty())
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
         ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiStatusMsgElementId, EmptyString);
         break;
      }

      if (-1 != TextControl->value.indexOf(DefaultTextFieldValue))
      {
         // DEBUG_V("User did not remove the default text");
         EnableCreate = false;
         EnableUpdate = false;
         ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiStatusMsgElementId, F("WARN: Instruction text cannot be in the message"));
         break;
      }

      if (TextControl->value.equals(EmptyString) && Messages.empty())
      {
         // DEBUG_V("Cant update an entry without a message");
         EnableCreate = false;
         EnableUpdate = false;
         ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiTextEntryElementId, DefaultTextFieldValue);
         ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiStatusMsgElementId, F("WARN: Empty message is not allowed"));
         break;
      }

      if (TextControl->value.equals(ChoiceList->value))
      {
         // DEBUG_V("No Change In Text");
         EnableCreate = false;
         EnableUpdate = false;
         ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiStatusMsgElementId, EmptyString);
         break;
      }

      // DEBUG_V("valid text that could be used for the existing Message");
      ESPUI.updateControlValue(ControllerMessagesUiControls.EspuiStatusMsgElementId, EmptyString);

   } while (false);

   // DEBUG_V("Update Buttons");
   ESPUI.getControl(ControllerMessagesUiControls.EspuiButtonCreateElementId)->enabled = EnableCreate;
   ESPUI.getControl(ControllerMessagesUiControls.EspuiButtonDeleteElementId)->enabled = EnableDelete;
   ESPUI.getControl(ControllerMessagesUiControls.EspuiButtonUpdateElementId)->enabled = EnableUpdate;

   ESPUI.updateControl(ControllerMessagesUiControls.EspuiButtonCreateElementId);
   ESPUI.updateControl(ControllerMessagesUiControls.EspuiButtonDeleteElementId);
   ESPUI.updateControl(ControllerMessagesUiControls.EspuiButtonUpdateElementId);

   // DEBUG_END;

} // TextChangeCb

// *********************************************************************************************
// EOF
