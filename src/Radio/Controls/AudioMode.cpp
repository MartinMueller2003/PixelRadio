/*
   File: AudioMode.cpp
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
#include "AudioMode.hpp"
#include "QN8027RadioApi.hpp"
#include "language.h"
#include "memdebug.h"

static const PROGMEM char RADIO_STEREO_FLAG     [] = "RADIO_STEREO_FLAG";
static const PROGMEM char RADIO_MONO_STR        [] = "MONO AUDIO";
static const PROGMEM char RADIO_STEREO_STR      [] = "STEREO AUDIO";
static const PROGMEM char RADIO_AUDIO_MODE_STR  [] = "AUDIO MODE";

// *********************************************************************************************
cAudioMode::cAudioMode()
{
    /// DEBUG_START;

    /// DEBUG_END;
}

// *********************************************************************************************
void cAudioMode::AddControls (uint16_t value)
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
                                RADIO_AUDIO_MODE_STR,
                                emptyString,
                                ControlColor::Emerald,
                                TabId,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cAudioMode *>(UserInfo)->Callback(sender, type);
                                    }
                                },
                                this);

        MessageID = ESPUI.addControl(
                                ControlType::Label, 
                                RADIO_AUDIO_MODE_STR, 
                                emptyString, 
                                ControlColor::Emerald, 
                                ControlId);
        ESPUI.setElementStyle(MessageID, CSS_LABEL_STYLE_BLACK);

        // force a UI Update
        Mode = !Mode; 
        set(!Mode);

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void cAudioMode::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    set(type == S_ACTIVE);

    // DEBUG_END;
}

// *********************************************************************************************
void cAudioMode::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    ReadFromJSON(Mode, config, RADIO_STEREO_FLAG);
    set(Mode);

    // DEBUG_END;
}

// *********************************************************************************************
void cAudioMode::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    // Use radio.Mode(0/1) when restoring this uint8 value. 1=ModeOn
    config[RADIO_STEREO_FLAG] = Mode;

    // DEBUG_END;
}

// *********************************************************************************************
void cAudioMode::set(bool value)
{
    // DEBUG_START;

    if(value != Mode)
    {
        Mode = value;
        QN8027RadioApi.setMonoAudio(value);

        ESPUI.print(MessageID, value ? F(RADIO_STEREO_STR) : F(RADIO_MONO_STR));
        ESPUI.updateControlValue(ControlId, value ? F("1") : F("0"));

        Log.infoln(String(F("Radio Audio Mode Set to: %s.")).c_str(), String(value ? F("Stereo") : F("Mono")).c_str());
        displaySaveWarning();
    }

    // DEBUG_END;
}

// *********************************************************************************************
cAudioMode AudioMode;

// *********************************************************************************************
// OEF
