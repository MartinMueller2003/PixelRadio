#pragma once
/*
  *    File: ControllerFPPDSequences.h
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
#include <ArduinoJson.h>
#include <ESPUI.h>
#include <map>

#include "ControllerFPPDSequence.h"
#include "Language.h"

class c_ControllerFPPDSequences
{
public:

    c_ControllerFPPDSequences ();
    virtual~c_ControllerFPPDSequences ();
    void    RestoreConfig (ArduinoJson::JsonObject & config);
    void    SaveConfig (ArduinoJson::JsonObject & config);

    void    AddControls (uint16_t ctrlTab, ControlColor color);
    void    begin ();
    void    CbButtonCreate (Control * sender, int type);
    void    CbButtonDelete (Control * sender, int type);
    void    CbButtonUpdate (Control * sender, int type);
    void    CbChoiceList (Control * sender, int type);
    void    CbTextChange (Control * sender, int type);
    bool    GetNextRdsMessage (const String & value, c_ControllerMgr::RdsMsgInfo_t & Response) {return ControllerMessages.GetNextRdsMessage (value, Response);}
    void    LearnSequenceName (String & value);

private:

    void    Activate ();
    void    AddSequence (String & SequenceName);

    String                                      SelectedSequenceName = N_default;

    uint16_t                                    EspuiParentElementId            = Control::noParent;
    uint16_t                                    EspuiChoiceListElementId        = Control::noParent;
    uint16_t                                    EspuiStatusMsgElementId         = Control::noParent;
    uint16_t                                    EspuiInstructionMsgElementId    = Control::noParent;
    uint16_t                                    EspuiTextEntryElementId         = Control::noParent;
    uint16_t                                    EspuiButtonCreateElementId      = Control::noParent;
    uint16_t                                    EspuiButtonDeleteElementId      = Control::noParent;
    uint16_t                                    EspuiButtonUpdateElementId      = Control::noParent;

    std::map <String, c_ControllerFPPDSequence> Sequences;
    c_ControllerMessages                        ControllerMessages;
};  // c_ControllerFPPDSequences

// *********************************************************************************************
// EOF
