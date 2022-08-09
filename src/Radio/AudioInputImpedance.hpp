#pragma once
/*
   File: AudioInputImpedance.cpp
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
class cAudioInputImpedance
{
public:
                cAudioInputImpedance ();
    virtual     ~cAudioInputImpedance() {}

    void        AddRadioControls (uint16_t Tab);

    String &    get() { return InputImpedanceStr; }
    void        restoreConfiguration(JsonObject &json);
    void        saveConfiguration (JsonObject & json);
    void        set(String & value);
    
// Callbacks need to be public 
    void        CbImpedanceAdjust(Control *sender, int type);
    
private:
    uint16_t    radioTab    = Control::noParent;
    uint16_t    SelectID    = Control::noParent;

    String      InputImpedanceStr;
    uint8_t     InputImpedanceValue = 0;
};

// *********************************************************************************************
// OEF
