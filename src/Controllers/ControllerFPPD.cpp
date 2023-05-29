/*
  *    File: ControllerFPPD.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.0
  *    Creation: Dec-16-2021
  *    Revised:  Apr-06-2022
  *    Public Release:
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *    Revision History: See PixelRadio.cpp
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
  *    absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *     Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
  */

// *********************************************************************************************
#include <Arduino.h>
#include "ControllerFPPD.h"
#include "SequenceLearning.hpp"
#include "CurrentSequence.hpp"
#include "FPPDiscovery.h"
#include "language.h"

#include "memdebug.h"

static const PROGMEM char Name [] = "FPPD";

// *********************************************************************************************
c_ControllerFPPD::c_ControllerFPPD () :   cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::FPPD_CNTRL)
{}  // c_ControllerFPPD

// *********************************************************************************************
c_ControllerFPPD::~c_ControllerFPPD ()
{}

// ************************************************************************************************
void c_ControllerFPPD::AddControls (uint16_t ctrlTab, ControlColor color)
{
    // DEBUG_START;

    cControllerCommon::AddControls (ctrlTab, color);
    SequenceLearning.AddControls (ctrlTab, color);
    CurrentSequence.AddControls (SequenceLearning.ControlId, color);
    Sequences.AddControls (ctrlTab, color);

    // DEBUG_END;
}   // AddControls

// *********************************************************************************************
void c_ControllerFPPD::begin ()
{
    // DEBUG_START;

    Sequences.begin ();

    FPPDiscovery.begin (
        [] (String & FppdFileName, void * param)
        {
            if (param)
            {
                reinterpret_cast <c_ControllerFPPD *> (param)->ProcessFppdFile (FppdFileName);
            }
        },
        this);

    // DEBUG_END;
}   // begin

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

        if (CurrentPlayingSequence.isEmpty ())
        {
            CurrentSequence.setMessage (F ("No Sequence Playing"), eCssStyle::CssStyleTransparent);
        }
        else
        {
            CurrentSequence.setMessage (CurrentPlayingSequence, eCssStyle::CssStyleWhite);

            // DEBUG_V(String("SequenceLearningEnabled: ") + String(SequenceLearningEnabled));
            if (SequenceLearning.getBool ())
            {
                Sequences.AddSequence (CurrentPlayingSequence);
            }
        }
    }

    // DEBUG_END;
}   // ProcessFppdFile

// *********************************************************************************************
void c_ControllerFPPD::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::restoreConfiguration (config);
    SequenceLearning.restoreConfiguration (config);
    Sequences.RestoreConfig (config);

    // DEBUG_V("Final");
    // serializeJsonPretty(config, Serial);

    // DEBUG_END;
}   // restoreConfiguration

// *********************************************************************************************
void c_ControllerFPPD::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::saveConfiguration (config);
    SequenceLearning.saveConfiguration (config);
    Sequences.SaveConfig (config);

    // DEBUG_V("Final");
    // serializeJsonPretty(config, Serial);

    // DEBUG_END;
}   // saveConfiguration

// *********************************************************************************************
// EOF
