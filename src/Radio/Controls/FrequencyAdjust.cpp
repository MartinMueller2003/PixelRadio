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

#include "FrequencyAdjust.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "memdebug.h"

static const PROGMEM String     ADJUST_FRQ_ADJ_STR      = "FREQUENCY ADJUST";
static const PROGMEM String     RADIO_FM_FREQ           = "RADIO_FM_FREQ";
static const PROGMEM String     UNITS_MHZ_STR           = " MHz";

const float  FM_FREQ_DEF            = 88.7f;    // 88.7MHz FM.
const float  FM_FREQ_MAX            = 107.9f;   // 107.9MHz FM.
const float  FM_FREQ_MIN            = 88.1f;    // 88.1MHz FM.
const float  FM_FREQ_SKP_KHZ_10X    = 1;        // 100Khz.
const float  FM_FREQ_SKP_MHZ_10X    = 10;       // 1MHz.

// *********************************************************************************************
cFrequencyAdjust::cFrequencyAdjust ()  :
    cControlCommon (RADIO_FM_FREQ, ControlType::Pad, ADJUST_FRQ_ADJ_STR, String(FM_FREQ_DEF), 5)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    UpdateStatus ();

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddRadioControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    RadioFreqStatus.AddControls(value, color);
    UpdateStatus ();

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddHomeControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    HomeFreqStatus.AddControls(value, color);
    UpdateStatus ();

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddAdjustControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    AdjustFreqStatus.AddControls(value, color);
    cControlCommon::AddControls(value, color);
    UpdateStatus ();

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

        uint32_t NewData = uint32_t(DataValueStr.toFloat() * 10.0f);
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

        String  DataStr = String (float(NewData) / 10.0f, 1);
        // DEBUG_V(String("DataStr: ") + DataStr);
        String  ResponseMessage;
        if( set (DataStr, ResponseMessage, false))
        {
            setMessage(emptyString, eCssStyle::CssStyleTransparent);
        }
        else
        {
            setMessage(ResponseMessage, eCssStyle::CssStyleRed);
        }

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
bool cFrequencyAdjust::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("       value: ") + value);

    bool Response = cControlCommon::set (value, ResponseMessage, ForceUpdate);

    if(Response)
    {
        float tempFloat = DataValueStr.toFloat();
        QN8027RadioApi.setFrequency (tempFloat, RfCarrier.get ());

        UpdateStatus ();
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

    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("       value: ") + value);

    float NewData = value.toFloat();

    // DEBUG_V(String("     NewData: ") + String(NewData, 1));

    do  // once
    {
        uint32_t PointPosition = DataValueStr.indexOf('.');
        if(PointPosition < 0)
        {
            ResponseMessage     = Title + F (": Set failed: Decimal point is missing.");
            Response            = false;
            break;
        }

        if (NewData > FM_FREQ_MAX)
        {
            ResponseMessage     = Title + F(": Set failed: Frequency is too high.");
            Response            = false;
            NewData             = FM_FREQ_MAX;
            break;
        }

        if (NewData < FM_FREQ_MIN)
        {
            ResponseMessage     = Title + F (": Set failed: Frequency is too low");
            Response            = false;
            NewData             = FM_FREQ_MIN;
            break;
        }
        // DEBUG_V(String("NewData: ") + String(NewData));
        DataValueStr = String(NewData,1);
        ResponseMessage = DataValueStr + UNITS_MHZ_STR;

    } while (false);

    // DEBUG_V(String("   DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V(String("       Response: ") + String(Response));

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cFrequencyAdjust::UpdateStatus ()
{
    // DEBUG_START;

    String Status = DataValueStr + UNITS_MHZ_STR;
    HomeFreqStatus.set(Status);
    AdjustFreqStatus.set(Status);
    RadioFreqStatus.set(Status);

    // DEBUG_END;
}

// *********************************************************************************************
cFrequencyAdjust FrequencyAdjust;

// *********************************************************************************************
// OEF
