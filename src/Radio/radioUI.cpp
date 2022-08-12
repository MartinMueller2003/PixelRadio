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
#include <Arduino.h>
#include <ArduinoLog.h>
#include "PixelRadio.h"
#include "radio.hpp"
#include "memdebug.h"
#include "language.h"

#include "AnalogAudioGain.hpp"
#include "AudioGain.hpp"
#include "AudioInputImpedance.hpp"
#include "AudioMode.hpp"
#include "AudioMute.hpp"
#include "DigitalAudioGain.hpp"
#include "FrequencyAdjust.hpp"
#include "PeakAudio.hpp"
#include "PiCode.hpp"
#include "ProgramServiceName.hpp"
#include "PtyCode.hpp"
#include "RdsText.hpp"
#include "RfCarrier.hpp"
#include "TestTone.hpp"
#include "PreEmphasis.hpp"


// *********************************************************************************************
void cRadio::AddAdjControls(uint16_t _adjTab)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != adjTab)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        adjTab = _adjTab;

        ESPUI.addControl(ControlType::Separator, ADJUST_FRQ_CTRL_STR, emptyString, ControlColor::None, adjTab);
        FrequencyAdjust.AddAdjustControls(adjTab);

        ESPUI.addControl(ControlType::Separator, ADJUST_AUDIO_SEP_STR, emptyString, ControlColor::None, adjTab);
        TestTone.AddControls(adjTab);
        AudioMute.AddControls(adjTab);

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::AddDiagControls (uint16_t _diagTab)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != diagTab)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        diagTab = _diagTab;

        
    } while (false);
    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::AddHomeControls (uint16_t _homeTab)
{
    // DEBUG_START;

    String tempStr;
    
    do // once
    {
        if(Control::noParent != homeTab)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        homeTab = _homeTab;

        RfCarrier.AddHomeControls(homeTab);
        FrequencyAdjust.AddHomeControls(homeTab);
        ESPUI.addControl(ControlType::Separator, HOME_SEP_RDS_STR, emptyString, ControlColor::None, homeTab);
        RdsText.AddControls(homeTab);

    } while (false);
    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::AddRadioControls (uint16_t _radioTab)
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

        ESPUI.addControl(ControlType::Separator, RADIO_SEP_RF_SET_STR, emptyString, ControlColor::None, radioTab);
        FrequencyAdjust.AddRadioControls(radioTab);
        RfCarrier.AddControls(radioTab);

        ESPUI.addControl(ControlType::Separator, RADIO_SEP_MOD_STR, emptyString, ControlColor::None, radioTab);
        AudioMode.AddControls(radioTab);
        #ifdef ADV_RADIO_FEATURES
        PreEmphasis.AddControls(radioTab);
        #endif // ifdef ADV_RADIO_FEATURES

        ESPUI.addControl(ControlType::Separator, RADIO_SEP_AUDIO_STR, emptyString,               ControlColor::None,    radioTab);
        AnalogAudioGain.AddControls(radioTab);
        AudioInputImpedance.AddControls(radioTab);
        #ifdef ADV_RADIO_FEATURES
        DigitalAudioGain.AddControls(radioTab);
        #endif // ifdef ADV_RADIO_FEATURES

        ESPUI.addControl(ControlType::Separator, RADIO_AMEAS_SEP_STR, emptyString,            ControlColor::None,    radioTab);
        PeakAudio.AddControls(radioTab);
        #ifdef ADV_RADIO_FEATURES
        AudioGain.AddControls(radioTab);
        #endif // ifdef ADV_RADIO_FEATURES

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::AddRdsControls (uint16_t _rdsTab)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != rdsTab)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        rdsTab = _rdsTab;
        ProgramServiceName.AddControls(rdsTab);
        PiCode.AddControls(rdsTab);
        PtyCode.AddControls(rdsTab);

        ESPUI.addControl(ControlType::Separator, RDS_RESET_SEP_STR, emptyString, ControlColor::None, rdsTab);

        rdsRstID = ESPUI.addControl(ControlType::Button,
                                    RDS_RESET_STR,
                                    RDS_RESTORE_STR,
                                    ControlColor::Alizarin,
                                    rdsTab,
                                    [](Control* sender, int type, void* UserInfo)
                                    {
                                        if(UserInfo)
                                        {
                                            static_cast<cRadio *>(UserInfo)->CbRdsRst(sender, type);
                                        }
                                    },
                                    this);
        ESPUI.setPanelStyle(rdsRstID, "font-size: 1.35em;");
        ESPUI.setElementStyle(rdsRstID, "color: black;");

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
