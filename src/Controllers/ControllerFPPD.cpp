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
void c_ControllerFPPD::GetNextRdsMessage (const String &, c_ControllerMgr::RdsMsgInfo_t & Response)
{
    // DEBUG_START;

    do // once
    {
        struct timeval tv;
        gettimeofday (& tv, NULL);
        if(BlankTime <= tv.tv_sec)
        {
            // DEBUG_V("need to stop displaying, No FPPD messages are coming in");
            break;
        }

        // DEBUG_V(String("CurrentPlayingSequenceName: '") + CurrentPlayingSequenceName + "'");

        if (!CurrentPlayingSequenceName.isEmpty ())
        {
            // DEBUG_V("Get next message");
            Sequences.GetNextRdsMessage (CurrentPlayingSequenceName, Response);
        }
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerFPPD::ProcessFppdFile (String & FppdFileName)
{
    // DEBUG_START;

    // normalize the name
    String TempName = FppdFileName;
    TempName.toLowerCase();
    String FinalName = FppdFileName.substring(0, TempName.lastIndexOf(".fseq"));

    struct timeval tv;
    gettimeofday (& tv, NULL);
    BlankTime = tv.tv_sec + BLANK_DELAY;

    // DEBUG_V(String("BlankTime: '") + String(BlankTime) + "'");
    // DEBUG_V(String("FppdFileName: '") + FppdFileName + "'");
    // DEBUG_V(String("   FinalName: '") + FinalName + "'");
    if (!FinalName.equals (CurrentPlayingSequenceName))
    {
        // DEBUG_V(String("New File: '") + FinalName + "'");
        CurrentPlayingSequenceName = FinalName;

        if (CurrentPlayingSequenceName.isEmpty ())
        {
            // DEBUG_V("No sequence playing");
            CurrentSequence.setMessage (F ("No Sequence Playing"), eCssStyle::CssStyleTransparent);
        }
        else
        {
            CurrentSequence.setMessage (CurrentPlayingSequenceName, eCssStyle::CssStyleWhite);

            // DEBUG_V(String("SequenceLearningEnabled: ") + String(SequenceLearning.getBool ()));
            if (SequenceLearning.getBool ())
            {
                // DEBUG_V("Learn Message");
                Sequences.LearnSequenceName (CurrentPlayingSequenceName);
            }
            else
            {
                // DEBUG_V("Not allowed to Learn Message");
            }
        }
    }
    else
    {
        // DEBUG_V("Message Already in progress");
    }

    // DEBUG_END;
}   // ProcessFppdFile

// *********************************************************************************************
void c_ControllerFPPD::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    // config["ControllerEnabled"] = true;
    // Serial.println("c_ControllerFPPD::restoreConfiguration");
    // serializeJsonPretty(config, Serial);
    // Serial.println("\nc_ControllerFPPD::restoreConfiguration");

    cControllerCommon::restoreConfiguration (config);
    SequenceLearning.restoreConfiguration (config);
    Sequences.RestoreConfig (config);

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
