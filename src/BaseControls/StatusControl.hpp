#pragma once
/*
  *    File: StatusControl.hpp
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

// *********************************************************************************************
class cStatusControl : public cControlCommonMsg
{
public:

    cStatusControl (const String & Title);
    cStatusControl ();
    virtual~cStatusControl ()    {}

    void    restoreConfiguration (JsonObject &) {}
    void    saveConfiguration (JsonObject &)    {}

    virtual bool    set (const String & value, bool SkipLogOutput, bool ForceUpdate);
    virtual void    set (const String & value, eCssStyle style, bool SkipLogOutput, bool ForceUpdate);
};  // class cStatusControl

// *********************************************************************************************
// OEF
