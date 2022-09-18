/*
  *    File: ControllerCommon.h
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
  *
  *    Pure virtual base class to define the controller API and provide common functionality.
  */

// *********************************************************************************************
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPUI.h>

#include "BinaryControl.hpp"
#include "ControllerMgr.h"

class cControllerCommon : public cBinaryControl
{
private:

    CtypeId TypeId = NullControllerId;
    String  SeperatorName;

public:
    cControllerCommon (const String & MyName, CtypeId MyId);
    virtual~cControllerCommon ();

    virtual void    begin ()    {}
    virtual void    poll ()     {}
    virtual void    AddControls (uint16_t tabId, ControlColor color);
    String          GetName () {return GetTitle ();}
    virtual void    saveConfiguration (ArduinoJson::JsonObject & config);

    virtual void    GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response) = 0;
    bool            ControllerIsEnabled () {return getBool ();}
};  // class cControllerCommon

// *********************************************************************************************
// EOF
