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
        if(Control::noParent != diagTab)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        adjTab = _adjTab;

        ESPUI.addControl(ControlType::Separator, ADJUST_FRQ_CTRL_STR, emptyString, ControlColor::None, adjTab);

        String tempStr = String(float(fmFreqX10) / 10.0f, 1) + F(UNITS_MHZ_STR);
        adjFmDispID = ESPUI.addControl(
                                ControlType::Label, 
                                ADJUST_FM_FRQ_STR, 
                                tempStr, 
                                ControlColor::Wetasphalt, 
                                adjTab);
        ESPUI.setPanelStyle(adjFmDispID, F("font-size: 3.0em;"));
        ESPUI.setElementStyle(adjFmDispID, F("max-width: 75%;"));

        adjFreqID = ESPUI.addControl(
                            ControlType::Pad,
                            ADJUST_FRQ_ADJ_STR,
                            emptyString,
                            ControlColor::Wetasphalt,
                            adjTab,
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cRadio *>(UserInfo)->CbAdjFmFreq(sender, type);
                                }
                            },
                            this);

        ESPUI.addControl(ControlType::Separator, ADJUST_AUDIO_SEP_STR, "", ControlColor::None, adjTab);

        adjTestModeID = ESPUI.addControl(
                                ControlType::Switcher,
                                ADJUST_TEST_STR,
                                testModeFlg ? F("1") : F("0"),
                                ControlColor::Wetasphalt,
                                adjTab,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRadio *>(UserInfo)->CbTestMode(sender, type);
                                    }
                                },
                                this);

        adjMuteID = ESPUI.addControl(
                                ControlType::Switcher,
                                ADJUST_MUTE_STR,
                                muteFlg ? F("1") : F("0"),
                                ControlColor::Wetasphalt,
                                adjTab,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRadio *>(UserInfo)->CbMute(sender, type);
                                    }
                                },
                                this);
    ESPUI.setElementStyle(adjMuteID, (muteFlg ? F("background: red;") : F("background: #bebebe;")));
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
    extern uint32_t paVolts;
    
    do // once
    {
        if(Control::noParent != homeTab)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        homeTab = _homeTab;

        if (fmRadioTestCode == FM_TEST_FAIL)
        {
            tempStr = RADIO_FAIL_STR;
        }
        else if (fmRadioTestCode == FM_TEST_VSWR) 
        {
            tempStr = RADIO_VSWR_STR;
        }
        else if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX)) 
        {
            tempStr = RADIO_VOLT_STR;
        }
        else 
        {
            tempStr = rfCarrierFlg ? RADIO_ON_AIR_STR : RADIO_OFF_AIR_STR;
        }
        homeOnAirID = ESPUI.addControl(ControlType::Label, HOME_RAD_STAT_STR, tempStr, ControlColor::Peterriver, homeTab);
        ESPUI.setPanelStyle(homeOnAirID, "font-size: 3.0em;");

        tempStr    = String(float(fmFreqX10) / 10.0f, 1) + F(UNITS_MHZ_STR);
        homeFreqID = ESPUI.addControl(ControlType::Label,
                                    HOME_FREQ_STR,
                                    tempStr,
                                    ControlColor::Peterriver,
                                    homeTab);
        ESPUI.setPanelStyle(homeFreqID, "font-size: 3.0em;");
        ESPUI.setElementStyle(homeFreqID, "max-width: 80%;");

        ESPUI.addControl(ControlType::Separator, HOME_SEP_RDS_STR, emptyString, ControlColor::None, homeTab);

        tempStr       = HOME_RDS_WAIT_STR;
        homeRdsTextID = ESPUI.addControl(ControlType::Label, HOME_CUR_TEXT_STR, tempStr, ControlColor::Peterriver, homeTab);
        ESPUI.setPanelStyle(homeRdsTextID, String(F("font-size: 1.25em;")));

        homeTextMsgID = ESPUI.addControl(ControlType::Label, emptyString.c_str(), tempStr, ControlColor::Peterriver, homeRdsTextID);
        ESPUI.setPanelStyle(homeTextMsgID, String(F("font-size: 1.25em;")));

        homeRdsTmrID  = ESPUI.addControl(ControlType::Label, HOME_RDS_TIMER_STR, tempStr, ControlColor::Peterriver, homeTab);
        ESPUI.setPanelStyle(homeRdsTmrID, String(F("font-size: 1.25em;")));
        // ESPUI.setElementStyle(homeRdsTmrID, String(F("max-width: 30%;")));

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

        radioFreqID = ESPUI.addControl(
                                ControlType::Label,
                                RADIO_FM_FRQ_STR,
                                String(float(fmFreqX10) / 10.0f, 1) + F(UNITS_MHZ_STR),
                                ControlColor::Emerald,radioTab);
        ESPUI.setPanelStyle(radioFreqID, F("font-size: 3.0em;"));
        ESPUI.setElementStyle(radioFreqID, F("width: 75%;"));

        radioRfEnbID = ESPUI.addControl(
                                ControlType::Switcher,
                                RADIO_RF_CARRIER_STR,
                                rfCarrierFlg ? F("1") : F("0"),
                                ControlColor::Emerald,
                                radioTab,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRadio *>(UserInfo)->CbRfCarrier(sender, type);
                                    }
                                },
                                this);

        ESPUI.addControl(ControlType::Separator, RADIO_SEP_MOD_STR, emptyString, ControlColor::None, radioTab);

        radioAudioID = ESPUI.addControl(
                                ControlType::Switcher, 
                                RADIO_AUDIO_MODE_STR, 
                                stereoEnbFlg ? F("1") : F("0"),
                                ControlColor::Emerald, 
                                radioTab, 
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRadio *>(UserInfo)->CbAudioMode(sender, type);
                                    }
                                },
                                this);

        radioAudioMsgID = ESPUI.addControl(
                                    ControlType::Label, 
                                    RADIO_AUDIO_MODE_STR, 
                                    stereoEnbFlg ? F(RADIO_STEREO_STR) : F(RADIO_MONO_STR), 
                                    ControlColor::Emerald, 
                                    radioAudioID);
        ESPUI.setElementStyle(radioAudioMsgID, CSS_LABEL_STYLE_BLACK);

        #ifdef ADV_RADIO_FEATURES
        radioEmphID = ESPUI.addControl(
                                ControlType::Select,
                                RADIO_PRE_EMPH_STR,
                                preEmphasisStr,
                                ControlColor::Emerald,
                                radioTab,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRadio *>(UserInfo)->CbRadioEmphasis(sender, type);
                                    }
                                },
                                this);
        ESPUI.addControl(ControlType::Option, PRE_EMPH_USA_STR, PRE_EMPH_USA_STR, ControlColor::Emerald, radioEmphID);
        ESPUI.addControl(ControlType::Option, PRE_EMPH_EUR_STR, PRE_EMPH_EUR_STR, ControlColor::Emerald, radioEmphID);
        #endif // ifdef ADV_RADIO_FEATURES

        ESPUI.addControl(ControlType::Separator, RADIO_SEP_AUDIO_STR, emptyString,               ControlColor::None,    radioTab);

        radioVgaGainID = ESPUI.addControl(
                                    ControlType::Select, 
                                    RADIO_VGA_AUDIO_STR, 
                                    vgaGainStr, 
                                    ControlColor::Emerald, 
                                    radioTab,
                                    [](Control* sender, int type, void* UserInfo)
                                    {
                                        if(UserInfo)
                                        {
                                            static_cast<cRadio *>(UserInfo)->CbVgaGainAdjust(sender, type);
                                        }
                                    },
                                    this);
        ESPUI.addControl(ControlType::Option, VGA_GAIN0_STR, VGA_GAIN0_STR, ControlColor::Emerald, radioVgaGainID);
        ESPUI.addControl(ControlType::Option, VGA_GAIN1_STR, VGA_GAIN1_STR, ControlColor::Emerald, radioVgaGainID);
        ESPUI.addControl(ControlType::Option, VGA_GAIN2_STR, VGA_GAIN2_STR, ControlColor::Emerald, radioVgaGainID);
        ESPUI.addControl(ControlType::Option, VGA_GAIN3_STR, VGA_GAIN3_STR, ControlColor::Emerald, radioVgaGainID);
        ESPUI.addControl(ControlType::Option, VGA_GAIN4_STR, VGA_GAIN4_STR, ControlColor::Emerald, radioVgaGainID);
        ESPUI.addControl(ControlType::Option, VGA_GAIN5_STR, VGA_GAIN5_STR, ControlColor::Emerald, radioVgaGainID);

        radioImpID = ESPUI.addControl(
                                ControlType::Select, 
                                RADIO_INP_IMP_STR, 
                                inpImpedStr, 
                                ControlColor::Emerald, 
                                radioTab, 
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRadio *>(UserInfo)->CbImpedanceAdjust(sender, type);
                                    }
                                },
                                this);
        ESPUI.addControl(ControlType::Option, INP_IMP05K_STR, INP_IMP05K_STR, ControlColor::None, radioImpID);
        ESPUI.addControl(ControlType::Option, INP_IMP10K_STR, INP_IMP10K_STR, ControlColor::None, radioImpID);
        ESPUI.addControl(ControlType::Option, INP_IMP20K_STR, INP_IMP20K_STR, ControlColor::None, radioImpID);
        ESPUI.addControl(ControlType::Option, INP_IMP40K_STR, INP_IMP40K_STR, ControlColor::None, radioImpID);
        // DEBUG_V();

        #ifdef ADV_RADIO_FEATURES
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
        ESPUI.addControl(ControlType::Option, DIG_GAIN0_STR, DIG_GAIN0_STR, ControlColor::Emerald, radioDgainID);
        ESPUI.addControl(ControlType::Option, DIG_GAIN1_STR, DIG_GAIN1_STR, ControlColor::Emerald, radioDgainID);
        ESPUI.addControl(ControlType::Option, DIG_GAIN2_STR, DIG_GAIN2_STR, ControlColor::Emerald, radioDgainID);
        #endif // ifdef ADV_RADIO_FEATURES

        ESPUI.addControl(ControlType::Separator, RADIO_AMEAS_SEP_STR, emptyString,            ControlColor::None,    radioTab);

        radioSoundID = ESPUI.addControl(ControlType::Label, RADIO_AUDLVL_STR, F("0%"), ControlColor::Emerald, radioTab);
        ESPUI.setPanelStyle(radioSoundID, F("font-size: 1.25em;"));
        ESPUI.setElementStyle(radioSoundID, F("max-width: 35%;"));

        #ifdef ADV_RADIO_FEATURES
        String tempStr = String(getAudioGain()) + F(" dB");
        radioGainID = ESPUI.addControl(ControlType::Label, RADIO_AUDIO_GAIN_STR, tempStr, ControlColor::Emerald, radioTab);
        ESPUI.setPanelStyle(radioGainID, F("font-size: 1.15em;"));
        ESPUI.setElementStyle(radioGainID, F("width: 35%;"));
        ESPUI.print(radioGainID, tempStr);
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

        rdsProgNameID = ESPUI.addControl(
                                ControlType::Text, 
                                RDS_PROG_SERV_NM_STR, 
                                ProgramServiceName, 
                                ControlColor::Alizarin, 
                                rdsTab,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRadio *>(UserInfo)->CbProgramServiceName(sender, type);
                                    }
                                },
                                this);
        ESPUI.setPanelStyle(rdsProgNameID,   "font-size: 1.5em;");
        ESPUI.setElementStyle(rdsProgNameID, "color: black;");
        ESPUI.addControl(ControlType::Min, emptyString.c_str(), "4", ControlColor::None, rdsProgNameID);
        ESPUI.addControl(ControlType::Max, emptyString.c_str(), "8", ControlColor::None, rdsProgNameID);
        // DEBUG_V();

        rdsPiID = ESPUI.addControl(
                            ControlType::Text, 
                            RDS_PI_CODE_STR, 
                            String(F("0x")) + String(PiCode, HEX), 
                            ControlColor::Alizarin, 
                            rdsTab,
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cRadio *>(UserInfo)->CbSetPiCode(sender, type);
                                }
                            },
                            this);
        ESPUI.setPanelStyle(rdsPiID, "font-size: 1.25em;");
        ESPUI.setElementStyle(rdsPiID, "color: black;");

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
        ESPUI.setPanelStyle(rdsPiID, "font-size: 1.35em;");
        ESPUI.setElementStyle(rdsProgNameID, "color: black;");

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
            control->value = CurrentEntry.name[emphVal];
            control->label = CurrentEntry.name[emphVal].c_str();
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
        if(ptyStr.equals(CurrentEntry.name[emphVal]))
        {
            PtyCode = CurrentEntry.code;
            setPtyCode();
            updateUiPtyCode();
        }
    }

    // DEBUG_END;
}

// ************************************************************************************************
// updateUiRdsText(): Update the currently playing RDS RadioText shown in the Home tab's RDS text element.
void cRadio::updateUiRdsText(String & Text)
{
    // DEBUG_START;

    ESPUI.print(homeRdsTextID, String(rfCarrierFlg ? Text : RDS_RF_DISABLED_STR));
    ESPUI.print(homeTextMsgID, String(String(RdsMsgInfo.DurationMilliSec ? String(F("Controller: ")) + RdsMsgInfo.ControllerName : HOME_RDS_WAIT_STR)));

    // DEBUG_END;
}

// ************************************************************************************************
// updateRdsMsgRemainingTime(): Updates the GUI's RDS time on homeTab.
//                    Test mode clears the time field.
void cRadio::updateRdsMsgRemainingTime(unsigned long now)
{
    // DEBUG_START;

    uint32_t timeCnt = 0;

    do // once
    {
        if (IsTestModeOn())
        {
            // DEBUG_V("Test Mode");
            ESPUI.print(homeRdsTmrID, String(F("Test Mode")));
            break;
        }

        if (!rfCarrierFlg)
        {
            // DEBUG_V("No Carrier");
            ESPUI.print(homeRdsTmrID, RDS_DISABLED_STR);
            break;
        }

        if(0 == RdsMsgInfo.DurationMilliSec)
        {
            // DEBUG_V("No Message to send");
            ESPUI.print(homeRdsTmrID, HOME_RDS_WAIT_STR);
            break;
        }

        if(now > CurrentMsgEndTime)
        {
            // DEBUG_V("Timed Out");
            ESPUI.print(homeRdsTmrID, RDS_EXPIRED_STR);
            break;
        }


        unsigned long TimeRemaining = ((CurrentMsgEndTime - now) + 999) / 1000;
        // DEBUG_V(String("Update Timer: ") + String(TimeRemaining));
        ESPUI.print(homeRdsTmrID, String(TimeRemaining) + F(" Secs"));

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void cRadio::updateUiPtyCode()
{
    // DEBUG_START;

    ESPUI.updateControlValue(rdsPtyID, ListOfPtyCodes[PtyCode].name[emphVal]);

    // DEBUG_END;
}

// ************************************************************************************************
// updateUiAudioLevel(): Update the diagTab UI's Audio Level (mV).
void cRadio::updateUiAudioLevel(void)
{
    // DEBUG_START;

    uint16_t mV;
    static uint32_t previousMillis = 0;
    String tempStr;

    if (previousMillis == 0)
    {
        previousMillis = millis(); // Initialize First entry;
    }
    else if (millis() - previousMillis >= AUDIO_MEAS_TIME) 
    {
        previousMillis = millis();
        mV             = measureAudioLevel();

        if (mV >= AUDIO_LEVEL_MAX) 
        {
            tempStr = ">";
        }

        tempStr += String(mV);
        tempStr += F("mV");
        ESPUI.print(radioSoundID, tempStr);
        Log.verboseln(String(F("Peak Audio Amplitude %03umV.")).c_str(), mV);
    }

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
