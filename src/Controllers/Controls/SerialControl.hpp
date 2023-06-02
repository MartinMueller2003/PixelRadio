#pragma once
/*
  *    File: SerialControl.hpp
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
#include "BaudrateControl.hpp"
#include "StatusControl.hpp"
#include "RBD_SerialManager.h"

class cSerialControl : public cBaudrateControl
{
public:

    cSerialControl ();
    virtual~cSerialControl ();
    virtual void    AddControls (uint16_t TabId, ControlColor color);
    void            initSerialControl (HardwareSerial * port);
    virtual void    SetControllerEnabled (bool value) {ControllerIsEnabled = value;}
    virtual bool    set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate);
    virtual void    poll (void);

private:
    bool                ControllerIsEnabled = false;

    HardwareSerial      * SerialPort = nullptr;
    cStatusControl      LastCmdProcessed;

    RBD::SerialManager  serial_manager;
    cCommandProcessor   CommandProcessor;

    String              cmdStr; // Serial Port Commands from user (CLI).
    String              paramStr;
};                              // class cSerialControl

// *********************************************************************************************
// EOF
