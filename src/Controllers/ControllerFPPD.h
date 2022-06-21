/*
   File: ControllerFPPD.h
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
#include "ControllerCommon.h"
#include "ControllerFPPDSequences.h"

class c_ControllerFPPD : public c_ControllerCommon
{
public:
            c_ControllerFPPD();
   virtual  ~c_ControllerFPPD();

   void     AddControls(uint16_t ctrlTab);
   void     RestoreConfiguration(ArduinoJson::JsonObject & config);
   void     SaveConfiguration(ArduinoJson::JsonObject & config);
   void     ControllerEnabledCb(Control *sender, int type);
   void     SequenceLearningEnabledCb(Control *sender, int type);

   void     begin();

private:
   
   void     updateVisibility();

   uint16_t SequenceLearningEnabledElementId       = Control::noParent;
   uint16_t SequenceLearningLabelEnabledElementId  = Control::noParent;
   uint16_t EspuiSequencesElementId                = Control::noParent;
   bool     SequenceLearningEnabled                = true;
   uint16_t MaxIdleTimeSec                         = 10;

   c_ControllerFPPDSequences Sequences;

}; // c_ControllerFPPD

// *********************************************************************************************
// EOF
