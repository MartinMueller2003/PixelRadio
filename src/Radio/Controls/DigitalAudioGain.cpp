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
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static std::map<String, uint8_t> MapOfGainValues
{
    {"0 dB (default)",  0},
    {"1 dB",  1},
    {"2 dB",  2},
};

static const PROGMEM char RADIO_DIG_AUDIO_STR   [] = "DIGITAL AUDIO GAIN";
static const PROGMEM char DIG_GAIN_DEF_STR      [] = "0 dB (default)";
static const PROGMEM char DIGITAL_GAIN_STR      [] = "DIGITAL_GAIN_STR";

// *********************************************************************************************
cDigitalAudioGain::cDigitalAudioGain()
{
    /// DEBUG_START;

    GainStr = DIG_GAIN_DEF_STR;
    GainValue = MapOfGainValues[GainStr];

    /// DEBUG_END;
}

// *********************************************************************************************
void cDigitalAudioGain::AddControls (uint16_t value)
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
                            RADIO_DIG_AUDIO_STR, 
                            emptyString, 
                            ControlColor::Emerald, 
                            TabId, 
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cDigitalAudioGain *>(UserInfo)->Callback(sender, type);
                                }
                            },
                            this);
/*
        radioDgainID = ESPUI.addControl(
                                ControlType::Select,
                                RADIO_DIG_AUDIO_STR,
                                digitalGainStr,
                                ControlColor::Emerald,
                                radioTab,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRadio *>(UserInfo)->CbDigitalGainAdjust(sender, type);
                                    }
                                },
                                this);
*/
        for(auto & CurrentOption : MapOfGainValues)
        {
            ESPUI.addControl(ControlType::Option, 
                            CurrentOption.first.c_str(), 
                            CurrentOption.first, 
                            ControlColor::None, 
                            ControlId);
        }

        String Temp = GainStr;
        GainStr.clear();
        set(Temp);

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void cDigitalAudioGain::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    set(sender->value);

    // DEBUG_END;
}

// *********************************************************************************************
void cDigitalAudioGain::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    String Temp;
    ReadFromJSON(Temp, config, DIGITAL_GAIN_STR);
    set(Temp);

    // DEBUG_END;
}

// *********************************************************************************************
void cDigitalAudioGain::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    config[DIGITAL_GAIN_STR] = GainStr;

    // DEBUG_END;
}

// *********************************************************************************************
void cDigitalAudioGain::set(String & value)
{
    // DEBUG_START;

    do // once
    {
        if(GainStr.equals(value))
        {
            // DEBUG_V("Ignore duplicate setting");
            break;
        }

        GainStr = value;
        if(MapOfGainValues.end() == MapOfGainValues.find(GainStr))
        {
            Log.errorln(String(F("Digital Gain: Set: BAD VALUE: '%s'. Using default.")).c_str(), value);
            break;
        }

        // DEBUG_V("Update the radio")
        GainValue = MapOfGainValues[GainStr];
        QN8027RadioApi.setDigitalGain(GainValue);

        ESPUI.updateControlValue(ControlId, GainStr);

#ifdef OldWay
        String tempStr  = String(getAudioGain()) + F(" dB");
        ESPUI.print(radioGainID, tempStr);
#endif // def OldWay

    displaySaveWarning();
    } while (false);

    Log.infoln(String("Digital Gain Set to: %s.").c_str(), GainStr.c_str());

    // DEBUG_END;
}

// *********************************************************************************************
cDigitalAudioGain DigitalAudioGain;

// *********************************************************************************************
// OEF
