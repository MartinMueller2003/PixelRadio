/*
   File: AudioInputImpedance.cpp
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
#include <map>

#include "AudioInputImpedance.hpp"
#include "AudioGain.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

#define INP_IMP05K_STR   "5K Ohms"
#define INP_IMP10K_STR   "10K Ohms"
#define INP_IMP20K_STR   "20K Ohms"
#define INP_IMP40K_STR   "40K Ohms"

static ChoiceListVector_t MapOfImpedances
{
    {INP_IMP05K_STR,  "5"},
    {INP_IMP10K_STR, "10"},
    {INP_IMP20K_STR, "20"},
    {INP_IMP40K_STR, "40"},
};

static const PROGMEM String     INPUT_IMPED_STR         = "INPUT_IMPED_STR";
static const PROGMEM char       INP_IMP_DEF_STR   []    = INP_IMP20K_STR;
static const PROGMEM String     RADIO_INP_IMP_STR       = "INPUT IMPEDANCE";

// *********************************************************************************************
cAudioInputImpedance::cAudioInputImpedance () :   cChoiceListControl (
        INPUT_IMPED_STR,
        RADIO_INP_IMP_STR,
        INP_IMP_DEF_STR,
        &MapOfImpedances)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
bool cAudioInputImpedance::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cChoiceListControl::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        QN8027RadioApi.setAudioImpedance (get32 ());
        AudioGain.set ();
    }
    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cAudioInputImpedance AudioInputImpedance;

// *********************************************************************************************
// OEF
