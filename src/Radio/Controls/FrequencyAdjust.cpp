/*
  *    File: FrequencyAdjust.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>

#include "FrequencyAdjust.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "memdebug.h"

static const PROGMEM char   ADJUST_FRQ_ADJ_STR      []  = "FREQUENCY ADJUST";
static const PROGMEM char   RADIO_FM_FREQ           []  = "RADIO_FM_FREQ";
static const PROGMEM char   UNITS_MHZ_STR           []  = " MHz";

const float FM_FREQ_DEF         = 88.7f;    // 88.7MHz FM.
const float FM_FREQ_MAX         = 107.9f;   // 107.9MHz FM.
const float FM_FREQ_MIN         = 88.1f;    // 88.1MHz FM.
const float FM_FREQ_SKP_KHZ_10X = 1;        // 100Khz.
const float FM_FREQ_SKP_MHZ_10X = 10;       // 1MHz.

// *********************************************************************************************
cFrequencyAdjust::cFrequencyAdjust ()  :
    cControlCommonMsg (RADIO_FM_FREQ, ControlType::Pad, ADJUST_FRQ_ADJ_STR, String (FM_FREQ_DEF), 5)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    UpdateStatus (true, true);

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddRadioControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    RadioFreqStatus.AddControls (value, color);
    UpdateStatus (true, true);

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddHomeControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    HomeFreqStatus.AddControls (value, color);
    UpdateStatus (true, true);

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddAdjustControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    AdjustFreqStatus.AddControls (value, color);
    cControlCommonMsg::AddControls (value, color);
    UpdateStatus (true, true);

    // DEBUG_END;
}

// ************************************************************************************************
void cFrequencyAdjust::Callback (Control * sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + String(sender->value));
    // DEBUG_V(String("        type: ") + String(type));
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);

    do  // once
    {
        if (0 <= type)
        {
            // DEBUG_V("Ignore button up event");
            break;
        }

        uint32_t NewData = uint32_t (GetDataValueStr ().toFloat () * 10.0f);
        // DEBUG_V(String("NewData: ") + String(NewData));

        if (type == P_LEFT_DOWN)
        {
            // DEBUG_V("Decr 100khz");
            NewData -= FM_FREQ_SKP_KHZ_10X;
        }
        else if (type == P_RIGHT_DOWN)
        {
            // DEBUG_V("Inc 100khz");
            NewData += FM_FREQ_SKP_KHZ_10X;
        }
        else if (type == P_BACK_DOWN)
        {
            // DEBUG_V("Dec 1Mhz");
            NewData -= FM_FREQ_SKP_MHZ_10X;
        }
        else if (type == P_FOR_DOWN)
        {
            // DEBUG_V("Inc 1Mhz");
            NewData += FM_FREQ_SKP_MHZ_10X;
        }

        String DataStr = String (float(NewData) / 10.0f, 1);
        // DEBUG_V(String("DataStr: ") + DataStr);
        String ResponseMessage;

        if (set (DataStr, ResponseMessage, false, false))
        {
            setMessage (emptyString, eCssStyle::CssStyleTransparent);
        }
        else
        {
            setMessage (ResponseMessage, eCssStyle::CssStyleRed);
        }
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
bool cFrequencyAdjust::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("       value: ") + value);

    bool Response = cControlCommonMsg::set (value, ResponseMessage, SkipLogOutput, ForceUpdate);

    if (Response)
    {
        float tempFloat = GetDataValueStr ().toFloat ();
        QN8027RadioApi.setFrequency (tempFloat, RfCarrier.get ());

        // DEBUG_V();
        UpdateStatus (SkipLogOutput, ForceUpdate);
    }

    // DEBUG_V(String("   DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("      DataValue: ") + String(DataValue));
    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V(String("       Response: ") + String(Response));

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
bool cFrequencyAdjust::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = true;

    // DEBUG_V(String("DataValueStr: ") + GetDataValueStr());
    // DEBUG_V(String("       value: ") + value);

    float NewData = value.toFloat ();

    // DEBUG_V(String("     NewData: ") + String(NewData, 1));

    do  // once
    {
        uint32_t PointPosition = value.indexOf ('.');
        // DEBUG_V();
        if (PointPosition < 0)
        {
            // DEBUG_V("No Decimal Point");
            ResponseMessage = GetTitle () + F (": Set failed: Decimal point is missing.");
            Response        = false;
            NewData         = FM_FREQ_MIN;
            SetDataValueStr (String (NewData, 1));
            break;
        }

        if (NewData > FM_FREQ_MAX)
        {
            // DEBUG_V("Too High");
            ResponseMessage = GetTitle () + F (": Set failed: Frequency is too high.");
            Response        = false;
            NewData         = FM_FREQ_MAX;
            SetDataValueStr (String (NewData, 1));
            break;
        }

        if (NewData < FM_FREQ_MIN)
        {
            // DEBUG_V("Too Low");
            ResponseMessage = GetTitle () + F (": Set failed: Frequency is too low");
            Response        = false;
            NewData         = FM_FREQ_MIN;
            SetDataValueStr (String (NewData, 1));
            break;
        }

        // DEBUG_V(String("        NewData: ") + String(NewData, 1));
        SetDataValueStr (String (NewData, 1));
        ResponseMessage = GetDataValueStr () + UNITS_MHZ_STR;
    } while (false);

    // DEBUG_V(String("   DataValueStr: ") + GetDataValueStr());
    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V(String("       Response: ") + String(Response));

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cFrequencyAdjust::UpdateStatus (bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    String Status = GetDataValueStr () + UNITS_MHZ_STR;
    HomeFreqStatus.set (Status, SkipLogOutput, ForceUpdate);
    AdjustFreqStatus.set (Status, SkipLogOutput, ForceUpdate);
    RadioFreqStatus.set (Status, SkipLogOutput, ForceUpdate);

    // DEBUG_END;
}

// *********************************************************************************************
cFrequencyAdjust FrequencyAdjust;

// *********************************************************************************************
// OEF
