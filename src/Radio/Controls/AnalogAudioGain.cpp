/*
   File: AnalogAudioGain.cpp
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
#include <map>

#include "AnalogAudioGain.hpp"
#include "AudioGain.hpp"
#include "memdebug.h"
#include "QN8027RadioApi.hpp"

#define VGA_GAIN0_STR     " 3dB"
#define VGA_GAIN1_STR     " 6dB"
#define VGA_GAIN2_STR     " 9dB"
#define VGA_GAIN3_STR     "12dB (default)"
#define VGA_GAIN4_STR     "15dB"
#define VGA_GAIN5_STR     "18dB"
#define VGA_GAIN_DEF_STR  VGA_GAIN3_STR;

static std::map <String, String> MapOfGainValues
{
    {VGA_GAIN0_STR,  "0"},
    {VGA_GAIN1_STR,  "1"},
    {VGA_GAIN2_STR,  "2"},
    {VGA_GAIN3_STR,  "3"},
    {VGA_GAIN4_STR,  "4"},
    {VGA_GAIN5_STR,  "5"},
};

static const PROGMEM String     RADIO_VGA_AUDIO_STR     = "ANALOG (VGA) AUDIO GAIN";
static const PROGMEM String     ANALOG_GAIN_STR         = "ANALOG_GAIN_STR";

// *********************************************************************************************
cAnalogAudioGain::cAnalogAudioGain () :   cChoiceListControl (ANALOG_GAIN_STR, RADIO_VGA_AUDIO_STR, MapOfGainValues)
{
    // _ DEBUG_START;

    DataValueStr = VGA_GAIN_DEF_STR;

    // _ DEBUG_END;
}

// *********************************************************************************************
bool cAnalogAudioGain::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (String ("value: ") + value);

    bool Response = true;

    do  // once
    {
        if (!cControlCommon::set (value, ResponseMessage, ForceUpdate))
        {
            // DEBUG_V ("Failed to save the new value");
            break;
        }
        // DEBUG_V ("Update the radio")
        QN8027RadioApi.setVgaGain (get32 ());
        AudioGain.set ();
    } while (false);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cAnalogAudioGain AnalogAudioGain;

// *********************************************************************************************
// OEF
