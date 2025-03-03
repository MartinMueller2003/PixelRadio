#pragma once
/*
  *    File: ControllerGpioSERIAL.hpp
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

#include "CommandProcessor.hpp"
#include "SerialControl.hpp"
#include "ControllerCommon.h"
#include "ControllerMessages.h"
#include "language.h"
#include "RBD_SerialManager.h"

class cControllerGpioSERIAL : public cControllerCommon
{
public:

    cControllerGpioSERIAL ();
    virtual~cControllerGpioSERIAL ();

    void    AddControls (uint16_t TabId, ControlColor color);
    bool    GetNextRdsMessage (const String & value, c_ControllerMgr::RdsMsgInfo_t & Response);
    void    poll () {SerialControl.poll ();}
    void    restoreConfiguration (ArduinoJson::JsonObject & config);
    void    saveConfiguration (ArduinoJson::JsonObject & config);
    bool    set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate);

private:
    cSerialControl          SerialControl;
    c_ControllerMessages    Messages;
};  // cControllerGpioSERIAL

// *********************************************************************************************
// EOF
