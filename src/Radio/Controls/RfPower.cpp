/*
   File: RfPower.cpp
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
#include <vector>

#include "RfPower.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "memdebug.h"

// *********************************************************************************************
// *********************************************************************************************
// *********************************************************************************************
//              RF Power Control is not compatible with the RF Amp Circutry.
//              Low Power levels do not correctly excite the PA Transistor.
// *********************************************************************************************
// *********************************************************************************************
// *********************************************************************************************

#define RF_PWR_LOW_STR   "Low"
#define RF_PWR_MED_STR   "Med"
#define RF_PWR_HIGH_STR  "High (default)"
#define RF_PWR_DEF_STR   RF_PWR_HIGH_STR;

static ChoiceListVector_t MapOfPowerLevels
{
    {RF_PWR_LOW_STR,  "27"},
    {RF_PWR_MED_STR,  "40"},
    {RF_PWR_HIGH_STR, "78"},
};

static const PROGMEM char     RADIO_RF_POWER_STR      [] = "RF POWER";
static const PROGMEM char     RADIO_POWER_STR         [] = "RADIO_POWER_STR";

// *********************************************************************************************
cRfPower::cRfPower () :   cChoiceListControl (
        RADIO_POWER_STR,
        RADIO_RF_POWER_STR,
        String (F (RF_PWR_HIGH_STR)),
        &MapOfPowerLevels)
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
bool cRfPower::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cChoiceListControl::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        QN8027RadioApi.setRfPower (uint8_t (get32 ()), RfCarrier.get ());
    }
    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cRfPower RfPower;

// *********************************************************************************************
// OEF
