/*
   File: measure.cpp
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

#include "config.h"
#include "globals.h"
#include "PixelRadio.h"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

const uint16_t  DEFAULT_VREF            = 1100;
const uint8_t   E_AVG_SIZE              = 16;                                   // Size of voltage averaging buffers.
const float VDC_SCALE                   = ((100000.0 + 33000.0) / 33000.0);     // Resistor Attenuator on "VDC" ADC Port.
const adc1_channel_t    VDC_ADC_PORT    = ADC1_CHANNEL_0;                       // GPIO-36, "VDC" Voltage Monitor (9V RF PA rail).

// Local Scope Vars

// *********************************************************************************************
// measureVdcVoltage(): Measure RF Power Amp's Voltage using data averaging.
void measureVdcVoltage (void)
{
    static int avgIndex         = 0;    // Index of the current Amps reading
    static uint32_t totalVdc    = 0;    // Current totalizer for Amps averaging
    uint32_t    voltage;
    uint32_t    reading;
/*
    reading = adc1_get_raw (VDC_ADC_PORT);
    voltage = esp_adc_cal_raw_to_voltage (reading, adc_chars);  // Convert to unscaled mV.

    totalVdc                = totalVdc - vdcAvgBuff[avgIndex];
    vdcAvgBuff[avgIndex]    = voltage;
    totalVdc    = totalVdc + vdcAvgBuff[avgIndex];
*/
    avgIndex    += 1;
    avgIndex    = avgIndex >= E_AVG_SIZE ? 0 : avgIndex;

    voltage = totalVdc / E_AVG_SIZE;
    extern uint32_t paVolts;

    paVolts = (voltage * VDC_SCALE) / 1000.0f;  // Apply Attenuator Scaling, covert from mV to VDC.
    paVolts = constrain (paVolts, 0.0f, 99.0f);
}

// *********************************************************************************************
// initVdcBuffer(): Initialize the Supply Volts Averaging Buffer (fill with current voltage).
void initVdcBuffer (void)
{
    for (uint8_t i = 0; i < E_AVG_SIZE; i++)
    {
        // measureVbatVoltage ();  // Prime the Volts averaging buffer.
        measureVdcVoltage ();   // Prime the Volts averaging buffer.
        delay (25);
    }
}

// *********************************************************************************************
// processMeasurements(): Periodically perform voltage measurements. Must be called in main loop.
void processMeasurements (void)
{
    uint32_t currentMillis              = millis ();    // Snapshot of System Timer.
    static uint32_t previousMeasMillis  = millis ();    // Timer for Voltage Measurement.

    currentMillis = millis ();

    // System Tick Timers Tasks
    if (currentMillis - previousMeasMillis >= MEAS_TIME)
    {
        previousMeasMillis = currentMillis;
        measureVdcVoltage ();
    }
}

// *********************************************************************************************
// EOF
