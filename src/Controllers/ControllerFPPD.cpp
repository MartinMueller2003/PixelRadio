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
#include "language.h"

#if __has_include ("memdebug.h")
 # include "memdebug.h"
#endif //  __has_include("memdebug.h")

static const String DefaultSequenceMsg = "No Sequence";
static const PROGMEM String Name = F("FPPD");

// *********************************************************************************************
c_ControllerFPPD::c_ControllerFPPD () :   cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::FPPD_CNTRL)
{}      // c_ControllerFPPD

// *********************************************************************************************
c_ControllerFPPD::~c_ControllerFPPD ()
{}

// ************************************************************************************************
void c_ControllerFPPD::AddControls (uint16_t ctrlTab, ControlColor color)
{
    // DEBUG_START;

    cControllerCommon::AddControls (ctrlTab, color);
/*
    uint16_t LabelId;

    LabelId = ESPUI.addControl (ControlType::Label,
                                "Sequence Learning",
                                "Sequence Learning",
                                ControlColor::Turquoise,
                                ControlLabelElementId);
    ESPUI.setElementStyle (LabelId, CSS_LABEL_STYLE_BLACK);

    SequenceLearningEnabledElementId = ESPUI.addControl (ControlType::Switcher,
                                                         "Sequence Learning",
                                                         String (SequenceLearningEnabled ? F ("1") : F ("0")),
                                                         ControlColor::Turquoise,
                                                         ControlLabelElementId,
                                                         [] (Control * sender, int type, void * param)
                                                         {
                                                             if (param)
                                                             {
                                                                 reinterpret_cast <c_ControllerFPPD *> (param)->CbSequenceLearningEnabled (
                                                                     sender, type);
                                                             }
                                                         },
                                                         this);

    LabelId = ESPUI.addControl (ControlType::Label,
                                emptyString.c_str (),
                                "Cuurent Sequence",     // String("Current Sequence"),
                                ControlColor::Turquoise,
                                ControlLabelElementId);
    ESPUI.      setElementStyle (LabelId, CSS_LABEL_STYLE_BLACK);

    CurrentSequenceElementId = ESPUI.addControl (ControlType::Label,
                                                 emptyString.c_str (),
                                                 DefaultSequenceMsg,
                                                 ControlColor::Turquoise,
                                                 ControlLabelElementId);
    ESPUI.      setElementStyle (CurrentSequenceElementId, CSS_LABEL_STYLE_WHITE);

    Sequences.AddControls (EspuiParentElementId);

    ControlsHaveBeenAdded = true;
*/
    // DEBUG_END;
}       // AddControls

// *********************************************************************************************
void c_ControllerFPPD::begin ()
{
    // DEBUG_START;

    Sequences.begin ();

    FPPDiscovery.begin ([] (String & FppdFileName, void * param)
                        {
                            if (param)
                            {
                                reinterpret_cast <c_ControllerFPPD *> (param)->ProcessFppdFile (FppdFileName);
                            }
                        },
                        this);

    // DEBUG_END;
}       // begin

// ************************************************************************************************
void c_ControllerFPPD::CbSequenceLearningEnabled (Control * sender, int type)
{
    // DEBUG_START;

    SequenceLearningEnabled = (S_ACTIVE == type);

    displaySaveWarning ();
    Log.infoln ((String (F ("FPPD Controller Sequence Learning Set to: ")) + String (SequenceLearningEnabled ? "On" : "Off")).c_str ());

    // DEBUG_END;
}       // SequenceLearningEnabledCb

// *********************************************************************************************
void c_ControllerFPPD::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response)
{
    // DEBUG_START;

    if (!CurrentPlayingSequence.isEmpty ())
    {
        Sequences.GetNextRdsMessage (Response);
    }
    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerFPPD::ProcessFppdFile (String & FppdFileName)
{
    // DEBUG_START;

    // DEBUG_V(String("FppdFileName: '") + FppdFileName + "'");
    if (!FppdFileName.equals (CurrentPlayingSequence))
    {
        // DEBUG_V(String("New File: '") + FppdFileName + "'");
        CurrentPlayingSequence = FppdFileName;
        Control * control = ESPUI.getControl (CurrentSequenceElementId);

        if (control)
        {
            if (CurrentPlayingSequence.isEmpty ())
            {
                ESPUI.updateControlValue (control, DefaultSequenceMsg);
            }
            else
            {
                ESPUI.updateControlValue (control, CurrentPlayingSequence);

                // DEBUG_V(String("SequenceLearningEnabled: ") + String(SequenceLearningEnabled));
                if (SequenceLearningEnabled)
                {
                    Sequences.AddSequence (CurrentPlayingSequence);
                }
            }
        }
    }
    // DEBUG_END;
}       // ProcessFppdFile

// *********************************************************************************************
void c_ControllerFPPD::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::restoreConfiguration (config);

    if (config.containsKey (N_SequenceLearningEnabled))
    {
        SequenceLearningEnabled = config[N_SequenceLearningEnabled];
        // DEBUG_V(String("SequenceLearningEnabled: ") + String(SequenceLearningEnabled));
    }

    if (config.containsKey (N_MaxIdleSec))
    {
        MaxIdleTimeSec = config[N_MaxIdleSec];
        // DEBUG_V(String("MaxIdleTimeSec: ") + String(MaxIdleTimeSec));
    }
    Sequences.RestoreConfig (config);

    // DEBUG_V("Final");
    // serializeJsonPretty(config, Serial);

    // DEBUG_END;
}       // restoreConfiguration

// *********************************************************************************************
void c_ControllerFPPD::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::saveConfiguration (config);

    config[N_MaxIdleSec]                = MaxIdleTimeSec;
    config[N_SequenceLearningEnabled]   = SequenceLearningEnabled;

    Sequences.SaveConfig (config);

    // DEBUG_V("Final");
    // serializeJsonPretty(config, Serial);

    // DEBUG_END;
}       // saveConfiguration

// *********************************************************************************************
// EOF
