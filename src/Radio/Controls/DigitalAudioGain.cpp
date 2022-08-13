/*
   File: DigitalAudioGain.cpp
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
#include "DigitalAudioGain.hpp"
#include "AudioGain.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static std::map<String, uint8_t> MapOfGainValues
{
    {"0 dB (default)",  0},
    {"1 dB",  1},
    {"2 dB",  2},
};

static const PROGMEM String RADIO_DIG_AUDIO_STR   = "DIGITAL AUDIO GAIN";
static const PROGMEM String DIG_GAIN_DEF_STR      = "0 dB (default)";
static const PROGMEM String DIGITAL_GAIN_STR      = "DIGITAL_GAIN_STR";

// *********************************************************************************************
cDigitalAudioGain::cDigitalAudioGain() : cControlCommon(String(DIGITAL_GAIN_STR))
{
    //_ DEBUG_START;

    DataValueStr = DIG_GAIN_DEF_STR;
    DataValue = MapOfGainValues[DataValueStr];

    //_ DEBUG_END;
}

// *********************************************************************************************
void cDigitalAudioGain::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls(value, ControlType::Select, color);
    ESPUI.updateControlLabel(ControlId, RADIO_DIG_AUDIO_STR.c_str());

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
bool cDigitalAudioGain::set(String & value, String & ResponseMessage)
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
            ResponseMessage = String(F("Digital Gain: Set: BAD VALUE: ")) + value;
            Log.errorln(ResponseMessage.c_str());
            Response = false;
            break;
        }
        DataValueStr = value;

        // DEBUG_V("Update the radio")
        DataValue = MapOfGainValues[DataValueStr];
        QN8027RadioApi.setDigitalGain(DataValue);

        ESPUI.updateControlValue(ControlId, DataValueStr);
        
        AudioGain.set();

        displaySaveWarning();

    } while (false);

    Log.infoln(String(F("Digital Gain Set to: %s.")).c_str(), DataValueStr.c_str());

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cDigitalAudioGain DigitalAudioGain;

// *********************************************************************************************
// OEF
