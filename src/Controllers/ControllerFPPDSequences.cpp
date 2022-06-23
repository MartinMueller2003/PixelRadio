/*
   File: ControllerFPPDSequences.cpp
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
#include "ControllerFPPDSequences.h"
#include <map>
#include "../Language.h"

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

// *********************************************************************************************
c_ControllerFPPDSequences::c_ControllerFPPDSequences()
{
   // DEBUG_START;

   String Key = N_default;
   c_ControllerFPPDSequence *Temp = new c_ControllerFPPDSequence();
   Sequences[Key] = *Temp;
   Sequences[Key].SetName(Key);
   delete Temp;

   // DEBUG_END;
} // c_ControllerFPPDSequences

// *********************************************************************************************
c_ControllerFPPDSequences::~c_ControllerFPPDSequences(){}

// ************************************************************************************************
void c_ControllerFPPDSequences::Activate()
{
   // DEBUG_START;

   Control *ChoiceList = ESPUI.getControl(EspuiChoiceListElementId);
   String Key = ChoiceList->value;
   // DEBUG_V(String("Activate key ") + Key);

   for (auto &CurrentSequence : Sequences)
   {
      // DEBUG_V(String("Deactivate Sequence: ") + CurrentSequence.first);
      CurrentSequence.second.Activate(false);
   }

   for (auto &CurrentSequence : Sequences)
   {
      if (CurrentSequence.first.equals(Key))
      {
         // DEBUG_V(String("Activate Sequence: ") + CurrentSequence.first);
         CurrentSequence.second.Activate(true);
      }
   }

   // DEBUG_END;
} // Activate

// ************************************************************************************************
void c_ControllerFPPDSequences::AddControls(uint16_t ctrlTab)
{
   // DEBUG_START;

   EspuiParentElementId = ctrlTab;

   if (Control::noParent == EspuiChoiceListElementId)
   {
      // DEBUG_V("Adding Select");
      EspuiChoiceListElementId = ESPUI.addControl(
         ControlType::Select,
         "FPPD Sequences",
         emptyString,
         ControlColor::Turquoise,
         EspuiParentElementId,
         [](Control *sender, int type, void* param)
         {
            reinterpret_cast<c_ControllerFPPDSequences*>(param)->CbChoiceList(sender, type);
         },
         this);

      // DEBUG_V("Adding Text");
      EspuiTextEntryElementId = ESPUI.addControl(
         ControlType::Text,
         emptyString.c_str(),
         DefaultTextFieldValue,
         ControlColor::None,
         EspuiChoiceListElementId,
         [](Control *sender, int type, void* param)
         {
            reinterpret_cast<c_ControllerFPPDSequences*>(param)->CbTextChange(sender, type);
         },
         this);

      // DEBUG_V("Adding Max");
      ESPUI.addControl(ControlType::Max, emptyString.c_str(), "64", ControlColor::None, EspuiTextEntryElementId);

      // DEBUG_V("Adding Create");
      EspuiButtonCreateElementId = ESPUI.addControl(
         ControlType::Button,
         emptyString.c_str(),
         " Create ",
         ControlColor::None,
         EspuiChoiceListElementId,
         [](Control *sender, int type, void* param)
         {
            reinterpret_cast<c_ControllerFPPDSequences*>(param)->CbButtonCreate(sender, type);
         },
         this);

      // DEBUG_V("Adding Delete");
      EspuiButtonDeleteElementId = ESPUI.addControl(
         ControlType::Button, 
         emptyString.c_str(), 
         " Delete ", 
         ControlColor::None, 
         EspuiChoiceListElementId,
         [](Control *sender, int type, void* param)
         {
            reinterpret_cast<c_ControllerFPPDSequences*>(param)->CbButtonDelete(sender, type);
         },
         this);

      // DEBUG_V("Adding Update");
      EspuiButtonUpdateElementId = ESPUI.addControl(
         ControlType::Button,
         emptyString.c_str(),
         " Update ",
         ControlColor::None,
         EspuiChoiceListElementId,
         [](Control *sender, int type, void* param)
         {
            reinterpret_cast<c_ControllerFPPDSequences*>(param)->CbButtonUpdate(sender, type);
         },
         this);

      // DEBUG_V("Adding Msg");
      EspuiStatusMsgElementId = ESPUI.addControl(
         ControlType::Label,
         emptyString.c_str(),
         emptyString,
         ControlColor::Turquoise,
         EspuiChoiceListElementId);
      ESPUI.setElementStyle(EspuiStatusMsgElementId, CSS_LABEL_STYLE_BLACK);
   }
   else
   {
      // DEBUG_V("NOT Adding Controls");
   }

   // DEBUG_V(String("Sequences.siz: ") + String(Sequences.size()));

   for(auto & CurrentSequence : Sequences)
   {
      // DEBUG_V(String("Adding Controls for ") + CurrentSequence.first);
      CurrentSequence.second.AddControls(EspuiParentElementId, EspuiChoiceListElementId);
   }

   // DEBUG_V(String("Activate ") + N_default);
   ESPUI.updateSelect(EspuiChoiceListElementId, N_default);
   Activate();

   CbTextChange(nullptr, 0);

   // DEBUG_END;

} // AddControls

// ************************************************************************************************
void c_ControllerFPPDSequences::CbButtonCreate(Control *sender, int type)
{
   // DEBUG_START;

   Control *TextControl = ESPUI.getControl(EspuiTextEntryElementId);
   String &Key = TextControl->value;
   // DEBUG_V(String("value: '") + Key + "'");

   do // once
   {
      if (type != B_DOWN)
      {
         // DEBUG_V("Ignore unwanted button action");
         break;
      }

      // DEBUG_V("Create a Sequence");
      DynamicJsonDocument NewConfigDoc(2048);
      JsonObject NewConfig = NewConfigDoc.to<JsonObject>();
      NewConfig[N_name] = Name;
      JsonObject SequenceList = NewConfig.createNestedObject(N_list);
      JsonObject SequenceEntry = SequenceList.createNestedObject(Key);
      SequenceEntry[N_name] = Key;
      SequenceEntry.createNestedObject(N_messages);

      RestoreConfig(NewConfig);
      // DEBUG_V();
      AddControls(EspuiParentElementId);

      // DEBUG_V(String("Set the Select List to ") + Key);
      ESPUI.updateSelect(EspuiChoiceListElementId, Key);

      // DEBUG_V("Activate");
      Activate();

      // DEBUG_V("Create a Sequence - Done");

      CbTextChange(nullptr, 0);
      ESPUI.jsonDom(0);
      displaySaveWarning();

   } while (false);

   // DEBUG_END;

} // ButtonCreateCb

// ************************************************************************************************
void c_ControllerFPPDSequences::CbButtonDelete(Control *sender, int type)
{
   // DEBUG_START;

   Control *ChoiceControl = ESPUI.getControl(EspuiChoiceListElementId);
   String Key = ChoiceControl->value;

   // DEBUG_V(String("Choice value: '") + Key + "'");

   do // once
   {
      if (type != B_DOWN)
      {
         // DEBUG_V("Ignore unwanted button action");
         break;
      }

      if (Sequences.end() == Sequences.find(ChoiceControl->value))
      {
         // DEBUG_V("Name does not exist");
         // DEBUG_V(String("  Key: '") + ChoiceControl->value + "'");
         break;
      }

      // DEBUG_V("now delete it");
      ESPUI.updateSelect(EspuiChoiceListElementId, N_default);
      ESPUI.updateControlValue(EspuiStatusMsgElementId, emptyString);
      ESPUI.updateControlValue(EspuiTextEntryElementId, DefaultTextFieldValue);
      Activate();

      Sequences[Key].Activate(false);
      Sequences.erase(Key);

      // DEBUG_V(String("Set the Select List to ") + N_default);
      ESPUI.updateSelect(EspuiChoiceListElementId, N_default);

      // DEBUG_V("Activate");
      Activate();

      CbTextChange(nullptr, 0);
      displaySaveWarning();

   } while (false);

   // DEBUG_END;
} // ButtonDeleteCb

// ************************************************************************************************
void c_ControllerFPPDSequences::CbButtonUpdate(Control *sender, int type)
{
   // DEBUG_START;

   Control *TextControl = ESPUI.getControl(EspuiTextEntryElementId);
   // DEBUG_V(String("Text value: '") + TextControl->value + "'");

   Control *ChoiceControl = ESPUI.getControl(EspuiChoiceListElementId);
   // DEBUG_V(String("Choice value: '") + ChoiceControl->value + "'");

   do // once
   {
      if (type != B_DOWN)
      {
         // DEBUG_V("Ignore unwanted button action");
      }

      // DEBUG_V("Set the new value");
      Sequences[ChoiceControl->value].SetName(TextControl->value);
      ESPUI.updateSelect(EspuiChoiceListElementId, TextControl->value);
      CbTextChange(nullptr, 0);
      displaySaveWarning();

   } while (false);

   // DEBUG_END;
} // ButtonUpdateCb

// ************************************************************************************************
void c_ControllerFPPDSequences::CbChoiceList(Control *sender, int type)
{
   // DEBUG_START;

   String CurrentSeletedSequenceName = sender->value;
   // DEBUG_V(String("Selected: '") + CurrentSeletedSequenceName + "'");

   if (CurrentSeletedSequenceName.equals(N_default))
   {
      // DEBUG_V("Selected Default message set");
      ESPUI.updateControlValue(EspuiTextEntryElementId, DefaultTextFieldValue);
   }
   else
   {
      // DEBUG_V("Selected Sequence Specific message set");
      ESPUI.updateControlValue(EspuiTextEntryElementId, CurrentSeletedSequenceName);
   }

   // DEBUG_V("Activate");
   Activate();

   CbTextChange(nullptr, 0);
   ESPUI.jsonReload();

   // DEBUG_END;

} // ChoiceListCb

// ************************************************************************************************
void c_ControllerFPPDSequences::CbTextChange(Control *, int)
{
   // DEBUG_START;

   Control *ChoiceList  = ESPUI.getControl(EspuiChoiceListElementId);
   Control *TextControl = ESPUI.getControl(EspuiTextEntryElementId);

   // DEBUG_V(String("Choice value: ") + ChoiceList->value);
   // DEBUG_V(String("  Text value: ") + TextControl->value);

   bool DeleteAllowed = true;
   bool UpdateAllowed = true;
   bool CreateAllowed = true;

   do // once
   {
      if (ChoiceList->value.equals(N_default))
      {
         // DEBUG_V("Cant do anything to the default entry")
         DeleteAllowed = false;
         UpdateAllowed = false;
         ESPUI.updateControlValue(EspuiStatusMsgElementId, DefaultTextFieldValue);
      }

      if (-1 != TextControl->value.indexOf(DefaultTextFieldValue))
      {
         // DEBUG_V("User did not remove the default text");
         CreateAllowed = false;
         UpdateAllowed = false;
         ESPUI.updateControlValue(EspuiStatusMsgElementId, emptyString);
         break;
      }

      if (TextControl->value.equals(emptyString))
      {
         // DEBUG_V("Cant use an entry without a name");
         CreateAllowed = false;
         UpdateAllowed = false;
         ESPUI.updateControlValue(EspuiStatusMsgElementId, String(F("A Blank Name Is Not Allowed")));
         break;
      }

      if (TextControl->value.equals(N_default))
      {
         // DEBUG_V("Cant use the default entry text");
         CreateAllowed = false;
         UpdateAllowed = false;
         ESPUI.updateControlValue(EspuiStatusMsgElementId, String(F("Cannot Create Another Default Sequence")));
         break;
      }

      if (TextControl->value.equals(ChoiceList->value))
      {
         // DEBUG_V("No Change in text");
         CreateAllowed = false;
         UpdateAllowed = false;
         ESPUI.updateControlValue(EspuiStatusMsgElementId, emptyString);
         break;
      }

      if (Sequences.end() != Sequences.find(TextControl->value))
      {
         // DEBUG_V("Name exists and it is not the selected name");
         // DEBUG_V(String("  Key: '") + Sequences.find(TextControl->value)->first + "'");

         CreateAllowed = false;
         UpdateAllowed = false;
         ESPUI.updateControlValue(EspuiStatusMsgElementId, String(F("A Sequence With This Name Already Exists")));
         break;
      }

      // DEBUG_V("valid text that could be used for the existing sequence");
      ESPUI.updateControlValue(EspuiStatusMsgElementId, emptyString);

   } while (false);

   ESPUI.getControl(EspuiButtonCreateElementId)->enabled = CreateAllowed;
   ESPUI.getControl(EspuiButtonDeleteElementId)->enabled = DeleteAllowed;
   ESPUI.getControl(EspuiButtonUpdateElementId)->enabled = UpdateAllowed;

   ESPUI.updateControl(EspuiButtonCreateElementId);
   ESPUI.updateControl(EspuiButtonDeleteElementId);
   ESPUI.updateControl(EspuiButtonUpdateElementId);
   ESPUI.updateControl(EspuiTextEntryElementId);
   ESPUI.updateControl(EspuiStatusMsgElementId);
   ESPUI.updateControl(EspuiChoiceListElementId);

   // DEBUG_END;

} // TextChangeCb

// *********************************************************************************************
void c_ControllerFPPDSequences::RestoreConfig(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   // serializeJsonPretty(config, Serial);
   // DEBUG_V();

   if (config.containsKey(N_name))
   {
      Name = (const char *)config[N_name];
   }

   if (!config.containsKey(N_list))
   {
      // DEBUG_V();
      config.createNestedObject(N_list);
   }
   // DEBUG_V();
   JsonObject ListOfSequences = config[N_list];

   for (JsonPair CurrentSequence : ListOfSequences)
   {
      String Key = CurrentSequence.key().c_str();
      // DEBUG_V(String("Setting up: ") + Key);

      JsonObject SequenceConfig = CurrentSequence.value();

      // do we have an entry for this sequence
      if (Sequences.end() == Sequences.find(Key))
      {
         // DEBUG_V(String("Add ") + Key);
         c_ControllerFPPDSequence *Temp = new c_ControllerFPPDSequence();
         Sequences[Key] = *Temp;
         Sequences[Key].SetName(Key);
         delete Temp;
      }
      else
      {
         // DEBUG_V("ERROR: Sequence already exists. Duplicate?");
      }

      // DEBUG_V();
      Sequences[Key].RestoreConfig(SequenceConfig);
   }

   // DEBUG_V("Final");
   // serializeJsonPretty(config, Serial);

   // DEBUG_END;
} // RestoreControllerConfiguration

// *********************************************************************************************
void c_ControllerFPPDSequences::SaveConfig(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   // serializeJsonPretty(config, Serial);

   config[N_name] = Name;

   if (false == config.containsKey(N_list))
   {
      // DEBUG_V("Create List");
      config.createNestedObject(N_list);
   }
   JsonObject SequenceConfigs = config[N_list];

   // DEBUG_V();

   for (auto & CurrentSequence : Sequences)
   {
      if (!SequenceConfigs.containsKey(CurrentSequence.first))
      {
         // DEBUG_V(String("Create Sequence entry") + CurrentSequence.first);
         SequenceConfigs.createNestedObject(CurrentSequence.first);
      }
      JsonObject SequenceConfig = SequenceConfigs[CurrentSequence.first];
      CurrentSequence.second.SaveConfig(SequenceConfig);
   }

   // DEBUG_END;
} // SaveControllerConfiguration

// *********************************************************************************************
// EOF
