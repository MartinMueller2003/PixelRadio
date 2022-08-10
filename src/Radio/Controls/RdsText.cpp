/*
   File: RdsText.cpp
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
#include "RdsText.hpp"
#include "RfCarrier.hpp"
#include "TestTone.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static const PROGMEM char HOME_RDS_WAIT_STR     [] = "Waiting for RDS RadioText ...";
static const PROGMEM char HOME_CUR_TEXT_STR     [] = "CURRENT RADIOTEXT";
static const PROGMEM char HOME_RDS_TIMER_STR    [] = "RDS TIMER COUNTDOWN";
static const PROGMEM char RDS_RF_DISABLED_STR   [] = "{ RADIO CARRIER OFF }";
static const PROGMEM char RDS_DISABLED_STR      [] = "{ DISABLED }";
static const PROGMEM char RDS_EXPIRED_STR       [] = "{ EXPIRED }";

// *********************************************************************************************
cRdsText::cRdsText()
{
    /// DEBUG_START;

    /// DEBUG_END;
}

// *********************************************************************************************
void cRdsText::AddHomeControls (uint16_t value)
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

        homeRdsTextID = ESPUI.addControl(
                                ControlType::Label, 
                                HOME_CUR_TEXT_STR, 
                                emptyString, 
                                ControlColor::Peterriver, 
                                HomeId);
        ESPUI.setPanelStyle(homeRdsTextID, String(F("font-size: 1.25em;")));

        homeTextMsgID = ESPUI.addControl(
                                ControlType::Label, 
                                emptyString.c_str(), 
                                emptyString, 
                                ControlColor::Peterriver, 
                                homeRdsTextID);
        ESPUI.setPanelStyle(homeTextMsgID,   String(F("font-size: 1.15em;")));
        ESPUI.setElementStyle(homeTextMsgID, CSS_LABEL_STYLE_WHITE);

        homeRdsTmrID = ESPUI.addControl(
                                ControlType::Label, 
                                HOME_RDS_TIMER_STR, 
                                emptyString, 
                                ControlColor::Peterriver, 
                                HomeId);
        ESPUI.setPanelStyle(homeRdsTmrID, String(F("font-size: 1.25em;")));
        // ESPUI.setElementStyle(homeRdsTmrID, String(F("max-width: 30%;")));

        UpdateStatus();

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cRdsText::poll()
{
    /// DEBUG_START;

    unsigned long now = millis();

    do // once
    {
        if (TestTone.get())
        {
            // dont output anything when test mode is running
            break;
        }

        // has it been one second since the last update?
        if (1000 > (now - CurrentMsgLastUpdateTime))
        {
            // need to wait a bit longer
            break;
        }
        CurrentMsgLastUpdateTime = now;
        /// DEBUG_V("One second later");
        
        UpdateStatus();

        // has the current message output expired?
        if(now < CurrentMsgEndTime)
        {
            /// DEBUG_V("still waiting");
            updateRdsMsgRemainingTime(now);
            break;
        }

        /// DEBUG_V("time for a new message");
        ControllerMgr.GetNextRdsMessage(RdsMsgInfo);
        CurrentMsgEndTime = now + RdsMsgInfo.DurationMilliSec;
        if(!LastMessageSent.equals(RdsMsgInfo.Text))
        {
            /// DEBUG_V("Display the new message");
            LastMessageSent = RdsMsgInfo.Text;
            Log.traceln(String(F("Refreshing RDS RadioText Message: %s")).c_str(), RdsMsgInfo.Text.c_str());
            set(RdsMsgInfo.Text);
        }
        updateRdsMsgRemainingTime(now);

    } while (false);

    /// DEBUG_END;
}

// *********************************************************************************************
void cRdsText::set(String & value)
{
    // DEBUG_START;
    // DEBUG_V(String("value: ") + value);

    LastMessageSent = value;
    QN8027RadioApi.setRdsMessage(LastMessageSent);
    UpdateStatus();

    // DEBUG_END;
}

// *********************************************************************************************
void cRdsText::UpdateStatus()
{
    // DEBUG_START;

    if (TestTone.get())
    {
        // DEBUG_V("Test Mode");
        ESPUI.print(homeRdsTextID, LastMessageSent);
        ESPUI.print(homeTextMsgID, emptyString);
    }
    else
    {
        ESPUI.print(homeRdsTextID, String(RfCarrier.get() ? LastMessageSent : RDS_RF_DISABLED_STR));
        ESPUI.print(homeTextMsgID, String(String(RdsMsgInfo.DurationMilliSec ? String(F("Controller: ")) + RdsMsgInfo.ControllerName : HOME_RDS_WAIT_STR)));
    }

    // DEBUG_END;
}

// ************************************************************************************************
// updateRdsMsgRemainingTime(): Updates the GUI's RDS time on homeTab.
//                    Test mode clears the time field.
void cRdsText::updateRdsMsgRemainingTime(uint32_t now)
{
    // DEBUG_START;

    uint32_t timeCnt = 0;

    do // once
    {
        if (TestTone.get())
        {
            // DEBUG_V("Test Mode");
            ESPUI.print(homeRdsTmrID, String(F("Test Mode")));
            break;
        }

        if (!RfCarrier.get())
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

// *********************************************************************************************
cRdsText RdsText;

// *********************************************************************************************
// OEF
