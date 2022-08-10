/*
   File: AudioMute.cpp
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
#include "AudioMute.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static const PROGMEM char RADIO_MUTE_FLAG [] = "RADIO_MUTE_FLAG";
static const PROGMEM char ADJUST_MUTE_STR [] = "AUDIO MUTE";

// *********************************************************************************************
cAudioMute::cAudioMute()
{
    /// DEBUG_START;

    /// DEBUG_END;
}

// *********************************************************************************************
void cAudioMute::AddControls (uint16_t value)
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
                                ControlType::Switcher,
                                ADJUST_MUTE_STR,
                                emptyString,
                                ControlColor::Wetasphalt,
                                TabId,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cAudioMute *>(UserInfo)->Callback(sender, type);
                                    }
                                },
                                this);
        Mute = !Mute; // force a UI Update
        set(!Mute);

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
// Callback(): Adjust Audio Input Impedance.
void cAudioMute::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    set(type == S_ACTIVE);

    // DEBUG_END;
}

// *********************************************************************************************
void cAudioMute::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    ReadFromJSON(Mute, config, RADIO_MUTE_FLAG);
    set(Mute);

    // DEBUG_END;
}

// *********************************************************************************************
void cAudioMute::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    config[RADIO_MUTE_FLAG] = Mute;        // Use radio.mute(0/1) when restoring this uint8 value. 1=MuteOn

    // DEBUG_END;
}

// *********************************************************************************************
// set(): Set the Audio Input Impedance on the QN8027 chip.
void cAudioMute::set(bool value)
{
    // DEBUG_START;

    if(value != Mute)
    {
        Mute = value;
        QN8027RadioApi.setAudioMute(Mute);

        ESPUI.setElementStyle(ControlId, Mute ? F("background: red;") : F("background: #bebebe;"));
        ESPUI.updateControlValue(ControlId, Mute ? F("1") : F("0"));

        Log.infoln(String(F("Audio Mute Set to: %s.")).c_str(), String(Mute ? F("On") : F("Off") ).c_str());
        displaySaveWarning();
    }

    // DEBUG_END;
}

// *********************************************************************************************
cAudioMute AudioMute;

// *********************************************************************************************
// OEF
