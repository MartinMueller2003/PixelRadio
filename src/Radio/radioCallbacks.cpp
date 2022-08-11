/*
   File: radioCallbacks.cpp
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
#include <Wire.h>
#include "PixelRadio.h"
#include "language.h"
#include "radio.hpp"
#include "memdebug.h"

// ************************************************************************************************
void cRadio::CbRdsRst(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));
  
    if(B_DOWN == type)
    {
        PiCode = 0x6400;
        PtyCode = 0;
#ifdef OldWay
        ProgramServiceName = F("PixeyFM");
        setProgramServiceName();
#endif // def OldWay

        setPiCode();
        setPtyCode();
        updateUiPtyCode();

        displaySaveWarning();
        Log.infoln(String(F("Reset RDS Settings to defaults.")).c_str());
    }

    DEBUG_END;
}

// ************************************************************************************************
// rfPower(): Sets RF Power.
void cRadio::CbRfPowerCallback(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    rfPowerStr = sender->value;

    if (!rfPowerStr.equals(RF_PWR_LOW_STR) &&
        !rfPowerStr.equals(RF_PWR_MED_STR) &&
        !rfPowerStr.equals(RF_PWR_HIGH_STR) )
    {
        Log.errorln(String(F("rfPower: %s.")).c_str(), BAD_VALUE_STR);
        rfPowerStr = RF_PWR_DEF_STR;
    }

    setRfPower();    // Update RF Power Setting on QN8027 FM Radio Chip.
    displaySaveWarning();
    Log.infoln(String(F("RF Power Set to: %s.")).c_str(), rfPowerStr.c_str());

    DEBUG_END;
}

// ************************************************************************************************
void cRadio::CbSetPtyCode(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    setPtyCode(sender->value);
    displaySaveWarning();

    Log.infoln(String(F("RDS PTY Code Set to: '%s'")).c_str(), sender->value.c_str());

    // DEBUG_END;
}

// ************************************************************************************************
void cRadio::CbSetPiCode(Control *sender, int type)
{
    DEBUG_START;
    
    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    uint16_t tempPiCode = PiCode;
    String piStr;

    piStr.reserve(20);

    piStr = sender->value;
    piStr.trim();
    if (piStr.isEmpty()) 
    {
        tempPiCode = RDS_PI_CODE_DEF; // Use default PI Code.
    }
    else 
    {
        tempPiCode = strtol(piStr.c_str(), NULL, HEX);
    }

    if ((tempPiCode < RDS_PI_CODE_MIN) || (tempPiCode > RDS_PI_CODE_MAX))
    { // Value Out of Range.
        tempPiCode = PiCode;
    }

    PiCode = tempPiCode;
    String Response = String(F("0x")) + String(PiCode, HEX);
    ESPUI.print(rdsPiID, Response);
    displaySaveWarning();
    setPiCode();

    Log.infoln(String(F("RDS PI Code Set to: \"%s\"")).c_str(), Response);
}

// *********************************************************************************************
// OEF
