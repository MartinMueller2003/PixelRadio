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

#define INP_IMP05K_STR   " 5K Ohms"
#define INP_IMP10K_STR   "10K Ohms"
#define INP_IMP20K_STR   "20K Ohms (default)"
#define INP_IMP40K_STR   "40K Ohms"

static std::map <String, uint8_t>  MapOfImpedances
{
    {
        INP_IMP05K_STR,  5
    },
    {
        INP_IMP10K_STR, 10
    },
    {
        INP_IMP20K_STR, 20
    },
    {
        INP_IMP40K_STR, 40
    },
};

static const PROGMEM String     INPUT_IMPED_STR         = "INPUT_IMPED_STR";
static const PROGMEM char       INP_IMP_DEF_STR   []    = INP_IMP20K_STR;
static const PROGMEM String     RADIO_INP_IMP_STR       = "INPUT IMPEDANCE";

// *********************************************************************************************
cAudioInputImpedance::cAudioInputImpedance () : cControlCommon (INPUT_IMPED_STR)
{
    // _ DEBUG_START;

    DataValueStr        = INP_IMP_DEF_STR;
    DataValue           = MapOfImpedances[DataValueStr];

    // _ DEBUG_END;
}

// *********************************************************************************************
void cAudioInputImpedance::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, ControlType::Select, color);
    ESPUI.updateControlLabel (ControlId, RADIO_INP_IMP_STR.c_str ());

    for (auto &CurrentOption : MapOfImpedances)
    {
        ESPUI.addControl (
            ControlType::Option,
            CurrentOption.first.c_str (),
            CurrentOption.first,
            ControlColor::None,
            ControlId);
    }

    ESPUI.updateControlValue (ControlId, DataValueStr);

    // DEBUG_END;
}

// *********************************************************************************************
bool cAudioInputImpedance::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    bool  Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (MapOfImpedances.end () == MapOfImpedances.find (value))
        {
            ResponseMessage     = F ("AudioInputImpedance::Set: BAD_VALUE: ");
            ResponseMessage     += DataValueStr;
            Log.errorln (ResponseMessage.c_str ());
            Response = false;
            break;
        }
        DataValueStr    = value;
        DataValue       = MapOfImpedances[DataValueStr];

        QN8027RadioApi.setAudioImpedance (DataValue);

        ESPUI.updateControlValue (ControlId, DataValueStr);

        AudioGain.set ();

        Log.infoln (String (F ("Input Impedance Set to: %s.")).c_str (), DataValueStr.c_str ());
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cAudioInputImpedance  AudioInputImpedance;

// *********************************************************************************************
// OEF
