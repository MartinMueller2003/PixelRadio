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

static const PROGMEM String     RADIO_RF_CARRIER_STR    = "RF CARRIER ENABLE";
static const PROGMEM String     RADIO_RF_CARR_FLAG      = "RADIO_RF_CARR_FLAG";
static const PROGMEM String     HOME_RAD_STAT_STR       = "RADIO STATUS";
/*
   static const PROGMEM String RADIO_OFF_AIR_STR     = "<font color=\"" COLOR_RED_STR "\">OFF AIR</font>";
   static const PROGMEM String RADIO_ON_AIR_STR      = "<font color=\"" COLOR_GRN_STR "\">ON AIR</font>";
   static const PROGMEM String RADIO_VOLT_STR        = "<font color=\"" COLOR_RED_STR "\">BAD-VDC</font>";
   static const PROGMEM String RADIO_VSWR_STR        = "<font color=\"" COLOR_RED_STR "\">HI-VSWR</font>";
   static const PROGMEM String RADIO_FAIL_STR        = "<font color=\"" COLOR_RED_STR "\">-FAIL-</font>";
 */
static const PROGMEM String     RADIO_OFF_AIR_STR       = "OFF AIR";
static const PROGMEM String     RADIO_ON_AIR_STR        = "ON AIR";
static const PROGMEM String     RADIO_VOLT_STR          = "BAD-VDC";
static const PROGMEM String     RADIO_VSWR_STR          = "HI-VSWR";
static const PROGMEM String     RADIO_FAIL_STR          = "-FAIL-";

// *********************************************************************************************
cRfCarrier::cRfCarrier () : cControlCommon (RADIO_RF_CARR_FLAG)
{
    // _ DEBUG_START;

    DataValueStr        = "0";
    DataValue           = false;

    ActiveLabelStyle    = CSS_LABEL_STYLE_BLACK;
    InactiveLabelStyle  = CSS_LABEL_STYLE_RED;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, ControlType::Switcher, color);
    ESPUI.updateControlLabel (ControlId, RADIO_RF_CARRIER_STR.c_str ());

    // DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::AddHomeControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    HomeId = value;

    HomeStatusMessageId = ESPUI.addControl (
            ControlType::Label,
            HOME_RAD_STAT_STR.c_str (),
            emptyString,
            color,
            HomeId);
    ESPUI.setPanelStyle (HomeStatusMessageId, F ("font-size: 3.0em;"));

    String  Dummy;
    UpdateStatus (Dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cRfCarrier::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool  Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();
    uint32_t  NewDataValue;

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
            ResponseMessage     = String (F ("RF Carrier: BAD_VALUE: ")) + value;
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

        UpdateStatus (ResponseMessage);

        Log.infoln (String (F ("RF Carrier Set to: %s.")).c_str (), String (DataValue ? F ("On") : F ("Off")).c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
void cRfCarrier::UpdateStatus (String &ResponseMessage)
{
    // DEBUG_START;

    QN8027RadioApi.setRfCarrier (DataValue);

    extern uint32_t  paVolts;

    if (DataValue)
    {
        cQN8027RadioApi::QN8027RadioFmTestStatus_e  TestStatus = QN8027RadioApi.GetTestStatus ();

        if (TestStatus == cQN8027RadioApi::QN8027RadioFmTestStatus_e::FM_TEST_FAIL)
        {
            // DEBUG_V(RADIO_FAIL_STR);
            ResponseMessage = RADIO_FAIL_STR;
            ESPUI.      print ( StatusMessageId,        RADIO_FAIL_STR);
            ESPUI.      print ( HomeStatusMessageId,    RADIO_FAIL_STR);

            ESPUI.      setElementStyle (       StatusMessageId,        CSS_LABEL_STYLE_RED);
            ESPUI.      setElementStyle (       HomeStatusMessageId,    CSS_LABEL_STYLE_RED);
            ActiveLabelStyle    = CSS_LABEL_STYLE_RED;
            InactiveLabelStyle  = CSS_LABEL_STYLE_RED;
        }
        else if (TestStatus == cQN8027RadioApi::QN8027RadioFmTestStatus_e::FM_TEST_VSWR)
        {
            // DEBUG_V(RADIO_VSWR_STR);
            ResponseMessage = RADIO_VSWR_STR;
            ESPUI.      print ( StatusMessageId,        RADIO_OFF_AIR_STR);
            ESPUI.      print ( HomeStatusMessageId,    RADIO_VSWR_STR);

            ESPUI.      setElementStyle (       StatusMessageId,        CSS_LABEL_STYLE_RED);
            ESPUI.      setElementStyle (       HomeStatusMessageId,    CSS_LABEL_STYLE_RED);
            ActiveLabelStyle    = CSS_LABEL_STYLE_RED;
            InactiveLabelStyle  = CSS_LABEL_STYLE_RED;
        }
        else if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX))
        {
            // DEBUG_V(RADIO_VOLT_STR);
            ResponseMessage = RADIO_VOLT_STR;
            ESPUI.      print ( StatusMessageId,        RADIO_OFF_AIR_STR);
            ESPUI.      print ( HomeStatusMessageId,    RADIO_VOLT_STR);

            ESPUI.      setElementStyle (       StatusMessageId,        CSS_LABEL_STYLE_RED);
            ESPUI.      setElementStyle (       HomeStatusMessageId,    CSS_LABEL_STYLE_RED);
            ActiveLabelStyle    = CSS_LABEL_STYLE_RED;
            InactiveLabelStyle  = CSS_LABEL_STYLE_RED;
        }
        else
        {
            // DEBUG_V(RADIO_ON_AIR_STR);
            ResponseMessage = RADIO_ON_AIR_STR;
            ESPUI.      print ( StatusMessageId,        RADIO_OFF_AIR_STR);
            ESPUI.      print ( HomeStatusMessageId,    RADIO_ON_AIR_STR);

            ESPUI.      setElementStyle (       StatusMessageId,        CSS_LABEL_STYLE_GREEN);
            ESPUI.      setElementStyle (       HomeStatusMessageId,    CSS_LABEL_STYLE_GREEN);
            ActiveLabelStyle    = CSS_LABEL_STYLE_GREEN;
            InactiveLabelStyle  = CSS_LABEL_STYLE_GREEN;
        }
    }
    else
    {
        // DEBUG_V(RADIO_OFF_AIR_STR);
        ResponseMessage = RADIO_OFF_AIR_STR;
        ESPUI.  print ( StatusMessageId,        RADIO_OFF_AIR_STR);
        ESPUI.  print ( HomeStatusMessageId,    RADIO_OFF_AIR_STR);

        ESPUI.  setElementStyle (       StatusMessageId,        CSS_LABEL_STYLE_RED);
        ESPUI.  setElementStyle (       HomeStatusMessageId,    CSS_LABEL_STYLE_RED);
        ActiveLabelStyle        = CSS_LABEL_STYLE_RED;
        InactiveLabelStyle      = CSS_LABEL_STYLE_RED;
    }
    ESPUI.updateControlValue (ControlId, DataValueStr);

    // DEBUG_END;
}

// *********************************************************************************************
cRfCarrier  RfCarrier;

// *********************************************************************************************
// OEF
