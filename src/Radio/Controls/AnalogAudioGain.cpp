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

static const PROGMEM String RADIO_VGA_AUDIO_STR = "ANALOG (VGA) AUDIO GAIN";
static const PROGMEM String ANALOG_GAIN_STR     = "ANALOG_GAIN_STR";

// *********************************************************************************************
cAnalogAudioGain::cAnalogAudioGain() : cControlCommon(String(ANALOG_GAIN_STR))
{
    //_ DEBUG_START;

    DataValueStr = VGA_GAIN_DEF_STR;
    DataValue = MapOfGainValues[DataValueStr];

    //_ DEBUG_END;
}

// *********************************************************************************************
void cAnalogAudioGain::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls(value, ControlType::Select, color);
    ESPUI.updateControlLabel(ControlId, RADIO_VGA_AUDIO_STR.c_str());

    for(auto & CurrentOption : MapOfGainValues)
    {
        ESPUI.addControl(ControlType::Option, 
                        CurrentOption.first.c_str(), 
                        CurrentOption.first, 
                        ControlColor::None, 
                        ControlId);
    }
    ESPUI.updateControlValue(ControlId, DataValueStr);

    // DEBUG_END;
}

// *********************************************************************************************
bool cAnalogAudioGain::set(String & value, String & ResponseMessage)
{
    // DEBUG_START;

    bool Response = true;
    ResponseMessage.reserve(128);
    ResponseMessage.clear();

    do // once
    {
        if(DataValueStr.equals(value))
        {
            // DEBUG_V("Ignore duplicate setting");
            break;
        }

        if(MapOfGainValues.end() == MapOfGainValues.find(value))
        {
            ResponseMessage = String(F("Analog (VGA) Gain: Set: BAD VALUE: ")) + value;
            Log.errorln(ResponseMessage.c_str());
            Response = false;
            break;
        }
        DataValueStr = value;

        // DEBUG_V("Update the radio")
        DataValue = MapOfGainValues[DataValueStr];
        QN8027RadioApi.setVgaGain(DataValue);

        ESPUI.updateControlValue(ControlId, DataValueStr);
        
        AudioGain.set();

        displaySaveWarning();

    } while (false);

    Log.infoln(String(F("Analog (VGA) Gain Set to: %s.")).c_str(), DataValueStr.c_str());

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cAnalogAudioGain AnalogAudioGain;

// *********************************************************************************************
// OEF
