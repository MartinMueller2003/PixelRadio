/*
   File: TestTone.cpp
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
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include "RdsText.hpp"
#include "RfCarrier.hpp"
#include "TestTone.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <vector>

// *********************************************************************************************
static const PROGMEM String     ADJUST_TEST_STR = "TEST TONES";
static const PROGMEM String     AUDIO_TEST_STR  = "PIXELRADIO AUDIO TEST";
static const PROGMEM String     AUDIO_PSN_STR   = "TestTone";

// Test Tone
const uint8_t TEST_TONE_CHNL    = 0;    // Test Tone PWM Channel.
const uint32_t  TEST_TONE_TIME  = 2000; // Test Tone Sequence Time, in mS.
const uint16_t  TONE_A3         = 220;  // 220Hz Audio Tone.
const uint16_t  TONE_A4         = 440;
const uint16_t  TONE_C4         = 262;
const uint16_t  TONE_C5         = 523;
const uint16_t  TONE_E4         = 330;
const uint16_t  TONE_F4         = 349;
const uint16_t  TONE_NONE       = 0;

const int       TONE_OFF        = 1;
const int       TONE_ON         = 0;

static std::vector <uint16_t> toneList
{
    TONE_NONE,
    TONE_A3,
    TONE_E4,
    TONE_A3,
    TONE_C4,
    TONE_C5,
    TONE_F4,
    TONE_F4,
    TONE_A4,
};

fsm_Tone_state_Idle fsm_Tone_state_Idle_imp;
fsm_Tone_state_SendingTone fsm_Tone_state_SendingTone_imp;

// *********************************************************************************************
cTestTone::cTestTone () : cOldControlCommon (emptyString)
{
    // _ DEBUG_START;

    DataValue           = 0;
    DataValueStr        = "off";

    ActiveLabelStyle    = CSS_LABEL_STYLE_WHITE;
    InactiveLabelStyle  = CSS_LABEL_STYLE_WHITE;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cTestTone::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;


    cOldControlCommon::AddControls (value, ControlType::Switcher, color);
    ESPUI.updateControlLabel (ControlId, ADJUST_TEST_STR.c_str ());
    ESPUI.setElementStyle (StatusMessageId, CSS_LABEL_STYLE_WHITE);

    // DEBUG_END;
}

// *********************************************************************************************
bool cTestTone::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();
    uint32_t NewDataValue;

    do  // once
    {
        if (value.equals (F ("0")) ||
            value.equals (F ("off")))
        {
            NewDataValue = false;
        }
        else if (value.equals (F ("1")) ||
            value.equals (F ("on")))
        {
            NewDataValue = true;
        }
        else
        {
            ResponseMessage     = String (F ("Test Tone: BAD_VALUE: ")) + value;
            Response            = false;
            break;
        }

        if (NewDataValue == DataValue)
        {
            // DEBUG_V("Skip duplicate setting");
            break;
        }
        DataValue       = NewDataValue;
        DataValueStr    = DataValue ? F ("1") : F ("0");

        ESPUI.updateControlValue (ControlId, DataValueStr);

        Log.infoln (String (F ("Test Mode Set to: %s.")).c_str (), String (DataValue ? F ("On") : F ("Off")).c_str ());
        // displaySaveWarning();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cTestTone::Init ()
{
    // DEBUG_START;

    pinMode (MUX_PIN, OUTPUT);          // Audio MUX Control (Line-In:Tone-In), Output.
    digitalWrite (MUX_PIN, TONE_OFF);   // Init Audio Mux, Enable Audio Line-In Jack, Music LED On.
    ledcSetup (TEST_TONE_CHNL, 1000, 8);

    FsmTimerExpirationTime = millis () + 1000;

    fsm_Tone_state_Idle_imp.SetParent (this);
    fsm_Tone_state_SendingTone_imp.SetParent (this);
    pCurrentFsmState = &fsm_Tone_state_Idle_imp;

    // DEBUG_END;
}

// *********************************************************************************************
void cTestTone::poll ()
{
    // _ DEBUG_START;

    uint32_t now = millis ();

    do  // once
    {
        if (now < FsmTimerExpirationTime)
        {
            // keep waiting
            break;
        }
        // do not use 'now' here or you will slip time
        FsmTimerExpirationTime += 1000;

        pCurrentFsmState->Poll (now);
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cTestTone::toneOff ()
{
    // DEBUG_START;

    ledcDetachPin (TONE_PIN);
    ledcWrite (TEST_TONE_CHNL, 0);

    // Allow a bit of time for tone channel to shutdown.
    delay (5);

    // DEBUG_END;
}

// *********************************************************************************************
void cTestTone::toneOn (uint16_t freq)
{
    // DEBUG_START;

    if (ledcRead (TEST_TONE_CHNL))
    {
        Log.warningln (String (F ("Ignored Tone Request: Channel %u is already in-use")).c_str (), TEST_TONE_CHNL);
    }
    else if (0 == freq)
    {
        toneOff ();
    }
    else
    {
        ledcAttachPin (TONE_PIN, TEST_TONE_CHNL);
        ledcWriteTone (TEST_TONE_CHNL, freq);
    }
    // DEBUG_END;
}

// *********************************************************************************************
void cTestTone::UpdateRdsTimeMsg ()
{
    // DEBUG_START;

    String FrequencyMessage;

    FrequencyMessage.reserve (128);

    char rdsBuff[25];

    // DEBUG_V("Update the test tone clock. HH:MM:SS will be sent as RadioText.");
    seconds++;

    FrequencyMessage = String (F ("Current Tone: ")) + String (pCurrentFsmState->getCurrentToneFrequency ()) + " hz";
    ESPUI.print (StatusMessageId, FrequencyMessage);
    sprintf (rdsBuff, "[ %02u:%02u:%02u ]", hours, minutes, seconds);
    String tmpStr;

    tmpStr.reserve (128);
    tmpStr = String (AUDIO_TEST_STR) + rdsBuff + String (F ("<br>")) + FrequencyMessage;
    String dummy;

    RdsText.set (tmpStr, dummy);
    tmpStr = AUDIO_PSN_STR;
    QN8027RadioApi.setProgramServiceName (tmpStr, RfCarrier.get ());

    if (seconds >= 60)
    {
        seconds = 0;
        minutes++;

        if (minutes >= 60)
        {
            minutes = 0;
            hours++;

            if (hours >= 100)
            {   // Clock wraps at 99:59:59.
                hours = 0;
            }
        }
    }
    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_Tone_state_Idle::Init ()
{
    // DEBUG_START;

    // DEBUG_V("Kill active tone generator.");
    pTestTone->toneOff ();
    digitalWrite (MUX_PIN, TONE_OFF);   // Switch Audio Mux chip to Line-In.

    ESPUI.setElementStyle (pTestTone->ControlId, String (F ("background: #bebebe;")));
    ESPUI.print (pTestTone->StatusMessageId, emptyString);

    pTestTone->pCurrentFsmState = &fsm_Tone_state_Idle_imp;

    // DEBUG_END;
}

/*****************************************************************************/
void fsm_Tone_state_Idle::Poll (uint32_t)
{
    // DEBUG_START;

    if (pTestTone->DataValue)
    {
        fsm_Tone_state_SendingTone_imp.Init ();
    }
    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_Tone_state_SendingTone::Init ()
{
    // DEBUG_START;

    pTestTone->hours    = 0;
    pTestTone->minutes  = 0;
    pTestTone->seconds  = 0;

    pTestTone->toneOff ();
    digitalWrite (MUX_PIN, TONE_ON);    // Switch Audio Mux chip to Test Tones.

    CurrentTone         = toneList.begin ();
    ToneExpirationTime  = millis () + TEST_TONE_TIME;

    ESPUI.setElementStyle (pTestTone->ControlId, String (F ("background: red;")));

    pTestTone->pCurrentFsmState = &fsm_Tone_state_SendingTone_imp;

    // DEBUG_END;
}

/*****************************************************************************/
void fsm_Tone_state_SendingTone::Poll (uint32_t now)
{
    // DEBUG_START;

    do  // once
    {
        if (!pTestTone->DataValue)
        {
            fsm_Tone_state_Idle_imp.Init ();
            break;
        }
        pTestTone->UpdateRdsTimeMsg ();

        if (now < ToneExpirationTime)
        {
            // DEBUG_V("Need to wait longer");
            break;
        }
        ToneExpirationTime += TEST_TONE_TIME;

        pTestTone->toneOff ();

        if (toneList.begin () == CurrentTone)
        {
            Log.verboseln (String (F ("New Test Tone Sequence, RadioText Sent.")).c_str ());
        }
        pTestTone->toneOn (*CurrentTone);

        // move to the next tone
        ++CurrentTone;

        if (toneList.end () == CurrentTone)
        {
            CurrentTone = toneList.begin ();
        }
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
cTestTone TestTone;

// *********************************************************************************************
// OEF
