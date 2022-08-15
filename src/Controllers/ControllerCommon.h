/*
   File: ControllerCommon.h
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

   Pure virtual base class to define the controller API and provide common functionality.
 */

// *********************************************************************************************
#pragma once

#include "Arduino.h"
#include "PixelRadio.h"
#include <ESPUI.h>
#include <ArduinoJson.h>
#include "ControllerMgr.h"
#include "ControllerMessages.h"

class c_ControllerCommon
{
private:

    String ControlName;
    String SeperatorName;

protected:

    // values stored in configuration file
    bool ControllerEnabled = false;

    // operational data
    String Name;        // Name of this controller
    CtypeId TypeId = NullControllerId;

    // ESPUI control IDs
    uint16_t homeTab                    = Control::noParent;
    uint16_t EspuiParentElementId       = Control::noParent;
    uint16_t ControlLabelElementId      = Control::noParent;
    uint16_t ControlerEnabledElementId  = Control::noParent;
    uint16_t EspuiMsgId                 = Control::noParent;

public:

    c_ControllerCommon (String MyName, c_ControllerMgr::ControllerTypeId_t MyId);
    virtual           ~c_ControllerCommon ();
    virtual void begin ()
    {
    }

    virtual void poll ()
    {
    }

    virtual void        AddControls (uint16_t tabId);
    void                CbControllerEnabled (Control * sender, int type);
    virtual void        restoreConfiguration (ArduinoJson::JsonObject &config);
    virtual void        saveConfiguration (ArduinoJson::JsonObject &config);
    String&             GetName ()
    {
        return Name;
    }

    virtual void        GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t &Response) = 0;
    bool                ControllerIsEnabled ()
    {
        return ControllerEnabled;
    }
};      // c_ControllerCommon

// *********************************************************************************************
// EOF
