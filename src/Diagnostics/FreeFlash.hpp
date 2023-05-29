#pragma once
/*
  *    File: FreeFlash.cpp
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
#include <esp_adc_cal.h>

#include "StatusControl.hpp"

// *********************************************************************************************
class cFreeFlash : public cStatusControl
{
public:

    cFreeFlash ();
    virtual~cFreeFlash () {}

    void    AddControls (uint16_t TabId, ControlColor color);
    void    Poll ();

private:

    const int32_t   MeasurementIntervalMs   = 3000; // Measurement Refresh Time, in mS.
    uint32_t        NextReadingTimeMs       = MeasurementIntervalMs;
    uint32_t        PreviousReading         = 0;
};  // class cFreeFlash

extern cFreeFlash FreeFlash;

// *********************************************************************************************
// OEF
