/*
   File: ControllerMessage.h
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

#include "ControllerMgr.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <ESPUI.h>

class c_ControllerMessage
{
public:

    struct MessageElementIds_t
    {
        uint16_t        ActiveChoiceListElementId       = Control::noParent;
        uint16_t        HiddenChoiceListElementId       = Control::noParent;
        uint16_t        MessageDetailsElementId         = Control::noParent;
        uint16_t        DisplayDurationElementId        = Control::noParent;
        uint16_t        EnabledElementId                = Control::noParent;
    };

    c_ControllerMessage ();
    c_ControllerMessage (const c_ControllerMessage & source) {} // Empty Copy constructor

    virtual ~c_ControllerMessage ();
    void        Activate (bool value);
    void        AddControls (MessageElementIds_t * _MessageElementIds);
    void        CbDuration (Control * sender, int type);
    void        CbEnabled (Control * sender, int type);
    uint16_t    GetElementId () {return MessageElementId;}

    void        GetMessage (c_ControllerMgr::RdsMsgInfo_t & Response);
    uint32_t    GetDuration ()  {return DurationSec;}

    bool        IsEnabled ()    {return Enabled;}

    void        RestoreConfig (ArduinoJson::JsonObject config);
    void        SaveConfig (ArduinoJson::JsonObject config);
    void        SelectMessage ();
    void        SetMessage (String & value);
    void        SetFppdMode ();

private:

    uint16_t MessageElementId = Control::noParent;
    String MessageText;
    uint32_t DurationSec                        = 5;
    bool Enabled                                = true;
    MessageElementIds_t * MessageElementIds     = nullptr;
};      // c_ControllerMessage

// *********************************************************************************************
// EOF
