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
#include "OldControlCommon.hpp"
#include <Arduino.h>

// *********************************************************************************************
class cAudioInputImpedance : public cOldControlCommon
{
public:

    cAudioInputImpedance ();
    virtual ~cAudioInputImpedance ()    {}

    void        AddControls (uint16_t Tab, ControlColor color);
    bool        set (String &, String &);
};      // class cAudioInputImpedance

extern cAudioInputImpedance AudioInputImpedance;

// *********************************************************************************************
// OEF
