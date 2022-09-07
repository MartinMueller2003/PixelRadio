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
#include "memdebug.h"

#include "Gpio19.hpp"
#include "Gpio23.hpp"
#include "Gpio33.hpp"

// *********************************************************************************************
void cDiagnostics::AddControls (uint16_t TabId, ControlColor color)
{
    DEBUG_START;

    DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::begin ()
{
    DEBUG_START;

    DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::Poll ()
{
    // _ DEBUG_START;


    // _ DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::restoreConfiguration (JsonObject & config)
{
    // DEBUG_START;

    // DEBUG_END;
}

// *********************************************************************************************
void cDiagnostics::saveConfiguration (JsonObject & config)
{
    // DEBUG_START;


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


// ************************************************************************************************
void updateUiVolts (void)
{
#ifdef OldWay
    extern uint32_t paVolts;
    static uint32_t previousMillis = 0;
    char logBuff[60];

    if (previousMillis == 0)
    {
        previousMillis = millis (); // Initialize First entry;
    }
    else if (millis () - previousMillis >= VOLTS_UPD_TIME)
    {
        previousMillis  = millis ();
        tempStr         = String (vbatVolts, 1);
        tempStr         += " VDC";
        ESPUI.print (diagVbatID, tempStr);
        sprintf (logBuff, "Health Check, System Voltage: %01.1f VDC.", vbatVolts);
        Log.verboseln (logBuff);

        tempStr = String (paVolts, 1);
        tempStr += " VDC";
        ESPUI.print (diagVdcID, tempStr);
        sprintf (logBuff, "Health Check, RF AMP Voltage: %01.1f VDC.", paVolts);
        Log.verboseln (logBuff);
    }
#endif // def OldWay

}
// *********************************************************************************************
cDiagnostics Diagnostics;

// *********************************************************************************************
// OEF
