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
            Log.errorln(String(F("adjFmFreqCallback: %s.")).c_str(), BAD_VALUE_STR);
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

        setFrequency();
        updateUiFrequency(fmFreqX10);
        displaySaveWarning();
        Log.infoln(String(F("FM Frequency Set to: %s.")).c_str(), (String(float(fmFreqX10) / 10.0f, 1) + F(UNITS_MHZ_STR)).c_str());

    } while (false);

    DEBUG_END;
}

// ************************************************************************************************
// audioCallback(): Update the Stereo / Mono Audio modes.
void cRadio::CbAudioMode(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    stereoEnbFlg = (S_ACTIVE == type);

    updateUiAudioMode(stereoEnbFlg);
    setMonoAudio();
    displaySaveWarning();
    Log.infoln(String(F("Radio Audio Mode Set to: %s.")).c_str(), String(stereoEnbFlg ? F("Stereo") : F("Mono")).c_str());

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
    displaySaveWarning();
    Log.infoln(String("Digital Gain Set to: %s.").c_str(), digitalGainStr.c_str());

    DEBUG_END;
}

// ************************************************************************************************
// impAdjustCallback(): Adjust Audio Input Impedance. Sets String.
void cRadio::CbImpedanceAdjust(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    do // once
    {
        inpImpedStr = sender->value;
        if (!inpImpedStr.equals(INP_IMP05K_STR) &&
            !inpImpedStr.equals(INP_IMP10K_STR) &&
            !inpImpedStr.equals(INP_IMP20K_STR) &&
            !inpImpedStr.equals(INP_IMP40K_STR) )
        {
            Log.errorln((String(F("impAdjust: BAD_VALUE: ")) + inpImpedStr).c_str());
            inpImpedStr = INP_IMP_DEF_STR;
            ESPUI.updateControlValue(sender, inpImpedStr);
        }

        setAudioImpedance(); // Update Impednace setting on QN8027 FM Radio Chip.

        ESPUI.print(radioGainID, String(getAudioGain()) + F(" dB"));
        Log.infoln(String(F("Input Impedance Set to: %s.")).c_str(), inpImpedStr.c_str());

        displaySaveWarning();

    } while (false);

    DEBUG_END;
}

// ************************************************************************************************
// muteCallback(): Turn audio on/off (true = Mute the audio).
void cRadio::CbMute(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    if (type == S_ACTIVE) 
    {
        ESPUI.setElementStyle(adjMuteID, "background: red;");
        muteFlg = true;
    }
    else if (type == S_INACTIVE) 
    {
        ESPUI.setElementStyle(adjMuteID, "background: #bebebe;");
        muteFlg = false;
    }

    setAudioMute(); // Update QN8027 Mute Register.
    displaySaveWarning();
    Log.infoln(String(F("Audio Mute Set to: %s.")).c_str(), String(muteFlg ? F("On") : F("Off") ).c_str());

    DEBUG_END;
}

// ************************************************************************************************
// radioEmphasisCallback(): Set Radio pre-emphasis to North America or European. Sets String.
void cRadio::CbRadioEmphasis(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    preEmphasisStr = sender->value;
    if( !preEmphasisStr.equals(PRE_EMPH_USA_STR) &&
        !preEmphasisStr.equals(PRE_EMPH_EUR_STR) )
    {
        preEmphasisStr = PRE_EMPH_DEF_STR;
        Log.errorln(String(F("radioEmphasisCallback: %s.")).c_str(), BAD_VALUE_STR);
    }

    setPreEmphasis(); // Update QN8027 Radio Chip's setting.
    displaySaveWarning();
    Log.infoln(String(F("Pre-Emphasis Set to: %s.")).c_str(), preEmphasisStr.c_str());

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
