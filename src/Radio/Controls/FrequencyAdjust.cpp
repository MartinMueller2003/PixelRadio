/*
   File: FrequencyAdjust.cpp
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
#include "language.h"
#include "FrequencyAdjust.hpp"
#include "RfCarrier.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"


static const PROGMEM char RADIO_FM_FRQ_STR      [] = "FM FREQUENCY";
static const PROGMEM char ADJUST_FRQ_ADJ_STR    [] = "FREQUENCY ADJUST";
static const PROGMEM char RADIO_FM_FREQ         [] = "RADIO_FM_FREQ";
static const PROGMEM char UNITS_MHZ_STR         [] = " MHz";
static const PROGMEM char HOME_RAD_STAT_STR     [] = "RADIO STATUS";

// *********************************************************************************************
cFrequencyAdjust::cFrequencyAdjust()
{
    /// DEBUG_START;

    /// DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddRadioControls (uint16_t value)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != RadioId)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        RadioId = value;

        RadioStatusID = ESPUI.addControl(
                                ControlType::Label,
                                RADIO_FM_FRQ_STR,
                                emptyString,
                                ControlColor::Emerald,
                                RadioId);
        ESPUI.setPanelStyle(RadioStatusID, F("font-size: 3.0em;"));
        ESPUI.setElementStyle(RadioStatusID, F("width: 75%;"));

        UpdateStatus();

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddHomeControls (uint16_t value)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != HomeId)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        HomeId = value;

        HomeStatusID = ESPUI.addControl(
                                ControlType::Label, 
                                HOME_RAD_STAT_STR, 
                                emptyString, 
                                ControlColor::Peterriver, 
                                HomeId);
        ESPUI.setPanelStyle(HomeStatusID, F("font-size: 3.0em;"));

        UpdateStatus();

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddAdjustControls (uint16_t value)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != AdjustId)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        AdjustId = value;

        AdjustStatusID = ESPUI.addControl(
                                ControlType::Label, 
                                RADIO_FM_FRQ_STR, 
                                emptyString, 
                                ControlColor::Wetasphalt, 
                                AdjustId);
        ESPUI.setPanelStyle(AdjustStatusID, F("font-size: 3.0em;"));
        ESPUI.setElementStyle(AdjustStatusID, F("max-width: 75%;"));

        ControlId = ESPUI.addControl(
                            ControlType::Pad,
                            ADJUST_FRQ_ADJ_STR,
                            emptyString,
                            ControlColor::Wetasphalt,
                            AdjustId,
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cFrequencyAdjust *>(UserInfo)->Callback(sender, type);
                                }
                            },
                            this);
        UpdateStatus();

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void cFrequencyAdjust::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("    value: ") + String(sender->value));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String("     data: ") + String(data));

    uint32_t NewData = data;
    do // once
    {
        if(0 <= type)
        {
            // DEBUG_V("Ignore button up event");
            break;
        }

        if (type == P_LEFT_DOWN) 
        {
            // DEBUG_V("Decr 100khz");
            NewData -= FM_FREQ_SKP_KHZ;
        }
        else if (type == P_RIGHT_DOWN) 
        {
            // DEBUG_V("Inc 100khz");
            NewData += FM_FREQ_SKP_KHZ;
        }
        else if (type == P_BACK_DOWN) 
        {
            // DEBUG_V("Dec 1Mhz");
            NewData -= FM_FREQ_SKP_MHZ;
        }
        else if (type == P_FOR_DOWN) 
        {
            // DEBUG_V("Inc 1Mhz");
            NewData += FM_FREQ_SKP_MHZ;
        }
        else
        {
            Log.errorln(String(F("CbAdjFmFreq: %s.")).c_str(), BAD_VALUE_STR);
            break;
        }
    } while (false);

    // DEBUG_V(String("data: ") + String(data));

    if (NewData > FM_FREQ_MAX_X10)
    {
        // DEBUG_V("Frequency is too high");
        NewData = FM_FREQ_MAX_X10;
    }
    else if (NewData < FM_FREQ_MIN_X10) 
    {
        // DEBUG_V("Frequency is too ;ow");
        NewData = FM_FREQ_MIN_X10;
    }

    // DEBUG_V(String("fmFreqX10: ") + String(data));

    set(NewData);
    displaySaveWarning();
    Log.infoln(String(F("FM Frequency Set to: %s.")).c_str(), (String(float(data) / 10.0f, 1) + F(UNITS_MHZ_STR)).c_str());

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    ReadFromJSON(data, config, RADIO_FM_FREQ);
    set(data);

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    config[RADIO_FM_FREQ] = data;

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::set(uint32_t value)
{
    // DEBUG_START;

    if(value != data)
    {
        data = value;

        UpdateStatus();

        Log.infoln(String(F("RF Frequency Set to: %.1f Mhz")).c_str(), float(data) / 10.0f);
        displaySaveWarning();
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::UpdateStatus()
{
    // DEBUG_START;

    float tempFloat = float(data) / 10.0f;

    QN8027RadioApi.setFrequency(tempFloat, RfCarrier.get());

    String tempStr = String(tempFloat, 1) + F(UNITS_MHZ_STR);
    // DEBUG_V(String("tempFloat: ") + String(tempFloat));

    ESPUI.print(AdjustStatusID, tempStr);
    ESPUI.print(HomeStatusID,   tempStr);
    ESPUI.print(RadioStatusID,  tempStr);

    // DEBUG_END;
}

// *********************************************************************************************
cFrequencyAdjust FrequencyAdjust;

// *********************************************************************************************
// OEF
