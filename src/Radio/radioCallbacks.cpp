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
// gainAdjust(): Adjust Digital (USB) or Analog Input Gain. Sets String.
void cRadio::CbDigitalGainAdjust(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    digitalGainStr = sender->value;

    if (!digitalGainStr.equals(DIG_GAIN0_STR) &&
        !digitalGainStr.equals(DIG_GAIN1_STR) &&
        !digitalGainStr.equals(DIG_GAIN2_STR) &&
        !digitalGainStr.equals(DIG_GAIN1_STR) )
    {
        Log.errorln(String("Invalid Digital Gain Value: %s.").c_str(), digitalGainStr.c_str());
        digitalGainStr = DIG_GAIN_DEF_STR;
        ESPUI.updateControlValue(sender, digitalGainStr);
    }

    setDigitalGain(); // Update setting on QN8027 FM Radio Chip.

#ifdef OldWay
    String tempStr  = String(getAudioGain()) + F(" dB");
    ESPUI.print(radioGainID, tempStr);
#endif // def OldWay

    displaySaveWarning();
    Log.infoln(String("Digital Gain Set to: %s.").c_str(), digitalGainStr.c_str());

    DEBUG_END;
}

// ************************************************************************************************
void cRadio::CbProgramServiceName(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    if(sender->value.length() < 4)
    {
        Log.infoln(String(F("Program Service Name: '%s' is too short.")).c_str(), sender->value.c_str());
        sender->value = ProgramServiceName;
        ESPUI.updateControl(sender);
    }

    ProgramServiceName = sender->value;

    setProgramServiceName(); // Update QN8027 Radio Chip's setting.
    displaySaveWarning();
    Log.infoln(String(F("Program Service Name: '%s'.")).c_str(), ProgramServiceName.c_str());

    DEBUG_END;
}

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
        ProgramServiceName = F("PixeyFM");

        setProgramServiceName();
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
