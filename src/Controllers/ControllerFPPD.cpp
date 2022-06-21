/*
   File: ControllerFPPD.cpp
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
#include "ControllerFPPD.h"
#include "FPPDiscovery.h"
#include "../language.h"

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

static String EmptyString = "";

// *********************************************************************************************
c_ControllerFPPD::c_ControllerFPPD() : c_ControllerCommon("FPPD", c_ControllerMgr::ControllerTypeId_t::FPPD_CNTRL)
{
} // c_ControllerFPPD

// *********************************************************************************************
c_ControllerFPPD::~c_ControllerFPPD(){}

// ************************************************************************************************
void c_ControllerFPPD::AddControls(uint16_t ctrlTab)
{
   // DEBUG_START;

   ESPUI.addControl(
       ControlType::Separator,
       "FPPD CONTROL SETTINGS",
       EmptyString,
       ControlColor::None,
       ctrlTab);

   ControlerEnabledElementId = ESPUI.addControl(
      ControlType::Switcher,
      "FPPD CONTROL",
      ControllerEnabled ? "1" : "0",
      ControlColor::Turquoise,
      ctrlTab,
      [](Control *sender, int type, void *param)
      {
         reinterpret_cast<c_ControllerFPPD*>(param)->ControllerEnabledCb(sender, type);
      },
      this);

   uint16_t LabelId = ESPUI.addControl(
      ControlType::Label,
      "Sequence Learning",
      "Sequence Learning",
      ControlColor::Turquoise,
      ControlerEnabledElementId);
   ESPUI.setElementStyle(LabelId, CSS_LABEL_STYLE_BLACK);

   SequenceLearningEnabledElementId = ESPUI.addControl(
      ControlType::Switcher,
      "Sequence Learning",
      SequenceLearningEnabled ? "1" : "0",
      ControlColor::Turquoise,
      ControlerEnabledElementId,
      [](Control *sender, int type, void *param)
      {
         reinterpret_cast<c_ControllerFPPD*>(param)->SequenceLearningEnabledCb(sender, type);
      },
      this);

   Sequences.AddControls(ctrlTab);

   // DEBUG_END;

} // AddControls

// *********************************************************************************************
void c_ControllerFPPD::begin()
{
   // DEBUG_START;
   FPPDiscovery.begin();
   // DEBUG_END;
} // begin

// ************************************************************************************************
void c_ControllerFPPD::ControllerEnabledCb(Control *sender, int type)
{
   // DEBUG_START;
   String tempStr;
   if (sender->id == ControlerEnabledElementId)
   {
      // DEBUG_V();
      if (type == S_ACTIVE)
      {
         // DEBUG_V(S_ACTIVE);
         ControllerEnabled = true;
      }
      else if (type == S_INACTIVE)
      {
         // DEBUG_V(S_INACTIVE);
         ControllerEnabled = false; // Must set flag BEFORE mqqtReconnect!
      }
      displaySaveWarning();
      tempStr = F("FPPD Controller Set to: ");
      tempStr += ControllerEnabled ? "On" : "Off";
      // DEBUG_V();
   }
   else
   {
      tempStr = String("controllerCallback: ") + BAD_SENDER_STR;
      // DEBUG_V();
   }

   // DEBUG_V();
   displayRdsText(); // Update RDS RadioText.
   Log.infoln(tempStr.c_str());

   // DEBUG_END;

} // controllerCallback

// *********************************************************************************************
void c_ControllerFPPD::RestoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::RestoreConfiguration(config);

   if (config.containsKey(N_MaxIdleSec))
   {
      MaxIdleTimeSec = config[N_MaxIdleSec];
   }

   if (!config.containsKey(N_sequences))
   {
      JsonObject SequencesConfig = config.createNestedObject(N_sequences);
      SequencesConfig[N_name] = N_sequences;
   }
   JsonObject SequencesConfig = config[N_sequences];
   Sequences.RestoreConfig(SequencesConfig);

   // DEBUG_V("Final");
   // serializeJsonPretty(config, Serial);

   // DEBUG_END;
} // RestoreConfiguration

// *********************************************************************************************
void c_ControllerFPPD::SaveConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::SaveConfiguration(config);

   config[N_MaxIdleSec] = MaxIdleTimeSec;

   if (!config.containsKey(N_sequences))
   {
      config.createNestedObject(N_sequences);
   }

   JsonObject SequencesConfig = config[N_sequences];
   Sequences.SaveConfig(SequencesConfig);

   // DEBUG_V("Final");
   // serializeJsonPretty(config, Serial);

   // DEBUG_END;
} // SaveConfiguration

// ************************************************************************************************
void c_ControllerFPPD::SequenceLearningEnabledCb(Control *sender, int type)
{
   // DEBUG_START;

   SequenceLearningEnabled = (S_ACTIVE == type);

   // DEBUG_END;

} // SequenceLearningEnabledCb

// *********************************************************************************************
// EOF
