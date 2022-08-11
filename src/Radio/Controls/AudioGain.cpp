/*
   File: AudioGain.cpp
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
#include "AudioGain.hpp"
#include "AnalogAudioGain.hpp"
#include "AudioInputImpedance.hpp"
#include "memdebug.h"

static const PROGMEM char RADIO_AUDIO_GAIN_STR []   = "AUDIO GAIN";
static const PROGMEM int32_t AUDIO_LEVEL_MAX        = uint32_t(675);

// *********************************************************************************************
cAudioGain::cAudioGain()
{
    //_ DEBUG_START;

    //_ DEBUG_END;
}

// *********************************************************************************************
void cAudioGain::AddControls (uint16_t value)
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
                                ControlType::Label,
                                RADIO_AUDIO_GAIN_STR,
                                emptyString,
                                ControlColor::Emerald,
                                TabId);

        ESPUI.setPanelStyle(ControlId, F("font-size: 1.15em;"));
        ESPUI.setElementStyle(ControlId, F("width: 35%;"));

        set();

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cAudioGain::set()
{
    // DEBUG_START;

    uint32_t    vgaGain   = AnalogAudioGain.get();
    uint32_t    impedance = AudioInputImpedance.get();
    int32_t     FinalGain = ((vgaGain + 1) * 3) - (impedance * 3);

    // DEBUG_V(String("  vgaGain: ") + String(vgaGain));
    // DEBUG_V(String("impedance: ") + String(impedance));
    // DEBUG_V(String("FinalGain: ") + String(FinalGain));

    String Result;
    Result.reserve(128);

    if (FinalGain >= AUDIO_LEVEL_MAX)
    {
        Result = ">";
    }

    Result += String(FinalGain);
    Result += F(" dB");
    // DEBUG_V(String("ControlId: ") + String(ControlId));
    ESPUI.print(ControlId, Result);
    // DEBUG_V(Result);

    // DEBUG_END;
}

// *********************************************************************************************
cAudioGain AudioGain;

// *********************************************************************************************
// OEF
