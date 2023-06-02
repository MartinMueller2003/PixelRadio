/*
  *    File: RfCarrier.cpp
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
#include "language.h"
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "RfCarrierStatus.hpp"
#include "RfPaVoltage.hpp"

static const PROGMEM char   RADIO_RF_CARRIER_STR    []  = "RF CARRIER ENABLE";
static const PROGMEM char   RADIO_RF_CARR_FLAG      []  = "RADIO_RF_CARR_FLAG";
static const PROGMEM char   HOME_RAD_STAT_STR       []  = "RADIO STATUS";
/*
  *    static const PROGMEM char RADIO_OFF_AIR_STR     [] = "<font color=\"" COLOR_RED_STR "\">OFF AIR</font>";
  *    static const PROGMEM char RADIO_ON_AIR_STR      [] = "<font color=\"" COLOR_GRN_STR "\">ON AIR</font>";
  *    static const PROGMEM char RADIO_VOLT_STR        [] = "<font color=\"" COLOR_RED_STR "\">BAD-VDC</font>";
  *    static const PROGMEM char RADIO_VSWR_STR        [] = "<font color=\"" COLOR_RED_STR "\">HI-VSWR</font>";
  *    static const PROGMEM char RADIO_FAIL_STR        [] = "<font color=\"" COLOR_RED_STR "\">-FAIL-</font>";
  */
static const PROGMEM char   RADIO_OFF_AIR_STR       []  = "OFF AIR";
static const PROGMEM char   RADIO_ON_AIR_STR        []  = "ON AIR";
static const PROGMEM char   RADIO_VOLT_STR          []  = "BAD-VDC";
static const PROGMEM char   RADIO_VSWR_STR          []  = "HI-VSWR";
static const PROGMEM char   RADIO_FAIL_STR          []  = "-FAIL-";

// *********************************************************************************************
cRfCarrier::cRfCarrier () :   cBinaryControl (RADIO_RF_CARR_FLAG, RADIO_RF_CARRIER_STR, false)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cRfCarrier::AddHomeControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    pinMode (ON_AIR_PIN, OUTPUT);
    digitalWrite (ON_AIR_PIN, SIGN_OFF);

    RfCarrierStatus.AddControls (TabId, color);
    ESPUI.setPanelStyle (HomeStatusMessageId, F ("font-size: 3.0em;"));

    String  Dummy;
    String  Temp = String (getBool ());
    set (Temp, Dummy, true, true);

    // DEBUG_END;
}

// *********************************************************************************************
bool cRfCarrier::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (       String ("       value: ") + value);
    // DEBUG_V (       String ("getBool()Str: ") + getBool()Str);
    // DEBUG_V (       String ("   getBool(): ") + String (getBool()));

    bool Response = cBinaryControl::set (value, ResponseMessage, SkipLogOutput, ForceUpdate);
    // DEBUG_V (       String ("getBool()Str: ") + getBool()Str);
    // DEBUG_V (       String ("   getBool(): ") + String (getBool()));

    do  // once
    {
        if (!Response)
        {
            // DEBUG_V ("Lower levels had an error. Dont change anything");
            break;
        }

        // DEBUG_V ("Set the carrier");
        QN8027RadioApi.setRfCarrier (getBool ());

        if (!getBool ())
        {
            // DEBUG_V (RADIO_OFF_AIR_STR);
            ResponseMessage = RADIO_OFF_AIR_STR;
            digitalWrite (ON_AIR_PIN, SIGN_OFF);
            setMessage (RADIO_OFF_AIR_STR, eCssStyle::CssStyleBlack);
            RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleWhite, SkipLogOutput, ForceUpdate);
            if(!SkipLogOutput)
            {
                Log.infoln ((GetTitle () + F (": ") + ResponseMessage).c_str ());
            }
            break;
        }

        // DEBUG_V ("Get radio status");
        cQN8027RadioApi::QN8027RadioFmTestStatus_e TestStatus = QN8027RadioApi.GetTestStatus ();

        if (TestStatus == cQN8027RadioApi::QN8027RadioFmTestStatus_e::FM_TEST_FAIL)
        {
            // DEBUG_V (RADIO_FAIL_STR);
            ResponseMessage = RADIO_FAIL_STR;
            digitalWrite (ON_AIR_PIN, SIGN_OFF);
            setMessage (ResponseMessage, eCssStyle::CssStyleRed);
            RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleRed, SkipLogOutput, ForceUpdate);
            if(!SkipLogOutput)
            {
                Log.errorln ((GetTitle () + F (": ") + ResponseMessage).c_str ());
            }
            break;
        }

        if (TestStatus == cQN8027RadioApi::QN8027RadioFmTestStatus_e::FM_TEST_VSWR)
        {
            // DEBUG_V (RADIO_VSWR_STR);
            ResponseMessage = RADIO_VSWR_STR;
            digitalWrite (ON_AIR_PIN, SIGN_OFF);
            setMessage (ResponseMessage, eCssStyle::CssStyleRed);
            RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleRed, SkipLogOutput, ForceUpdate);
            Log.warningln ((GetTitle () + F (": ") + ResponseMessage).c_str ());
            break;
        }

        float paVolts = RfPaVoltage.GetVoltage ();
        if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX))
        {
            // DEBUG_V (RADIO_VOLT_STR);
            ResponseMessage = RADIO_VOLT_STR;
            digitalWrite (ON_AIR_PIN, SIGN_OFF);
            setMessage (ResponseMessage, eCssStyle::CssStyleRed);
            RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleRed, SkipLogOutput, ForceUpdate);
            Log.warningln ((GetTitle () + F (": ") + ResponseMessage).c_str ());
            break;
        }

        // DEBUG_V (RADIO_ON_AIR_STR);
        ResponseMessage = RADIO_ON_AIR_STR;
        digitalWrite (ON_AIR_PIN, SIGN_ON);
        setMessage (ResponseMessage, eCssStyle::CssStyleWhite);
        RfCarrierStatus.set (ResponseMessage, eCssStyle::CssStyleGreen, SkipLogOutput, ForceUpdate);
        Log.infoln ((GetTitle () + F (": ") + ResponseMessage).c_str ());
    } while (false);

    // DEBUG_V (String ("Response: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cRfCarrier RfCarrier;

// *********************************************************************************************
// OEF
