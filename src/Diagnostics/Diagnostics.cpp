/*
  *    File: Diagnostics.cpp
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

// #include "PixelRadio.h"
#include "Diagnostics.hpp"
#include "SystemVoltage.hpp"
#include "RfPaVoltage.hpp"
#include "LogLevel.hpp"
#include "FreeMemory.hpp"
#include "FreeFlash.hpp"
#include "SystemRunTime.hpp"
#include "RebootControl.hpp"

#include "memdebug.h"

const char PROGMEM  DIAG_HEALTH_SEP_STR  [] = "HEALTH";
const char PROGMEM  DIAG_DEBUG_SEP_STR   [] = "CODE DEBUGGING";
const char PROGMEM  DIAG_SYSTEM_SEP_STR  [] = "SYSTEM";

// *********************************************************************************************
void cDiagnostics::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    ESPUI.addControl (ControlType::Separator, DIAG_HEALTH_SEP_STR, emptyString, ControlColor::None, TabId);

    HealthGroup.AddControls (TabId, color);
    SystemVoltage.AddControls (HealthGroup.ControlId, color);
    RfPaVoltage.AddControls (HealthGroup.ControlId, color);

    ESPUI.addControl (ControlType::Separator, DIAG_DEBUG_SEP_STR, emptyString, ControlColor::None, TabId);

    LogLevel.AddControls (TabId, color);

    ESPUI.addControl (ControlType::Separator, DIAG_SYSTEM_SEP_STR, emptyString, ControlColor::None, TabId);

    SystemGroup.AddControls (TabId, color);
    FreeMemory.AddControls (SystemGroup.ControlId, color);
    FreeFlash.AddControls (SystemGroup.ControlId, color);
    SystemRunTime.AddControls (SystemGroup.ControlId, color);
    
    RebootControl.AddControls (TabId, color);

    // DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::begin ()
{
    // DEBUG_START;

    LogLevel.begin ();

    // DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::Poll ()
{
    // _ DEBUG_START;

    SystemVoltage.Poll ();
    RfPaVoltage.Poll ();
    FreeMemory.Poll ();
    FreeFlash.Poll ();
    SystemRunTime.Poll ();
    RebootControl.Poll ();

    // _ DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::restoreConfiguration (JsonObject & config)
{
    // DEBUG_START;

    LogLevel.restoreConfiguration (config);

    // DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::saveConfiguration (JsonObject & config)
{
    // DEBUG_START;

    LogLevel.saveConfiguration (config);

    // DEBUG_END;
}

// *********************************************************************************************
cDiagnostics Diagnostics;

// *********************************************************************************************
// OEF
