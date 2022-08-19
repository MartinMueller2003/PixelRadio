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
#include "FrequencyAdjust.hpp"
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM String     RADIO_FM_FRQ_STR        = "FM FREQUENCY";
static const PROGMEM String     ADJUST_FRQ_ADJ_STR      = "FREQUENCY ADJUST";
static const PROGMEM String     RADIO_FM_FREQ           = "RADIO_FM_FREQ";
static const PROGMEM String     UNITS_MHZ_STR           = " MHz";
static const PROGMEM String     HOME_RAD_STAT_STR       = "RADIO STATUS";

// *********************************************************************************************
cFrequencyAdjust::cFrequencyAdjust ()  : cOldControlCommon (String (RADIO_FM_FREQ))
{
    // _ DEBUG_START;

    DataValueStr        = "88.1";
    DataValue           = 881;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddRadioControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    RadioId = value;

    RadioStatusID = ESPUI.addControl (ControlType::Label,
                                      RADIO_FM_FRQ_STR.c_str (),
                                      emptyString,
                                      color,
                                      RadioId);
    ESPUI.setPanelStyle (RadioStatusID, F ("font-size: 3.0em;"));
    ESPUI.setElementStyle (RadioStatusID, F ("width: 75%;"));

    UpdateStatus ();

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddHomeControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    HomeId = value;

    HomeStatusID = ESPUI.addControl (ControlType::Label,
                                     HOME_RAD_STAT_STR.c_str (),
                                     emptyString,
                                     color,
                                     HomeId);
    ESPUI.setPanelStyle (HomeStatusID, F ("font-size: 3.0em;"));

    UpdateStatus ();

    // DEBUG_END;
}

// *********************************************************************************************
void cFrequencyAdjust::AddAdjustControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    AdjustStatusID = ESPUI.addControl (ControlType::Label,
                                       RADIO_FM_FRQ_STR.c_str (),
                                       emptyString,
                                       color,
                                       value);
    ESPUI.setPanelStyle (AdjustStatusID, F ("font-size: 3.0em;"));
    ESPUI.setElementStyle (AdjustStatusID, F ("max-width: 75%;"));

    cOldControlCommon::AddControls (value,
                                    ControlType::Pad,
                                    color);
    ESPUI.updateControlLabel (ControlId, ADJUST_FRQ_ADJ_STR.c_str ());

    UpdateStatus ();

    // DEBUG_END;
}

// ************************************************************************************************
void cFrequencyAdjust::Callback (Control * sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("    value: ") + String(sender->value));
    // DEBUG_V(String("     type: ") + String(type));
    // DEBUG_V(String("DataValue: ") + String(DataValue));

    uint32_t NewData = DataValue;

    do  // once
    {
        if (0 <= type)
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
            Log.errorln (String (F ("CbAdjFmFreq: %s.")).c_str (), BAD_VALUE_STR);
            break;
        }

        if (NewData > FM_FREQ_MAX_X10)
        {
            // DEBUG_V("Frequency is too high");
            NewData = FM_FREQ_MAX_X10;
        }
        else if (NewData < FM_FREQ_MIN_X10)
        {
            // DEBUG_V("Frequency is too low");
            NewData = FM_FREQ_MIN_X10;
        }
        String  DataStr = String (float(NewData) / 10.0f);
        String  Response;
        set (DataStr, Response);
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
bool cFrequencyAdjust::set (String & value, String & ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));
    // DEBUG_V(String("       value: ") + value);

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    uint32_t NewData = uint32_t (atof (value.c_str ()) * 10.0f);

    // DEBUG_V(String("     NewData: ") + NewData);

    do  // once
    {
        if (NewData > FM_FREQ_MAX_X10)
        {
            ResponseMessage     = F ("RF Frequency Set failed: Frequency is too high");
            Response            = false;
            break;
        }

        if (NewData < FM_FREQ_MIN_X10)
        {
            ResponseMessage     = F ("RF Frequency Set failed: Frequency is too low");
            Response            = false;
            break;
        }

        if (NewData == DataValue)
        {
            // DEBUG_V("Ignore duplicate value");
            break;
        }
        DataValueStr    = String (float(NewData) / 10.0f, 1) + UNITS_MHZ_STR;
        DataValue       = NewData;

        UpdateStatus ();

        Log.infoln (String (F ("RF Frequency Set to: %s")).c_str (), DataValueStr);
        displaySaveWarning ();
    } while (false);

    // DEBUG_V(String("   DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("      DataValue: ") + String(DataValue));
    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V(String("       Response: ") + String(Response));

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cFrequencyAdjust::UpdateStatus ()
{
    // DEBUG_START;

    float tempFloat = float(DataValue) / 10.0f;

    QN8027RadioApi.setFrequency (tempFloat, RfCarrier.get ());

    // DEBUG_V(String("tempFloat: ") + String(tempFloat));

    ESPUI.      print ( AdjustStatusID, DataValueStr);
    ESPUI.      print ( HomeStatusID,   DataValueStr);
    ESPUI.      print ( RadioStatusID,  DataValueStr);

    // DEBUG_END;
}

// *********************************************************************************************
cFrequencyAdjust FrequencyAdjust;

// *********************************************************************************************
// OEF
