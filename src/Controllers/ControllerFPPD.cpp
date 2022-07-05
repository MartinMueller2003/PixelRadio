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

// *********************************************************************************************
c_ControllerFPPD::c_ControllerFPPD() : c_ControllerCommon("FPPD", c_ControllerMgr::ControllerTypeId_t::FPPD_CNTRL)
{
} // c_ControllerFPPD

// *********************************************************************************************
c_ControllerFPPD::~c_ControllerFPPD(){}

// ************************************************************************************************
void c_ControllerFPPD::AddControls(uint16_t ParentElementId)
{
   // DEBUG_START;

   ESPUI.addControl(
       ControlType::Separator,
       "FPPD CONTROL SETTINGS",
       emptyString,
       ControlColor::None,
       ParentElementId);

   ControlerEnabledElementId = ESPUI.addControl(
      ControlType::Switcher,
      "FPPD CONTROL",
      ControllerEnabled ? "1" : "0",
      ControlColor::Turquoise,
      ParentElementId,
      [](Control *sender, int type, void *param)
      {
         if(param)
         {
            reinterpret_cast<c_ControllerFPPD*>(param)->CbControllerEnabled(sender, type);
         }
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
         if(param)
         {
            reinterpret_cast<c_ControllerFPPD*>(param)->CbSequenceLearningEnabled(sender, type);
         }
      },
      this);

   Sequences.AddControls(ParentElementId);

   // DEBUG_END;

} // AddControls

// *********************************************************************************************
void c_ControllerFPPD::begin()
{
   // DEBUG_START;
   FPPDiscovery.begin(
      [](String &FppdFileName, void *param)
      {
         if(param)
         {
            reinterpret_cast<c_ControllerFPPD *>(param)->ProcessFppdFile(FppdFileName);
         }
      },
      this);
   // DEBUG_END;
} // begin

// ************************************************************************************************
void c_ControllerFPPD::CbControllerEnabled(Control *sender, int type)
{
   // DEBUG_START;

   ControllerEnabled = (type == S_ACTIVE);

   // DEBUG_V();
   displaySaveWarning();
   displayRdsText(); // Update RDS RadioText.
   Log.infoln((String(F("FPPD Controller Set to: ")) + String(ControllerEnabled ? "On" : "Off")).c_str());

   // DEBUG_END;

} // controllerCallback

// ************************************************************************************************
void c_ControllerFPPD::CbSequenceLearningEnabled(Control *sender, int type)
{
   // DEBUG_START;

   SequenceLearningEnabled = (S_ACTIVE == type);

   displaySaveWarning();
   displayRdsText(); // Update RDS RadioText.
   Log.infoln((String(F("FPPD Controller Sequence Learning Set to: ")) + String(SequenceLearningEnabled ? "On" : "Off")).c_str());

   // DEBUG_END;

} // SequenceLearningEnabledCb

// *********************************************************************************************
void c_ControllerFPPD::ProcessFppdFile(String & FppdFileName)
{
   // DEBUG_START;

   // DEBUG_V(String("FppdFileName: '") + FppdFileName + "'");
   if (!FppdFileName.equals(CurrentPlayingSequence))
   {
      DEBUG_V(String("New File: '") + FppdFileName + "'");
      CurrentPlayingSequence = FppdFileName;
   }

   // DEBUG_END;
} // ProcessFppdFile

// *********************************************************************************************
void c_ControllerFPPD::RestoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::RestoreConfiguration(config);

   if (config.containsKey(N_SequenceLearningEnabled))
   {
      SequenceLearningEnabled = config[N_SequenceLearningEnabled];
   }

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
   config[N_SequenceLearningEnabled] = SequenceLearningEnabled;

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

// *********************************************************************************************
// EOF
