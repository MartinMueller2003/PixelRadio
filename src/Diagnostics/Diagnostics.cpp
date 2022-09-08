/*
   File: Diagnostics.cpp
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
#include <ArduinoLog.h>

// #include "PixelRadio.h"
#include "Diagnostics.hpp"
#include "SystemVoltage.hpp"
#include "RfPaVoltage.hpp"
#include "LogLevel.hpp"
#include "FreeMemory.hpp"
#include "SystemRunTime.hpp"

#include "memdebug.h"

const char PROGMEM DIAG_HEALTH_SEP_STR  [] = "HEALTH";
const char PROGMEM DIAG_DEBUG_SEP_STR   [] = "CODE DEBUGGING";
const char PROGMEM DIAG_SYSTEM_SEP_STR  [] = "SYSTEM";

// *********************************************************************************************
void cDiagnostics::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    ESPUI.addControl (ControlType::Separator, DIAG_HEALTH_SEP_STR, emptyString, ControlColor::None, TabId);

    SystemVoltage.AddControls(TabId, color);
    RfPaVoltage.AddControls(TabId, color);

    ESPUI.addControl (ControlType::Separator, DIAG_DEBUG_SEP_STR, emptyString, ControlColor::None, TabId);

    LogLevel.AddControls(TabId, color);

    ESPUI.addControl (ControlType::Separator, DIAG_SYSTEM_SEP_STR, emptyString, ControlColor::None, TabId);

    FreeMemory.AddControls(TabId, color);
    SystemRunTime.AddControls(TabId, color);

    // DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::begin ()
{
    // DEBUG_START;

    LogLevel.begin();

    // DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::Poll ()
{
    // _ DEBUG_START;

    SystemVoltage.Poll();
    RfPaVoltage.Poll();
    FreeMemory.Poll();
    SystemRunTime.Poll();

    // _ DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::restoreConfiguration (JsonObject & config)
{
    // DEBUG_START;

    LogLevel.restoreConfiguration(config);

    // DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::saveConfiguration (JsonObject & config)
{
    // DEBUG_START;

    LogLevel.saveConfiguration(config);

    // DEBUG_END;
}

// *********************************************************************************************
// updateOnAirSign(): Turn on the "On Air" LED Sign if RF Carrier is present.
void cDiagnostics::updateOnAirSign (void)
{
    // DEBUG_START;
#ifdef OldWay
        new way:
        call GPIO and tell it to update the output and the UI.

        if (rfCarrierFlg)
        {
            digitalWrite (ON_AIR_PIN, SIGN_ON);
        }
        else
        {
            digitalWrite (ON_AIR_PIN, SIGN_OFF);
        }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
cDiagnostics Diagnostics;

// *********************************************************************************************
// OEF
