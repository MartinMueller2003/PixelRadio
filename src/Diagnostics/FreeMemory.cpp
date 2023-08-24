/*
  *    File: FreeMemory.cpp
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

#include "FreeMemory.hpp"
#include "memdebug.h"

const PROGMEM String DIAG_FREE_MEM_STR  = "FREE MEMORY";
const PROGMEM String KB_STR = "KB";

// *********************************************************************************************
cFreeMemory::cFreeMemory () :   cStatusControl (DIAG_FREE_MEM_STR, KB_STR)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cFreeMemory::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cStatusControl::AddControls (TabId, color);
    setControlPanelStyle (ePanelStyle::PanelStyle135_black);

    // DEBUG_END;
}

// *********************************************************************************************
void cFreeMemory::Poll ()
{
    // _ DEBUG_START;

    uint32_t Now = millis ();

    if (Now >= NextReadingTimeMs)
    {
        // forward one second
        NextReadingTimeMs += MeasurementIntervalMs;

        uint32_t NewReading = ESP.getFreeHeap () / 1024;

        if (NewReading != PreviousReading)
        {
            // DEBUG_V();
            PreviousReading = NewReading;
            String TempStr = String (NewReading);
            set (TempStr, true, false);
            Log.verboseln ((String (F ("Free Heap Memory: ")) + TempStr).c_str ());
        }
    }

    // _ DEBUG_END;
}

cFreeMemory FreeMemory;

// *********************************************************************************************
// OEF
