/*
   File: PreEmphasis.cpp
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
#include "memdebug.h"
#include "PreEmphasis.hpp"
#include "PtyCode.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <map>

#define PRE_EMPH_USA_STR    "North America (75uS)"      // North America / Japan.
#define PRE_EMPH_EUR_STR    "Europe (50uS)"             // Europe, Australia, China.
#define PRE_EMPH_USA_VAL    OFF
#define PRE_EMPH_EUR_VAL    ON

static std::map <String, uint8_t> MapOfRegions
{
    {
        PRE_EMPH_USA_STR, PRE_EMPH_USA_VAL
    },
    {
        PRE_EMPH_EUR_STR, PRE_EMPH_EUR_VAL
    },
};

static const PROGMEM String PRE_EMPH_DEF_STR            = PRE_EMPH_USA_STR;
static const PROGMEM uint8_t    PRE_EMPH_DEF_VAL        = uint8_t (PRE_EMPH_USA_VAL);
static const PROGMEM String     RADIO_PRE_EMPH_STR      = "RADIO_PRE_EMPH_STR";
static const PROGMEM String     PRE_EMPH_STR            = "FM PRE-EMPHASIS";

// *********************************************************************************************
cPreEmphasis::cPreEmphasis () : cControlCommon (RADIO_PRE_EMPH_STR)
{
    // DEBUG_START;

    DataValueStr        = PRE_EMPH_DEF_STR;     // Control.
    DataValue           = PRE_EMPH_DEF_VAL;

    // DEBUG_END;
}

// *********************************************************************************************
void cPreEmphasis::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls (value, ControlType::Select, color);
    ESPUI.updateControlLabel (ControlId, PRE_EMPH_STR.c_str ());

    for (auto &CurrentOption : MapOfRegions)
    {
        ESPUI.addControl (ControlType::Option,
            CurrentOption.first.c_str (),
            CurrentOption.first,
            ControlColor::None,
            ControlId);
    }

    ESPUI.updateControlValue (ControlId, DataValueStr);

    // DEBUG_END;
}

// *********************************************************************************************
bool cPreEmphasis::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (DataValueStr.equals (value))
        {
            // DEBUG_V("Ignore a duplicate request");
            break;
        }

        if (MapOfRegions.end () == MapOfRegions.find (value))
        {
            ResponseMessage     = String (F ("radioEmphasis Set: BAD VALUE: ")) + value;
            Response            = false;
            Log.errorln (ResponseMessage.c_str ());
            break;
        }
        // DEBUG_V("Update the radio")
        DataValueStr    = value;
        DataValue       = MapOfRegions[DataValueStr];
        QN8027RadioApi.setPreEmphasis (DataValue, RfCarrier.get ());

        ESPUI.updateControlValue (ControlId, DataValueStr);

        PtyCode.setPtyCodeOptionValues ();

        displaySaveWarning ();
        Log.infoln (String (F ("Pre-Emphasis Set to: %s.")).c_str (), DataValueStr.c_str ());
    } while (false);

    // DEBUG_V(String("   DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("      DataValue: ") + String(DataValue));
    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V(String("       Response: ") + String(Response));

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cPreEmphasis PreEmphasis;

// *********************************************************************************************
// OEF
