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
#include "RdsText.hpp"
#include "RfCarrier.hpp"
#include "TestTone.hpp"
#include "PreEmphasis.hpp"


typedef struct 
{
    uint16_t code;
    uint16_t UiId;
    String name[2];
} PtyCodeEntry_t;

// 0 = US, 1 = EU
static PtyCodeEntry_t ListOfPtyCodes[] =
{
    { 0, Control::noParent, {{"No programme type defined"}, {"No programme type defined"}}},
    { 1, Control::noParent, {{"News"},                      {"News"}}},
    { 2, Control::noParent, {{"Information"},               {"Current affairs"}}},
    { 3, Control::noParent, {{"Sport"},                     {"Information"}}},
    { 4, Control::noParent, {{"Talk"},                      {"Sport"}}},
    { 5, Control::noParent, {{"Rock"},                      {"Education"}}},
    { 6, Control::noParent, {{"Classic Rock"},              {"Drama"}}},
    { 7, Control::noParent, {{"Adult Hits"},                {"Culture"}}},
    { 8, Control::noParent, {{"Soft Rock"},                 {"Soft Science"}}},
    { 9, Control::noParent, {{"Top 40"},                    {"Varied"}}},
    {10, Control::noParent, {{"Country Music"},             {"Popular Music (Pop)"}}},
    {11, Control::noParent, {{"Oldies (Music)"},            {"Rock Music"}}},
    {12, Control::noParent, {{"Soft Music"},                {"Easy Listening"}}},
    {13, Control::noParent, {{"Nostalgia"},                 {"Light Classical"}}},
    {14, Control::noParent, {{"Jazz"},                      {"Serious Classical"}}},
    {15, Control::noParent, {{"Classical"},                 {"Other Music"}}},
    {16, Control::noParent, {{"Rhythm & Blues"},            {"Weather"}}},
    {17, Control::noParent, {{"Soft Rhythm & Blues"},       {"Finance"}}},
    {18, Control::noParent, {{"Language"},                  {"Children's Programmes"}}},
    {19, Control::noParent, {{"Religious Music"},           {"Social Affairs"}}},
    {20, Control::noParent, {{"Religious Talk"},            {"Religion"}}},
    {21, Control::noParent, {{"Personality"},               {"Phone-in"}}},
    {22, Control::noParent, {{"Public"},                    {"Travel"}}},
    {23, Control::noParent, {{"College"},                   {"Leisure"}}},
    {24, Control::noParent, {{"Not assigned 1"},            {"Jazz Music"}}},
    {25, Control::noParent, {{"Not assigned 2"},            {"Country Music"}}},
    {26, Control::noParent, {{"Not assigned 3"},            {"National Music"}}},
    {27, Control::noParent, {{"Not assigned 4"},            {"Oldies Music"}}},
    {28, Control::noParent, {{"Not assigned 5"},            {"Folk Music"}}},
    {29, Control::noParent, {{"Weather"},                   {"Documentary"}}},
    {30, Control::noParent, {{"Emergency Test"},            {"Alarm Test"}}},
    {31, Control::noParent, {{"Emergency"},                 {"Alarm"}}},
};

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
        RdsText.AddHomeControls(homeTab);

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

        rdsPtyID = ESPUI.addControl(
                            ControlType::Select, 
                            RDS_PTY_CODE_STR, 
                            emptyString, 
                            ControlColor::Alizarin, 
                            rdsTab,
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cRadio *>(UserInfo)->CbSetPtyCode(sender, type);
                                }
                            },
                            this);
        ESPUI.setPanelStyle(rdsPtyID, "font-size: 1.15em;");
        ESPUI.setElementStyle(rdsPtyID, "color: black;");
        setPtyCodeOptionValues();

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

// ************************************************************************************************
void cRadio::setPtyCodeOptionValues ()
{
    // DEBUG_START;
    
    // DEBUG_V(String("emphVal: ") + String(emphVal));
    // DEBUG_V(String("M0: ") + ListOfPtyCodes[2].name[0]);
    // DEBUG_V(String("M1: ") + ListOfPtyCodes[2].name[1]);

    for (auto &CurrentEntry : ListOfPtyCodes)
    {
        // DEBUG_V(String("code: ") + String(CurrentEntry.code));

        if (Control::noParent == CurrentEntry.UiId)
        {
            // DEBUG_V("Create Option");
            CurrentEntry.UiId = ESPUI.addControl(
                                        ControlType::Option, 
                                        emptyString.c_str(), 
                                        emptyString, 
                                        ControlColor::None, 
                                        rdsPtyID);
        }
        Control *control = ESPUI.getControl(CurrentEntry.UiId);
        if(control)
        {
            // DEBUG_V("Update control fields");
            control->value = CurrentEntry.name[PreEmphasis.get()];
            control->label = CurrentEntry.name[PreEmphasis.get()].c_str();
            ESPUI.updateControl(control);
            // DEBUG_V(String("label: ") + String(control->label));
        }
        else
        {
            // DEBUG_V("Could not access control")
        }
    }

    updateUiPtyCode();
    ESPUI.jsonDom(0);

    // DEBUG_END;
}

// ************************************************************************************************
void cRadio::setPtyCode(String & ptyStr)
{
    // DEBUG_START;

    for (auto &CurrentEntry : ListOfPtyCodes)
    {
        if(ptyStr.equals(CurrentEntry.name[PreEmphasis.get()]))
        {
            PtyCode = CurrentEntry.code;
            setPtyCode();
            updateUiPtyCode();
        }
    }

    // DEBUG_END;
}

// ************************************************************************************************
void cRadio::updateUiPtyCode()
{
    // DEBUG_START;

    ESPUI.updateControlValue(rdsPtyID, ListOfPtyCodes[PtyCode].name[PreEmphasis.get()]);

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
