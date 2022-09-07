#pragma once
/*
   File: VoltageStatus.cpp
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
#include <esp_adc_cal.h>

#include "StatusControl.hpp"

// *********************************************************************************************
class cVoltageStatus : public cStatusControl
{
public:

    cVoltageStatus (String Title, adc1_channel_t ADC_PORT);
    virtual ~cVoltageStatus () {}

    void    AddControls (uint16_t TabId, ControlColor color);
    void    Poll ();

private:
    void    initVdcAdc (void);
    float   measureVoltage (void);

    const int32_t   MeasurementIntervalMs   = 1000;   // Measurement Refresh Time, in mS.
    uint32_t        NextReadingTimeMs       = MeasurementIntervalMs;

    const uint32_t  DEFAULT_VREF    = 1100;
    const float     SCALE           = ((100000.0 + 100000.0) / 100000.0);   // Onboard ESP32 Resistor Attenuator on "VBAT" ADC Port.

    adc1_channel_t  ADC_PORT        = ADC1_CHANNEL_7;                       // GPIO-35, Onboard ESP32 "VBAT" Voltage.

    #define NumberOfReadingsToSave   16
    int32_t ArrayOfVoltageReadings[NumberOfReadingsToSave];
    uint32_t CurrentReadingIndex    = 0;    // Index of the current voltage reading
    uint32_t SumOfVoltages          = 0;    // average voltage

    esp_adc_cal_characteristics_t * adc_chars = nullptr;

};  // class cVoltageStatus

// *********************************************************************************************
// OEF
