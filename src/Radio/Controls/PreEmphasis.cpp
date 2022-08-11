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
#include <Arduino.h>
#include <ArduinoLog.h>
#include <map>
#include "PreEmphasis.hpp"
#include "RfCarrier.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

#define PRE_EMPH_USA_STR    "North America (75uS)" // North America / Japan.
#define PRE_EMPH_EUR_STR    "Europe (50uS)"        // Europe, Australia, China.
#define PRE_EMPH_USA_VAL    OFF
#define PRE_EMPH_EUR_VAL    ON

static std::map<String, uint8_t> MapOfRegions
{
    {PRE_EMPH_USA_STR, PRE_EMPH_USA_VAL},
    {PRE_EMPH_EUR_STR, PRE_EMPH_EUR_VAL},
};

static const PROGMEM char PRE_EMPH_DEF_STR      [] = PRE_EMPH_USA_STR;
static const PROGMEM uint8_t PRE_EMPH_DEF_VAL      = uint8_t(PRE_EMPH_USA_VAL);
static const PROGMEM char RADIO_PRE_EMPH_STR    [] = "RADIO_PRE_EMPH_STR";
static const PROGMEM char PRE_EMPH_STR          [] = "FM PRE-EMPHASIS";

// *********************************************************************************************
cPreEmphasis::cPreEmphasis()
{
    // DEBUG_START;

    preEmphasisStr  = PRE_EMPH_DEF_STR; // Control.
    emphVal         = PRE_EMPH_DEF_VAL;

    // DEBUG_END;
}

// *********************************************************************************************
void cPreEmphasis::AddControls (uint16_t value)
{
    // DEBUG_START;

    do // once
    {
        if(Control::noParent != TabId)
        {
            // DEBUG_V("Controls have already been set up");
            break;
        }

        TabId = value;

        ControlId = ESPUI.addControl(
                            ControlType::Select, 
                            PRE_EMPH_STR, 
                            emptyString, 
                            ControlColor::Emerald, 
                            TabId, 
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cPreEmphasis *>(UserInfo)->Callback(sender, type);
                                }
                            },
                            this);

        for(auto & CurrentOption : MapOfRegions)
        {
            ESPUI.addControl(ControlType::Option, 
                            CurrentOption.first.c_str(), 
                            CurrentOption.first, 
                            ControlColor::None, 
                            ControlId);
        }

        String Temp = preEmphasisStr;
        preEmphasisStr.clear();
        set(Temp);

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void cPreEmphasis::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    set(sender->value);

    // DEBUG_END;
}

// *********************************************************************************************
void cPreEmphasis::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    String Temp;
    ReadFromJSON(Temp, config, RADIO_PRE_EMPH_STR);
    set(Temp);

    // DEBUG_END;
}

// *********************************************************************************************
void cPreEmphasis::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    config[RADIO_PRE_EMPH_STR] = preEmphasisStr; // Use radio.setPreEmphTime50(0/1), uint8 value. Not working?

    // DEBUG_END;
}

// *********************************************************************************************
void cPreEmphasis::set(String & value)
{
    // DEBUG_START;

    do // once
    {
        if(preEmphasisStr.equals(value))
        {
            // DEBUG_V("Ignore a duplicate request");
            break;
        }

        preEmphasisStr = value;
        if(MapOfRegions.end() == MapOfRegions.find(preEmphasisStr))
        {
            Log.errorln(String(F("radioEmphasis Set: BAD VALUE: '%s'. Using default.")).c_str(), value);
            break;
        }

        // DEBUG_V("Update the radio")
        emphVal = MapOfRegions[preEmphasisStr];
        QN8027RadioApi.setPreEmphasis(emphVal, RfCarrier.get());

        ESPUI.updateControlValue(ControlId, preEmphasisStr);

#ifdef OldWay
        setPtyCodeOptionValues();
#endif // def OldWay
        displaySaveWarning();

    } while (false);

    Log.infoln(String(F("Pre-Emphasis Set to: %s.")).c_str(), preEmphasisStr.c_str());

    // DEBUG_END;
}

// *********************************************************************************************
cPreEmphasis PreEmphasis;

// *********************************************************************************************
// OEF
