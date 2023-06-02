/*
  *    File: ControllerLOCAL.h
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
#include "ControllerMessages.h"

class c_ControllerLOCAL : public cControllerCommon
{
public:

    c_ControllerLOCAL ();
    virtual~c_ControllerLOCAL ();

    void    AddControls (uint16_t ctrlTab, ControlColor color);
    void    restoreConfiguration (ArduinoJson::JsonObject & config);
    void    saveConfiguration (ArduinoJson::JsonObject & config);
    void    CreateDefaultMsgSet ();
    void    GetNextRdsMessage (const String & value, c_ControllerMgr::RdsMsgInfo_t & Response);
    bool    SetRdsText (String & payloadStr, String & ResponseMessage);
    bool    SetRdsTime (String & payloadStr, String & ResponseMessage);

private:

    c_ControllerMessages Messages;
};  // c_ControllerLOCAL

// *********************************************************************************************
// EOF
