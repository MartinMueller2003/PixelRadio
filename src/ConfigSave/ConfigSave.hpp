#pragma once
/*
   File: ConfigSave.cpp
   Project: PixelConfigSave, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelConfigSave.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include "ESPUI.h"

// *********************************************************************************************
class cConfigSave
{
public:

    cConfigSave ()
    {
    }

    virtual ~cConfigSave ()
    {
    }

    void        AddControls (uint16_t adjTab, ControlColor color);
    void        ClearSaveNeeded ();
    void        InitiateSaveOperation ();
    void        SetSaveNeeded ();
};

extern cConfigSave  ConfigSave;

// *********************************************************************************************
// OEF
