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

#include "RdsText.hpp"
#include "RdsTextStatus.hpp"
#include "language.h"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "TestTone.hpp"
#include "memdebug.h"

static const PROGMEM char   HOME_RDS_WAIT_STR       []  = "Waiting for RDS RadioText ...";
static const PROGMEM char   HOME_CUR_TEXT_STR       []  = "CURRENT RADIOTEXT";
static const PROGMEM char   HOME_RDS_TIMER_STR      []  = "RDS TIMER COUNTDOWN";
static const PROGMEM char   RDS_RF_DISABLED_STR     []  = "{ RADIO CARRIER OFF }";
static const PROGMEM char   RDS_DISABLED_STR        []  = "{ DISABLED }";
static const PROGMEM char   RDS_EXPIRED_STR         []  = "{ EXPIRED }";

// *********************************************************************************************
cRdsText::cRdsText () :   cStatusControl (HOME_CUR_TEXT_STR)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cRdsText::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cStatusControl::AddControls (value, color);
    setControlStyle (eCssStyle::CssStyleWhite);
    setMessageStyle (eCssStyle::CssStyleWhite);

    RdsTextStatus.AddControls (value, color);

    UpdateStatus ();

    // DEBUG_END;
}

// *********************************************************************************************
void cRdsText::poll ()
{
    // _ DEBUG_START;

    unsigned long now = millis ();

    do  // once
    {
        // has it been one second since the last update?
        if (1000 > (now - CurrentMsgLastUpdateTime))
        {
            // need to wait a bit longer
            break;
        }

        if (TestTone.getBool ())
        {
            // DEBUG_V();
            CurrentMsgLastUpdateTime += 1000;
            updateRdsMsgRemainingTime (now);
            break;
        }
        CurrentMsgLastUpdateTime = now;
        // _ DEBUG_V("One second later");

        UpdateStatus ();

        // has the current message output expired?
        if (now < CurrentMsgEndTime)
        {
            // DEBUG_V("still waiting");
            updateRdsMsgRemainingTime (now);
            break;
        }
        // _ DEBUG_V("time for a new message");
        ControllerMgr.GetNextRdsMessage (RdsMsgInfo);
        CurrentMsgEndTime = now + RdsMsgInfo.DurationMilliSec;

        if (!LastMessageSent.equals (RdsMsgInfo.Text))
        {
            // _ DEBUG_V("Display the new message");
            LastMessageSent = RdsMsgInfo.Text;
            Log.traceln (String (F ("Refreshing RDS RadioText Message: %s")).c_str (), RdsMsgInfo.Text.c_str ());
            String dummy;
            set (RdsMsgInfo.Text, dummy);
        }
        // DEBUG_V();
        updateRdsMsgRemainingTime (now);
    } while (false);

    // _ DEBUG_END;
}

// *********************************************************************************************
bool cRdsText::set (String & value, String &)
{
    // DEBUG_START;
    // DEBUG_V(String("value: ") + value);

    LastMessageSent = value;
    QN8027RadioApi.setRdsMessage (LastMessageSent);
    UpdateStatus ();

    // DEBUG_END;
    return true;
}

// *********************************************************************************************
void cRdsText::UpdateStatus ()
{
    // DEBUG_START;

    if (TestTone.getBool ())
    {
        // DEBUG_V("Test Mode");
        setControl (LastMessageSent, eCssStyle::CssStyleMaroon);
        setMessage (emptyString, eCssStyle::CssStyleTransparent);
    }
    else    // not in test mode
    {
        setControl (String (RfCarrier.get () ? LastMessageSent : RDS_RF_DISABLED_STR), eCssStyle::CssStyleWhite);
        String TempMsg;
        TempMsg.reserve (128);

        TempMsg = RdsMsgInfo.DurationMilliSec ? String (F ("Controller: ")) + RdsMsgInfo.ControllerName : HOME_RDS_WAIT_STR;
        setMessage (TempMsg, eCssStyle::CssStyleWhite);
    }
    // DEBUG_END;
}

// ************************************************************************************************
void cRdsText::updateRdsMsgRemainingTime (uint32_t now)
{
    // DEBUG_START;

    uint32_t timeCnt = 0;

    do  // once
    {
        if (TestTone.getBool ())
        {
            // DEBUG_V("Test Mode");
            RdsTextStatus.set (String (F ("Test Mode")));
            break;
        }

        if (!RfCarrier.getBool ())
        {
            // DEBUG_V("No Carrier");
            RdsTextStatus.set (RDS_DISABLED_STR);
            break;
        }

        if (0 == RdsMsgInfo.DurationMilliSec)
        {
            // DEBUG_V("No Message to send");
            RdsTextStatus.set (HOME_RDS_WAIT_STR);
            break;
        }

        if (now > CurrentMsgEndTime)
        {
            // DEBUG_V("Timed Out");
            RdsTextStatus.set (RDS_EXPIRED_STR);
            break;
        }
        unsigned long TimeRemaining = ((CurrentMsgEndTime - now) + 999) / 1000;
        // DEBUG_V(String("Update Timer: ") + String(TimeRemaining));
        RdsTextStatus.set (String (TimeRemaining) + F (" Secs"));
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
cRdsText RdsText;

// *********************************************************************************************
// OEF
