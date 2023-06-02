/*
  *    File: PreEmphasis.cpp
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
#include <vector>

#include "PreEmphasis.hpp"
#include "PtyCode.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "memdebug.h"

#define PRE_EMPH_USA_STR    "North America (75uS)"  // North America / Japan.
#define PRE_EMPH_EUR_STR    "Europe (50uS)"         // Europe, Australia, China.
#define PRE_EMPH_USA_VAL    OFF
#define PRE_EMPH_EUR_VAL    ON

static ChoiceListVector_t ListOfRegions
{
    {PRE_EMPH_USA_STR, "0"},
    {PRE_EMPH_EUR_STR, "1"},
};

static const PROGMEM char   PRE_EMPH_DEF_STR            []  = PRE_EMPH_USA_STR;
static const PROGMEM char   RADIO_PRE_EMPH_STR      []      = "RADIO_PRE_EMPH_STR";
static const PROGMEM char   PRE_EMPH_STR            []      = "FM PRE-EMPHASIS";
static const PROGMEM uint8_t PRE_EMPH_DEF_VAL               = uint8_t (PRE_EMPH_USA_VAL);

// *********************************************************************************************
cPreEmphasis::cPreEmphasis () :   cChoiceListControl (RADIO_PRE_EMPH_STR,
        PRE_EMPH_STR,
        PRE_EMPH_DEF_STR,
        & ListOfRegions)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cPreEmphasis::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (       String ("          value: '") + value + "'");
    // DEBUG_V (       String ("   DataValueStr: '") + DataValueStr + "'");
    // DEBUG_V (       String ("          get32: ") + String (get32 ()));

    bool Response = cChoiceListControl::set (value, ResponseMessage, SkipLogOutput, ForceUpdate);

    if (Response || ForceUpdate)
    {
        QN8027RadioApi.setPreEmphasis (get32 (), RfCarrier.get ());
        PtyCode.setPtyCodeOptionValues ();
    }

    // DEBUG_V (       String ("   DataValueStr: ") + DataValueStr);
    // DEBUG_V (       String ("          get32: ") + String (get32 ()));
    // DEBUG_V (       String ("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V (       String ("       Response: ") + String (Response));

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
cPreEmphasis PreEmphasis;

// *********************************************************************************************
// OEF
