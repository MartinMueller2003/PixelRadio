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
#include "language.h"
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "RfCarrierStatus.hpp"

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
cRfCarrier::cRfCarrier () : cBinaryControl (RADIO_RF_CARR_FLAG, RADIO_RF_CARRIER_STR)
{
    // _ DEBUG_START;

    DataValueStr        = F ("0");
    SkipSetLog          = true;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::AddHomeControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    RfCarrierStatus.AddControls (TabId, color);
    ESPUI.setPanelStyle (HomeStatusMessageId, F ("font-size: 3.0em;"));

    String Dummy;
    set (DataValueStr, Dummy, true);

    // DEBUG_END;
}

// *********************************************************************************************
bool cRfCarrier::set (const String &value, String &ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cBinaryControl::set (value, ResponseMessage, ForceUpdate);
    // DEBUG_V (String (" DataValue: ") + String (DataValue));

    extern uint32_t paVolts;

    do  // once
    {
        if (!Response)
        {
            // DEBUG_V ("Lower levels had an error. Done change anything");
            break;
        }
        // DEBUG_V ("Set the carrier");
        QN8027RadioApi.setRfCarrier (DataValue);

        if (!DataValue)
        {
            // DEBUG_V (RADIO_OFF_AIR_STR);
            ResponseMessage = RADIO_OFF_AIR_STR;
            setMessage (RADIO_OFF_AIR_STR, eCssStyle::CssStyleBlack);
            RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleWhite);
            Log.infoln ((Title + F (": ") + ResponseMessage).c_str ());
            break;
        }
        // DEBUG_V ("Get radio status");
        cQN8027RadioApi::QN8027RadioFmTestStatus_e TestStatus = QN8027RadioApi.GetTestStatus ();

        if (TestStatus == cQN8027RadioApi::QN8027RadioFmTestStatus_e::FM_TEST_FAIL)
        {
            // DEBUG_V (RADIO_FAIL_STR);
            ResponseMessage = RADIO_FAIL_STR;
            setMessage (ResponseMessage, eCssStyle::CssStyleRed);
            RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleRed);
            Log.errorln ((Title + F (": ") + ResponseMessage).c_str ());
            break;
        }

        if (TestStatus == cQN8027RadioApi::QN8027RadioFmTestStatus_e::FM_TEST_VSWR)
        {
            // DEBUG_V (RADIO_VSWR_STR);
            ResponseMessage = RADIO_VSWR_STR;
            setMessage (ResponseMessage, eCssStyle::CssStyleRed);
            RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleRed);
            Log.warningln ((Title + F (": ") + ResponseMessage).c_str ());
            break;
        }

        if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX))
        {
            // DEBUG_V (RADIO_VOLT_STR);
            ResponseMessage = RADIO_VOLT_STR;
            setMessage (ResponseMessage, eCssStyle::CssStyleRed);
            RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleRed);
            Log.warningln ((Title + F (": ") + ResponseMessage).c_str ());
            break;
        }
        // DEBUG_V (RADIO_ON_AIR_STR);
        ResponseMessage = RADIO_ON_AIR_STR;
        setMessage (ResponseMessage, eCssStyle::CssStyleGreen);
        RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleGreen);
        Log.infoln ((Title + F (": ") + ResponseMessage).c_str ());
    } while (false);

    // DEBUG_V (String ("Response: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cRfCarrier RfCarrier;

// *********************************************************************************************
// OEF
