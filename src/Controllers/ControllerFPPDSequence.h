/*
   File: ControllerFPPDSequence.h
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
#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "ControllerMessages.h"

class c_ControllerFPPDSequence
{
public:

    c_ControllerFPPDSequence ();
    virtual ~c_ControllerFPPDSequence ();

    void        RestoreConfig (ArduinoJson::JsonObject &config);
    void        SaveConfig (ArduinoJson::JsonObject &config);
    void        AddControls (uint16_t ctrlTab, uint16_t EspuiSequencesElementId);
    void        SetName (String &value);
    void        Activate (bool value);
    void        SetMessages (c_ControllerMessages * value)
    {
        Messages = value;
    }

    void GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t &Response)
    {
        Messages->GetNextRdsMessage (Response);
    }

private:

    String Name;
    uint16_t EspuiParentElementId       = Control::noParent;
    uint16_t EspuiRootElementId         = Control::noParent;
    uint16_t EspuiElementId             = Control::noParent;
    c_ControllerMessages * Messages     = nullptr;
};      // c_ControllerFPPDSequence

// *********************************************************************************************
// EOF
