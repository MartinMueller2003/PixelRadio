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
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

#define VGA_GAIN0_STR     " 3dB"
#define VGA_GAIN1_STR     " 6dB"
#define VGA_GAIN2_STR     " 9dB"
#define VGA_GAIN3_STR     "12dB (default)"
#define VGA_GAIN4_STR     "15dB"
#define VGA_GAIN5_STR     "18dB"
#define VGA_GAIN_DEF_STR  VGA_GAIN3_STR;

static std::map<String, uint8_t> MapOfGainValues
{
    {VGA_GAIN0_STR,  0},
    {VGA_GAIN1_STR,  1},
    {VGA_GAIN2_STR,  2},
    {VGA_GAIN3_STR,  3},
    {VGA_GAIN4_STR,  4},
    {VGA_GAIN5_STR,  5},
};

static const PROGMEM char RADIO_VGA_AUDIO_STR   [] = "ANALOG (VGA) AUDIO GAIN";
static const PROGMEM char ANALOG_GAIN_STR       [] = "ANALOG_GAIN_STR";

// *********************************************************************************************
cAnalogAudioGain::cAnalogAudioGain()
{
    /// DEBUG_START;

    vgaGainStr = VGA_GAIN_DEF_STR;
    vgaGainValue = MapOfGainValues[vgaGainStr];

    /// DEBUG_END;
}

// *********************************************************************************************
void cAnalogAudioGain::AddControls (uint16_t value)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != TabId)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        TabId = value;

        ControlId = ESPUI.addControl(
                            ControlType::Select, 
                            RADIO_VGA_AUDIO_STR, 
                            emptyString, 
                            ControlColor::Emerald, 
                            TabId, 
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cAnalogAudioGain *>(UserInfo)->Callback(sender, type);
                                }
                            },
                            this);

        for(auto & CurrentOption : MapOfGainValues)
        {
            ESPUI.addControl(ControlType::Option, 
                            CurrentOption.first.c_str(), 
                            CurrentOption.first, 
                            ControlColor::None, 
                            ControlId);
        }

        String Temp = vgaGainStr;
        vgaGainStr.clear();
        set(Temp);

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void cAnalogAudioGain::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    set(sender->value);

    // DEBUG_END;
}

// *********************************************************************************************
void cAnalogAudioGain::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    String Temp;
    ReadFromJSON(Temp, config, ANALOG_GAIN_STR);
    set(Temp);

    // DEBUG_END;
}

// *********************************************************************************************
void cAnalogAudioGain::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    config[ANALOG_GAIN_STR] = vgaGainStr;       // Use radio.setTxInputBufferGain(0-5) when restoring this Int value.

    // DEBUG_END;
}

// *********************************************************************************************
void cAnalogAudioGain::set(String & value)
{
    // DEBUG_START;

    do // once
    {
        if(vgaGainStr.equals(value))
        {
            // DEBUG_V("Ignore duplicate setting");
            break;
        }

        vgaGainStr = value;
        if(MapOfGainValues.end() == MapOfGainValues.find(vgaGainStr))
        {
            Log.errorln(String(F("Analog (VGA) Gain: Set: BAD VALUE: '%s'. Using default.")).c_str(), value);
            break;
        }

        // DEBUG_V("Update the radio")
        vgaGainValue = MapOfGainValues[vgaGainStr];
        QN8027RadioApi.setVgaGain(vgaGainValue);

        ESPUI.updateControlValue(ControlId, vgaGainStr);
        AudioGain.set();

        displaySaveWarning();
    } while (false);

    Log.infoln(String(F("Analog (VGA) Gain Set to: %s.")).c_str(), vgaGainStr.c_str());

    // DEBUG_END;
}

// *********************************************************************************************
cAnalogAudioGain AnalogAudioGain;

// *********************************************************************************************
// OEF
