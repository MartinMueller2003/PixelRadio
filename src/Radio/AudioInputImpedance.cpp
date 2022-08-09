/*
   File: AudioInputImpedance.cpp
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
#include "radio.hpp"
#include "AudioInputImpedance.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

#define INP_IMP05K_STR   "5K Ohms"
#define INP_IMP10K_STR   "10K Ohms"
#define INP_IMP20K_STR   "20K Ohms (default)"
#define INP_IMP40K_STR   "40K Ohms"

static std::map<String, uint8_t> MapOfImpedances
{
    {INP_IMP05K_STR,  5},
    {INP_IMP10K_STR, 10},
    {INP_IMP20K_STR, 20},
    {INP_IMP40K_STR, 40},
};

static const PROGMEM char INPUT_IMPED_STR   [] = "INPUT_IMPED_STR";
static const PROGMEM char INP_IMP_DEF_STR   [] = INP_IMP20K_STR;
static const PROGMEM char RADIO_INP_IMP_STR [] = "INPUT IMPEDANCE";

// *********************************************************************************************
cAudioInputImpedance::cAudioInputImpedance()
{
    /// DEBUG_START;

    InputImpedanceStr = INP_IMP_DEF_STR;

    /// DEBUG_END;
}

// *********************************************************************************************
void cAudioInputImpedance::AddRadioControls (uint16_t _radioTab)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != radioTab)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        radioTab = _radioTab;

        SelectID = ESPUI.addControl(
                            ControlType::Select, 
                            RADIO_INP_IMP_STR, 
                            InputImpedanceStr, 
                            ControlColor::Emerald, 
                            radioTab, 
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cAudioInputImpedance *>(UserInfo)->CbImpedanceAdjust(sender, type);
                                }
                            },
                            this);

        for(auto & CurrentOption : MapOfImpedances)
        {
            ESPUI.addControl(ControlType::Option, 
                            CurrentOption.first.c_str(), 
                            CurrentOption.first, 
                            ControlColor::None, 
                            SelectID);
        }
        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
// CbImpedanceAdjust(): Adjust Audio Input Impedance.
void cAudioInputImpedance::CbImpedanceAdjust(Control *sender, int type)
{
    DEBUG_START;

    DEBUG_V(String("value: ") + String(sender->value));
    DEBUG_V(String(" type: ") + String(type));

    do // once
    {
        set(sender->value);

    } while (false);

    DEBUG_END;
}

// *********************************************************************************************
void cAudioInputImpedance::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    ReadFromJSON(InputImpedanceStr, config, INPUT_IMPED_STR);
    set(InputImpedanceStr);

    // DEBUG_END;
}

// *********************************************************************************************
void cAudioInputImpedance::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    config[INPUT_IMPED_STR] = InputImpedanceStr;      // Use radio.setAudioInpImp(5/10/20/40) when restoring this Int value.

    // DEBUG_END;
}

// *********************************************************************************************
// set(): Set the Audio Input Impedance on the QN8027 chip.
void cAudioInputImpedance::set(String & value)
{
    // DEBUG_START;

    InputImpedanceStr = value;
    if(MapOfImpedances.end() == MapOfImpedances.find(InputImpedanceStr))
    {
        Log.errorln((String(F("cAudioInputImpedance::Set: BAD_VALUE: ")) + InputImpedanceStr).c_str());
        InputImpedanceStr = INP_IMP_DEF_STR;
    }

    Log.infoln(String(F("Input Impedance Set to: %s.")).c_str(), InputImpedanceStr.c_str());

    InputImpedanceValue = MapOfImpedances[InputImpedanceStr];

    QN8027RadioApi.setAudioImpedance(InputImpedanceValue);

    ESPUI.updateControlValue(SelectID, InputImpedanceStr);

#ifdef OldWay
    ESPUI.print(radioGainID, String(getAudioGain()) + F(" dB"));
#endif // def OldWay
    displaySaveWarning();

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
