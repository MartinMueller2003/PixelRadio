#pragma once
/*
  *    File: FrequencyAdjust.hpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  */

// *********************************************************************************************
#include <Arduino.h>
#include "ControlCommonMsg.hpp"
#include "FrequencyStatus.hpp"

// *********************************************************************************************
class cFrequencyAdjust : public cControlCommonMsg
{
public:

    cFrequencyAdjust ();
    virtual~cFrequencyAdjust ()    {}

    void    AddControls (uint16_t TabId, ControlColor color);
    void    AddAdjustControls (uint16_t TabId, ControlColor color);
    void    AddRadioControls (uint16_t TabId, ControlColor color);
    void    AddHomeControls (uint16_t TabId, ControlColor color);
    void    Callback (Control * sender, int type);
    bool    set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate);
    bool    validate (const String & value, String & ResponseMessage, bool ForceUpdate);

private:

    void UpdateStatus (bool SkipLogOutput, bool ForceUpdate);

    cFrequencyStatus    HomeFreqStatus;
    cFrequencyStatus    AdjustFreqStatus;
    cFrequencyStatus    RadioFreqStatus;
};  // class cFrequencyAdjust

extern cFrequencyAdjust FrequencyAdjust;

// *********************************************************************************************
// OEF
