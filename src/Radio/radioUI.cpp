/*
   File: radio.cpp
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
#include "language.h"
#include "memdebug.h"
#include "PixelRadio.h"
#include "radio.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

#include "AnalogAudioGain.hpp"
#include "AudioGain.hpp"
#include "AudioInputImpedance.hpp"
#include "AudioMode.hpp"
#include "AudioMute.hpp"
#include "DigitalAudioGain.hpp"
#include "FrequencyAdjust.hpp"
#include "PeakAudio.hpp"
#include "PiCode.hpp"
#include "PreEmphasis.hpp"
#include "ProgramServiceName.hpp"
#include "PtyCode.hpp"
#include "RdsReset.hpp"
#include "RdsText.hpp"
#include "RfCarrier.hpp"
#include "TestTone.hpp"

const char PROGMEM  ADJUST_FRQ_CTRL_STR  [] = "FM TRANSMIT FREQUENCY";
const char PROGMEM  ADJUST_AUDIO_SEP_STR [] = "AUDIO CONTROLS";
const char PROGMEM  RADIO_AMEAS_SEP_STR  [] = "AUDIO MEASUREMENT";
const char PROGMEM  RADIO_SEP_AUDIO_STR  [] = "AUDIO SETTINGS";
const char PROGMEM  RADIO_SEP_MOD_STR    [] = "MODULATION SETTINGS";
const char PROGMEM  RADIO_SEP_RF_SET_STR [] = "RF SETTINGS";

// *********************************************************************************************
void cRadio::AddAdjControls (uint16_t adjTab, ControlColor color)
{
    // DEBUG_START;

    ESPUI.addControl (ControlType::Separator, ADJUST_FRQ_CTRL_STR, emptyString, ControlColor::None, adjTab);
    FrequencyAdjust.AddAdjustControls (adjTab, color);

    ESPUI.addControl (ControlType::Separator, ADJUST_AUDIO_SEP_STR, emptyString, ControlColor::None, adjTab);
    TestTone.AddControls (adjTab, color);
    AudioMute.AddControls (adjTab, color);

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::AddDiagControls (uint16_t diagTab, ControlColor color)
{
    // DEBUG_START;



    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::AddHomeControls (uint16_t homeTab, ControlColor color)
{
    // DEBUG_START;

    String tempStr;

    RfCarrier.AddHomeControls (homeTab, color);
    FrequencyAdjust.AddHomeControls (homeTab, color);
    ESPUI.addControl (ControlType::Separator, HOME_SEP_RDS_STR, emptyString, ControlColor::None, homeTab);
    RdsText.AddControls (homeTab, color);

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::AddRadioControls (uint16_t radioTab, ControlColor color)
{
    // DEBUG_START;

    ESPUI.addControl (ControlType::Separator, RADIO_SEP_RF_SET_STR, emptyString, ControlColor::None, radioTab);
    FrequencyAdjust.AddRadioControls (radioTab, color);
    RfCarrier.AddControls (radioTab, color);

    ESPUI.addControl (ControlType::Separator, RADIO_SEP_MOD_STR, emptyString, ControlColor::None, radioTab);
    AudioMode.AddControls (radioTab, color);
#ifdef ADV_RADIO_FEATURES
        PreEmphasis.AddControls (radioTab, color);
#endif // ifdef ADV_RADIO_FEATURES

    ESPUI.addControl (ControlType::Separator, RADIO_SEP_AUDIO_STR, emptyString, ControlColor::None, radioTab);
    AnalogAudioGain.AddControls (radioTab, color);
    AudioInputImpedance.AddControls (radioTab, color);
#ifdef ADV_RADIO_FEATURES
        DigitalAudioGain.AddControls (radioTab, color);
#endif // ifdef ADV_RADIO_FEATURES

    ESPUI.addControl (ControlType::Separator, RADIO_AMEAS_SEP_STR, emptyString, ControlColor::None, radioTab);
    PeakAudio.AddControls (radioTab, color);
#ifdef ADV_RADIO_FEATURES
        AudioGain.AddControls (radioTab, color);
#endif // ifdef ADV_RADIO_FEATURES

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::AddRdsControls (uint16_t rdsTab, ControlColor color)
{
    // DEBUG_START;

    ProgramServiceName.AddControls (rdsTab, color);
    PiCode.AddControls (rdsTab, color);
    PtyCode.AddControls (rdsTab, color);
    RdsReset.AddControls (rdsTab, color);

    // DEBUG_END;
}

// *********************************************************************************************
// EOF
