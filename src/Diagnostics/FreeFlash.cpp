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
#include <ArduinoLog.h>

#include "FreeFlash.hpp"
#include "memdebug.h"

const char PROGMEM DIAG_FREE_FLASH_STR    [] = "FREE FLASH";

// *********************************************************************************************
cFreeFlash::cFreeFlash () :   cStatusControl (DIAG_FREE_FLASH_STR)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cFreeFlash::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cStatusControl::AddControls (TabId, color);
    setControlPanelStyle (ePanelStyle::PanelStyle135_black);

    // DEBUG_END;
}

// *********************************************************************************************
void cFreeFlash::Poll ()
{
    // _ DEBUG_START;
    extern uint32_t GetFreeFsSpace ();

    uint32_t Now = millis ();

    if (Now >= NextReadingTimeMs)
    {
        // forward one second
        NextReadingTimeMs += MeasurementIntervalMs;

        uint32_t NewReading = GetFreeFsSpace ();

        if (NewReading != PreviousReading)
        {
            // DEBUG_V();
            PreviousReading = NewReading;
            String TempStr = String (NewReading) + F (" KB");
            set (TempStr, true, true);
            Log.verboseln ((String (F ("Free Flash Memory: ")) + TempStr).c_str ());
        }
    }

    // _ DEBUG_END;
}

cFreeFlash FreeFlash;

// *********************************************************************************************
// OEF
