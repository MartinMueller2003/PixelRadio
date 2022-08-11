/*
   File: RfCarrier.cpp
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
#include "RfCarrier.hpp"
#include "QN8027RadioApi.hpp"
#include "language.h"
#include "memdebug.h"

static const PROGMEM char RADIO_RF_CARRIER_STR  [] = "RF CARRIER ENABLE";
static const PROGMEM char RADIO_OFF_AIR_STR     [] = "<font color=\"" COLOR_RED_STR "\">OFF AIR</font>";
static const PROGMEM char RADIO_ON_AIR_STR      [] = "<font color=\"" COLOR_GRN_STR "\">ON AIR</font>";
static const PROGMEM char RADIO_VOLT_STR        [] = "<font color=\"" COLOR_RED_STR "\">BAD-VDC</font>";
static const PROGMEM char RADIO_VSWR_STR        [] = "<font color=\"" COLOR_RED_STR "\">HI-VSWR</font>";
static const PROGMEM char RADIO_FAIL_STR        [] = "<font color=\"" COLOR_RED_STR "\">-FAIL-</font>";
static const PROGMEM char RADIO_RF_CARR_FLAG    [] = "RADIO_RF_CARR_FLAG";
static const PROGMEM char HOME_RAD_STAT_STR     [] = "RADIO STATUS";

// *********************************************************************************************
cRfCarrier::cRfCarrier()
{
    /// DEBUG_START;

    /// DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::AddControls (uint16_t value)
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
                                RADIO_RF_CARRIER_STR,
                                emptyString,
                                ControlColor::Emerald,
                                TabId,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cRfCarrier *>(UserInfo)->Callback(sender, type);
                                    }
                                },
                                this);

        MessageID = ESPUI.addControl(
                                ControlType::Label, 
                                RADIO_RF_CARRIER_STR, 
                                emptyString, 
                                ControlColor::Emerald, 
                                ControlId);
        ESPUI.setElementStyle(MessageID, CSS_LABEL_STYLE_BLACK);

        UpdateStatus();

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::AddHomeControls (uint16_t value)
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

        HomeMessageID = ESPUI.addControl(
                                ControlType::Label, 
                                HOME_RAD_STAT_STR, 
                                emptyString, 
                                ControlColor::Peterriver, 
                                HomeId);
        ESPUI.setPanelStyle(HomeMessageID, F("font-size: 3.0em;"));

        UpdateStatus();

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void cRfCarrier::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    set(type == S_ACTIVE);

    // DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    ReadFromJSON(Mode, config, RADIO_RF_CARR_FLAG);
    set(Mode);

    // DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    config[RADIO_RF_CARR_FLAG] = Mode;

    // DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::set(bool value)
{
    // DEBUG_START;

    if(value != Mode)
    {
        Mode = value;

        UpdateStatus();

        Log.infoln(String(F("RF Carrier Set to: %s.")).c_str(), String(value ? F("On") : F("Off")).c_str());
        displaySaveWarning();
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::UpdateStatus()
{
    // DEBUG_START;

    QN8027RadioApi.setRfCarrier(Mode);

    extern uint32_t paVolts;

    if (Mode)
    {
        cQN8027RadioApi::QN8027RadioFmTestStatus_e TestStatus = QN8027RadioApi.GetTestStatus();
        if (TestStatus == cQN8027RadioApi::QN8027RadioFmTestStatus_e::FM_TEST_FAIL)
        {
            ESPUI.print(MessageID, RADIO_FAIL_STR);   // Update homeTab panel.
            ESPUI.print(HomeMessageID, RADIO_FAIL_STR);   // Update homeTab panel.
        }
        else if (TestStatus == cQN8027RadioApi::QN8027RadioFmTestStatus_e::FM_TEST_VSWR) 
        {
            ESPUI.print(MessageID, RADIO_VSWR_STR);   // Update homeTab panel.
            ESPUI.print(HomeMessageID, RADIO_VSWR_STR);   // Update homeTab panel.
        }
        else if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX)) 
        {
            ESPUI.print(MessageID, RADIO_VOLT_STR);   // Update homeTab panel.
            ESPUI.print(HomeMessageID, RADIO_VOLT_STR);   // Update homeTab panel.
        }
        else 
        {
            ESPUI.print(MessageID, RADIO_ON_AIR_STR); // Update homeTab panel.
            ESPUI.print(HomeMessageID, RADIO_ON_AIR_STR); // Update homeTab panel.
        }
    }
    else 
    {
        ESPUI.print(MessageID, RADIO_OFF_AIR_STR); // Update homeTab panel.
        ESPUI.print(HomeMessageID, RADIO_OFF_AIR_STR); // Update homeTab panel.
    }

    ESPUI.updateControlValue(ControlId, Mode ? F("1") : F("0"));

    // DEBUG_END;
}

// *********************************************************************************************
cRfCarrier RfCarrier;

// *********************************************************************************************
// OEF
