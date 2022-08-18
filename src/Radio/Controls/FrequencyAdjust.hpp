#pragma once
/*
   File: FrequencyAdjust.cpp
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
#include "OldControlCommon.hpp"
#include <Arduino.h>

// *********************************************************************************************
class cFrequencyAdjust : public cOldControlCommon
{
public:

    cFrequencyAdjust ();
    virtual ~cFrequencyAdjust ()    {}

    void        AddControls (uint16_t TabId, ControlColor color) {AddAdjustControls (TabId, color);}
    void        AddAdjustControls (uint16_t TabId, ControlColor color);
    void        AddRadioControls (uint16_t TabId, ControlColor color);
    void        AddHomeControls (uint16_t TabId, ControlColor color);
    bool        set (String &value, String &ResponseMessage);
    void        Callback (Control * sender, int type);

private:

    void        UpdateStatus ();

    uint16_t RadioId    = Control::noParent;
    uint16_t HomeId     = Control::noParent;

    uint16_t HomeStatusID       = Control::noParent;
    uint16_t AdjustStatusID     = Control::noParent;
    uint16_t RadioStatusID      = Control::noParent;
};      // class cFrequencyAdjust

extern cFrequencyAdjust FrequencyAdjust;

// *********************************************************************************************
// OEF
