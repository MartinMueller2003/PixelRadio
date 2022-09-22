/*
  *    File: VoltageStatus.cpp
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
#include <ArduinoLog.h>

#include "VoltageStatus.hpp"
#include "memdebug.h"

// *********************************************************************************************
cVoltageStatus::cVoltageStatus (String Title, adc1_channel_t _ADC_PORT, float _SCALE) :
    ADC_PORT (_ADC_PORT),
    SCALE (_SCALE),
    cStatusControl (Title)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cVoltageStatus::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cStatusControl::AddControls (TabId, color);
    setControlPanelStyle (ePanelStyle::PanelStyle135_black);
    initVdcAdc ();

    // DEBUG_END;
}

// *********************************************************************************************
// initVdcAdc(): Initialize the two ADC used in PixelRadio.
// (1) ESP32's VBAT ADC. We use ADC1_CHANNEL_7, which is GPIO 35.
// (2) ESP32's VDC ADC. We use ADC1_CHANNEL_0, which is GPIO 36.
// This MUST be called in setup() before first use of measureVbatVoltage().
void cVoltageStatus::initVdcAdc (void)
{
    // DEBUG_START;

    // Configure ADC
    adc1_config_width (ADC_WIDTH_BIT_12);
    adc1_config_channel_atten (ADC_PORT, ADC_ATTEN_DB_11);

    // Characterize ADC
    adc_chars = (esp_adc_cal_characteristics_t *)calloc (1, sizeof (esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize (ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        Log.infoln ("ADC eFuse provided Factory Stored Vref Calibration.");  // Best Accuracy.
    }
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        Log.infoln ("ADC eFuse provided Factory Stored Two Point Calibration.");    // Good Accuracy.
    }
    else
    {
        Log.infoln ("ADC eFuse not supported, using Default VRef (1100mV).");   // Low Quality Accuracy.
    }

    // make sure the poll routine fires right away
    NextReadingTimeMs = millis () - MeasurementIntervalMs;

    // do a reading
    measureVoltage ();

    uint32_t SingleReadingValue = SumOfVoltages;
    SumOfVoltages = 0;

    for (auto & CurrentEntry : ArrayOfVoltageReadings)
    {
        CurrentEntry    = SingleReadingValue;
        SumOfVoltages   += SingleReadingValue;
    }

    // DEBUG_END;
}

// *********************************************************************************************
// measureVbatVoltage(): Measure ESP32's Onboard "VBAT" Voltage using data averaging.
// Be sure to call initVdcAdc() in setup();
float cVoltageStatus::measureVoltage (void)
{
    // DEBUG_START;

    // remove the oldest reading
    SumOfVoltages -= ArrayOfVoltageReadings[CurrentReadingIndex];

    // get a new reading
    int32_t currentReading  = adc1_get_raw (ADC_PORT);
    int32_t currentVoltage  = esp_adc_cal_raw_to_voltage (currentReading, adc_chars);   // Convert to unscaled mV.

    SumOfVoltages                               += currentVoltage;
    ArrayOfVoltageReadings[CurrentReadingIndex] = currentVoltage;

    // advance to the next entry in the array
    CurrentReadingIndex = (++CurrentReadingIndex) >= NumberOfReadingsToSave ? 0 : CurrentReadingIndex;

    CurrentAvgVoltage   = float(SumOfVoltages) / float(NumberOfReadingsToSave);
    CurrentAvgVoltage   = (CurrentAvgVoltage * SCALE) / 1000.0f; // Apply Attenuator Scaling, covert from mV to VDC.
    CurrentAvgVoltage   = constrain (CurrentAvgVoltage, 0.0f, 99.0f);

    // DEBUG_END;
    return CurrentAvgVoltage;
}

// *********************************************************************************************
void cVoltageStatus::Poll ()
{
    // _ DEBUG_START;

    uint32_t Now = millis ();

    if (Now >= NextReadingTimeMs)
    {
        // forward one second
        NextReadingTimeMs += MeasurementIntervalMs;

        uint32_t OldSumOfVoltages = SumOfVoltages;
        // DEBUG_V(String(" OldSumOfVoltages: ") + String(OldSumOfVoltages));

        float NewAverageVoltage = measureVoltage ();
        // DEBUG_V(String(" NewSumOfVoltages: ") + String(SumOfVoltages));
        // DEBUG_V(String("NewAverageVoltage: ") + String(float(NewAverageVoltage)));

        // has the voltage changed?
        if (OldSumOfVoltages != SumOfVoltages)
        {
            // DEBUG_V("Set a new value");
            cStatusControl::set (String (NewAverageVoltage, 2) + F (" Vdc"));
        }
    }

    // _ DEBUG_END;
}

// *********************************************************************************************
// OEF
