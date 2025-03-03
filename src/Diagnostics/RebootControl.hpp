#pragma once
/*
  *    File: RebootControl.hpp
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
#include "ButtonControl.hpp"

// *********************************************************************************************
class cRebootControl : public cButtonControl
{
public:

    cRebootControl ();
    virtual~cRebootControl ()    {}

    void    AddControls (uint16_t TabId, ControlColor color);
    void    Callback (Control * sender, int type);
    void    Poll ();

private:
    #define FiveSecondsInMs (5 * 1000)
    volatile uint32_t ButtonEndTime = 0;
    volatile bool       WarningSent = false;
    volatile uint32_t   RebootTime  = 0;
};  // class cRebootControl

// *********************************************************************************************
extern cRebootControl RebootControl;

// *********************************************************************************************
// OEF
