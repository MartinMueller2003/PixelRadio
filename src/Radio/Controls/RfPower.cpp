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
#include "memdebug.h"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "RfPower.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <map>

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

static std::map <String, uint8_t> MapOfPowerLevels
{
    {
        RF_PWR_LOW_STR,  27
    },
    {
        RF_PWR_MED_STR,  40
    },
    {
        RF_PWR_HIGH_STR, 78
    },
};

static const PROGMEM String     RADIO_RF_POWER_STR      = "RF POWER";
static const PROGMEM String     RADIO_POWER_STR         = "RADIO_POWER_STR";

// *********************************************************************************************
cRfPower::cRfPower () :   cOldControlCommon (RADIO_POWER_STR)
{
    // _ DEBUG_START;

    DataValueStr        = RF_PWR_HIGH_STR;
    DataValue           = MapOfPowerLevels[DataValueStr];

    // String rfPowerStr = RF_PWR_DEF_STR;                    // Control.

    // _ DEBUG_END;
}

// *********************************************************************************************
void cRfPower::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Select, color);
    ESPUI.updateControlLabel (ControlId, RADIO_RF_POWER_STR.c_str ());

    for (auto & CurrentOption : MapOfPowerLevels)
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
bool cRfPower::set (String & value, String & ResponseMessage)
{
    // DEBUG_START;

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();
    ResponseMessage = F ("RF Power: ");

    std::map <String, uint8_t>::iterator CurrentMapEntry = MapOfPowerLevels.end ();

    do  // once
    {
        if (value.isEmpty ())
        {
            DEBUG_V ("Nothing to test");
            ResponseMessage     += F ("empty parameter ignored");
            Response            = false;
            Log.errorln (ResponseMessage.c_str ());
            break;
        }
        CurrentMapEntry = MapOfPowerLevels.find (value);

        if (MapOfPowerLevels.end () == CurrentMapEntry)
        {
            ResponseMessage     += String (F ("invalid string parameter ignored: ")) + value;
            Response            = false;
            Log.errorln (ResponseMessage.c_str ());
            break;
        }

        if (DataValue == CurrentMapEntry->second)
        {
            DEBUG_V ("ignore duplicate setting");
        }
        else
        {
            DataValue           = CurrentMapEntry->second;
            DataValueStr        = CurrentMapEntry->first;

            QN8027RadioApi.setRfPower (DataValue, RfCarrier.get ());
        }
        ResponseMessage += String (F ("set to: ")) + DataValueStr + ": " + String (DataValue);
        Log.infoln (ResponseMessage.c_str ());
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cRfPower RfPower;

// *********************************************************************************************
// OEF
