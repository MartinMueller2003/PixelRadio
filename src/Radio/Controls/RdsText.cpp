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
#include "language.h"
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include "RdsText.hpp"
#include "RfCarrier.hpp"
#include "TestTone.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String     HOME_RDS_WAIT_STR       = "Waiting for RDS RadioText ...";
static const PROGMEM String     HOME_CUR_TEXT_STR       = "CURRENT RADIOTEXT";
static const PROGMEM String     HOME_RDS_TIMER_STR      = "RDS TIMER COUNTDOWN";
static const PROGMEM String     RDS_RF_DISABLED_STR     = "{ RADIO CARRIER OFF }";
static const PROGMEM String     RDS_DISABLED_STR        = "{ DISABLED }";
static const PROGMEM String     RDS_EXPIRED_STR         = "{ EXPIRED }";

// *********************************************************************************************
cRdsText::cRdsText () : cOldControlCommon (emptyString)
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cRdsText::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Label, color);
    ESPUI.updateControlLabel (ControlId, HOME_CUR_TEXT_STR.c_str ());
    ESPUI.setPanelStyle (ControlId, String (F ("font-size: 1.25em;")));
    ESPUI.setElementStyle (ControlId, CSS_LABEL_STYLE_WHITE);

    ESPUI.setPanelStyle (StatusMessageId, String (F ("font-size: 1.15em;")));
    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_WHITE);

    homeRdsTmrID = ESPUI.addControl (ControlType::Label,
            HOME_RDS_TIMER_STR.c_str (),
            emptyString,
            color,
            value);
    ESPUI.setPanelStyle (homeRdsTmrID, String (F ("font-size: 1.25em;")));

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
        if (TestTone.get ())
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
        // _ DEBUG_V("One second later");

        UpdateStatus ();

        // has the current message output expired?
        if (now < CurrentMsgEndTime)
        {
            // _ DEBUG_V("still waiting");
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
        updateRdsMsgRemainingTime (now);
    } while (false);

    // _ DEBUG_END;
}

// *********************************************************************************************
bool cRdsText::set (String &value, String &)
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

    if (TestTone.get ())
    {
        // DEBUG_V("Test Mode");
        ESPUI.  print ( ControlId,              LastMessageSent);
        ESPUI.  print ( StatusMessageId,        emptyString);
    }
    else
    {
        ESPUI.  print ( ControlId, String (RfCarrier.get () ? LastMessageSent : RDS_RF_DISABLED_STR));
        ESPUI.  print ( StatusMessageId,
            String (String (RdsMsgInfo.DurationMilliSec ? String (F ("Controller: ")) + RdsMsgInfo.ControllerName : HOME_RDS_WAIT_STR)));
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
        if (TestTone.get ())
        {
            // DEBUG_V("Test Mode");
            ESPUI.print (homeRdsTmrID, String (F ("Test Mode")));
            break;
        }

        if (!RfCarrier.get ())
        {
            // DEBUG_V("No Carrier");
            ESPUI.print (homeRdsTmrID, RDS_DISABLED_STR);
            break;
        }

        if (0 == RdsMsgInfo.DurationMilliSec)
        {
            // DEBUG_V("No Message to send");
            ESPUI.print (homeRdsTmrID, HOME_RDS_WAIT_STR);
            break;
        }

        if (now > CurrentMsgEndTime)
        {
            // DEBUG_V("Timed Out");
            ESPUI.print (homeRdsTmrID, RDS_EXPIRED_STR);
            break;
        }
        unsigned long TimeRemaining = ((CurrentMsgEndTime - now) + 999) / 1000;
        // DEBUG_V(String("Update Timer: ") + String(TimeRemaining));
        ESPUI.print (homeRdsTmrID, String (TimeRemaining) + F (" Secs"));
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
cRdsText RdsText;

// *********************************************************************************************
// OEF
