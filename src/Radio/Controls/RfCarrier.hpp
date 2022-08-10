#pragma once
/*
   File: RfCarrier.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <ESPUI.h>

// *********************************************************************************************
class cRfCarrier
{
public:
                cRfCarrier ();
    virtual     ~cRfCarrier() {}

    void        AddControls (uint16_t TabId);
    void        AddHomeControls (uint16_t TabId);

    bool        get() { return Mode; }
    void        restoreConfiguration(JsonObject &json);
    void        saveConfiguration (JsonObject & json);
    void        set(bool value);
    
// Callbacks need to be public 
    void        Callback(Control *sender, int type);
    
private:
    void    UpdateStatus();

    uint16_t    TabId           = Control::noParent;
    uint16_t    HomeId          = Control::noParent;
    uint16_t    ControlId       = Control::noParent;
    uint16_t    MessageID       = Control::noParent;
    uint16_t    HomeMessageID   = Control::noParent;

    bool        Mode = false;
};

extern cRfCarrier RfCarrier;

// *********************************************************************************************
// OEF
