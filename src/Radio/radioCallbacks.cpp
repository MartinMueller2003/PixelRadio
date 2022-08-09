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
void cRadio::CbAdjFmFreq(Control *sender, int type) 
{
    DEBUG_START;

    DEBUG_V(String("    value: ") + String(sender->value));
    DEBUG_V(String("     type: ") + String(type));
    DEBUG_V(String("fmFreqX10: ") + String(fmFreqX10));

    do // once
    {
        if(0 <= type)
        {
            DEBUG_V("Ignore button up event");
            break;
        }

        if (type == P_LEFT_DOWN) 
        {
            DEBUG_V("Decr 100khz");
            fmFreqX10 -= FM_FREQ_SKP_KHZ;
        }
        else if (type == P_RIGHT_DOWN) 
        {
            DEBUG_V("Inc 100khz");
            fmFreqX10 += FM_FREQ_SKP_KHZ;
        }
        else if (type == P_BACK_DOWN) 
        {
            DEBUG_V("Dec 1Mhz");
            fmFreqX10 -= FM_FREQ_SKP_MHZ;
        }
        else if (type == P_FOR_DOWN) 
        {
            DEBUG_V("Inc 1Mhz");
            fmFreqX10 += FM_FREQ_SKP_MHZ;
        }
        else
        {
            Log.errorln(String(F("CbAdjFmFreq: %s.")).c_str(), BAD_VALUE_STR);
            break;
        }
    
        DEBUG_V(String("fmFreqX10: ") + String(fmFreqX10));

        if (fmFreqX10 > FM_FREQ_MAX_X10) 
        {
            DEBUG_V("Frequency is too high");
            fmFreqX10 = FM_FREQ_MAX_X10;
        }
        else if (fmFreqX10 < FM_FREQ_MIN_X10) 
        {
            DEBUG_V("Frequency is too ;ow");
            fmFreqX10 = FM_FREQ_MIN_X10;
        }

        DEBUG_V(String("fmFreqX10: ") + String(fmFreqX10));

        setFrequency(fmFreqX10);
        displaySaveWarning();
        Log.infoln(String(F("FM Frequency Set to: %s.")).c_str(), (String(float(fmFreqX10) / 10.0f, 1) + F(UNITS_MHZ_STR)).c_str());

    } while (false);

    DEBUG_END;
}

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
    String tempStr  = String(getAudioGain()) + F(" dB");
    ESPUI.print(radioGainID, tempStr);

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
// radioEmphasisCallback(): Set Radio pre-emphasis to North America or European. Sets String.
void cRadio::CbRadioEmphasis(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    preEmphasisStr = sender->value;
    if( !preEmphasisStr.equals(PRE_EMPH_USA_STR) &&
        !preEmphasisStr.equals(PRE_EMPH_EUR_STR) )
    {
        preEmphasisStr = PRE_EMPH_DEF_STR;
        Log.errorln(String(F("radioEmphasisCallback: %s.")).c_str(), BAD_VALUE_STR);
    }

    setPreEmphasis(); // Update QN8027 Radio Chip's setting.
    setPtyCodeOptionValues();
    displaySaveWarning();
    Log.infoln(String(F("Pre-Emphasis Set to: %s.")).c_str(), preEmphasisStr.c_str());

    // DEBUG_END;
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
// rfCarrierCallback(): Controls RF Carrier, On/Off.
void cRadio::CbRfCarrier(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    extern float paVolts;
    
    rfCarrierFlg = (S_ACTIVE == type);
    String tempStr;
#ifdef OldWay
    if (type == S_ACTIVE)
    {
        if (fmRadioTestCode == FM_TEST_FAIL)
        {
            Log.errorln(String(F("rfCarrierCallback: RADIO MODULE FAILURE!")).c_str());
            tempStr = F("On, Warning: Radio Module Failure");
            ESPUI.print(homeOnAirID, RADIO_FAIL_STR); // Update homeTab panel.
        }
        else if (fmRadioTestCode == FM_TEST_VSWR)
        {
            Log.errorln(String(F("rfCarrierCallback: RADIO MODULE HAS HIGH VSWR!")).c_str());
            tempStr = F("On, Warning: Radio Module RF-Out has High VSWR");
            ESPUI.print(homeOnAirID, RADIO_VSWR_STR); // Update homeTab panel.
        }
        else if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX))
        {
            Log.errorln(String(F("rfCarrierCallback: RF PA HAS INCORRECT VOLTAGE!")).c_str());
            tempStr = F("On, Warning: RF PA has Incorrect Voltage");
            ESPUI.print(homeOnAirID, RADIO_VOLT_STR); // Update homeTab panel.
        }
        else
        {
            tempStr = F("On");
            ESPUI.print(homeOnAirID, RADIO_ON_AIR_STR); // Update homeTab panel.
        }
    }
    else
    {
        tempStr = F("Off");
        ESPUI.print(homeOnAirID, RADIO_OFF_AIR_STR); // Update homeTab panel.
    }

    setRfCarrier();
    displaySaveWarning();
    Log.infoln(String(F("RF Carrier Enable Set to: %s.")).c_str(), tempStr.c_str());
#endif // def OldWay

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
// testModeCallback(): Audio Test Tone Mode Control (true = Audio Test Mode On).
void cRadio::CbTestMode(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    testModeFlg = type == S_ACTIVE;

    ESPUI.setElementStyle(adjTestModeID, String(testModeFlg ? F("background: red;") : F("background: #bebebe;")));

    updateTestTones(testModeFlg); // Reset Test Tone Elasped Timer.

    Log.infoln(String(F("Test Mode Set to: %s.")).c_str(), String(testModeFlg ? F("On") : F("Off")).c_str());

    DEBUG_END;
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

// ************************************************************************************************
void cRadio::CbVgaGainAdjust(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    vgaGainStr = sender->value;

    if(!vgaGainStr.equals(VGA_GAIN0_STR) &&
       !vgaGainStr.equals(VGA_GAIN1_STR) &&
       !vgaGainStr.equals(VGA_GAIN2_STR) &&
       !vgaGainStr.equals(VGA_GAIN3_STR) &&
       !vgaGainStr.equals(VGA_GAIN4_STR) &&
       !vgaGainStr.equals(VGA_GAIN5_STR) )
    {
        vgaGainStr = VGA_GAIN_DEF_STR;
        ESPUI.updateControlValue(sender, vgaGainStr);
        Log.errorln(String(F("gainAdjust: BAD_VALUE.")).c_str());
    }

    setVgaGain(); // Update analog (VGA) gain setting on QN8027 FM Radio Chip.
    displaySaveWarning();

    String tempStr  = String(getAudioGain()) + F(" dB");
    ESPUI.print(radioGainID, tempStr);
    Log.infoln(String(F("Analog Input Gain Set to: %s.")).c_str(), tempStr.c_str());

    DEBUG_END;
}

// *********************************************************************************************
// OEF
