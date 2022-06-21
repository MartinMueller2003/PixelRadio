/*
   File: ControllerFPPDSequences.h
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

#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include "ControllerFPPDSequence.h"
#include <ESPUI.h>

class c_ControllerFPPDSequences
{
public:
            c_ControllerFPPDSequences();
   virtual  ~c_ControllerFPPDSequences();
   void     RestoreConfig(ArduinoJson::JsonObject & config);
   void     SaveConfig(ArduinoJson::JsonObject & config);

   void     AddControls(uint16_t ctrlTab);
   void     ChoiceListCb(Control *sender, int type);
   void     TextChangeCb(Control *sender, int type);
   void     ButtonCreateCb(Control *sender, int type);
   void     ButtonDeleteCb(Control *sender, int type);
   void     ButtonUpdateCb(Control *sender, int type);

private:
   void     AddTestObjects(ArduinoJson::JsonObject &config);
   void     Activate();

   String   Name                          = F("FPPD Sequences");
   String   DefaultTextFieldValue         = F("Type New Sequence Name Here");
   uint16_t EspuiParentElementId          = Control::noParent;
   uint16_t EspuiChoiceListElementId      = Control::noParent;
   uint16_t EspuiStatusMsgElementId       = Control::noParent;
   uint16_t EspuiTextEntryElementId       = Control::noParent;
   uint16_t EspuiButtonCreateElementId    = Control::noParent;
   uint16_t EspuiButtonDeleteElementId    = Control::noParent;
   uint16_t EspuiButtonUpdateElementId    = Control::noParent;

   std::map<String, c_ControllerFPPDSequence>Sequences;

}; // c_ControllerFPPDSequences

// *********************************************************************************************
// EOF
