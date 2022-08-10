#pragma once
/*
   File: AnalogAudioGain.cpp
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
class cAnalogAudioGain
{
public:
                cAnalogAudioGain ();
    virtual     ~cAnalogAudioGain() {}

    void        AddControls (uint16_t Tab);

    uint8_t     get() { return vgaGainValue; }

    void        restoreConfiguration(JsonObject &json);
    void        saveConfiguration (JsonObject & json);
    void        set(String & value);
    
// Callbacks need to be public 
    void        Callback(Control *sender, int type);
    
private:
    uint16_t    TabId       = Control::noParent;
    uint16_t    ControlId   = Control::noParent;

    String      vgaGainStr;
    uint8_t     vgaGainValue;
};

extern cAnalogAudioGain AnalogAudioGain;

// *********************************************************************************************
// OEF
