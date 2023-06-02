/*
  *    File: ControllerFPPD.h
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
#pragma once

#include "ControllerCommon.h"
#include "ControllerFPPDSequences.h"

class c_ControllerFPPD : public cControllerCommon
{
public:

    c_ControllerFPPD ();
    virtual~c_ControllerFPPD ();

    void    begin ();
    void    ProcessFppdFile (String & FppdFileName);

    void    AddControls (uint16_t ctrlTab, ControlColor color);
    void    restoreConfiguration (ArduinoJson::JsonObject & config);
    void    saveConfiguration (ArduinoJson::JsonObject & config);
    void    CbSequenceLearningEnabled (Control * sender, int type);
    void    GetNextRdsMessage (const String & value, c_ControllerMgr::RdsMsgInfo_t & Response);

private:

    void updateVisibility ();

    uint16_t                    SequencesElementId          = Control::noParent;
    uint16_t                    CurrentSequenceElementId    = Control::noParent;

    bool                        ControlsHaveBeenAdded = false;
    String                      CurrentPlayingSequenceName;

    c_ControllerFPPDSequences   Sequences;
};  // c_ControllerFPPD

// *********************************************************************************************
// EOF
