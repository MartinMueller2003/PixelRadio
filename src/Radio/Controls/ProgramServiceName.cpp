/*
   File: ProgramServiceName.cpp
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
#include "ProgramServiceName.hpp"
#include "RfCarrier.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static const PROGMEM char RDS_PSN_DEF_STR       [] = "PixeyFM";
static const PROGMEM char RDS_PROG_SERV_STR     [] = "RDS_PROG_SERV_STR";
static const PROGMEM char RDS_PROG_SERV_NM_STR  [] = "PROGRAM SERVICE NAME<br>Station Name (4-8 Chars)";

// *********************************************************************************************
cProgramServiceName::cProgramServiceName()
{
    //_ DEBUG_START;
    ProgramServiceName.reserve(10);
    ProgramServiceName = RDS_PSN_DEF_STR;
    //_ DEBUG_END;
}

// *********************************************************************************************
void cProgramServiceName::AddControls (uint16_t value)
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
                                ControlType::Text,
                                RDS_PROG_SERV_NM_STR,
                                emptyString,
                                ControlColor::Alizarin,
                                TabId,
                                [](Control* sender, int type, void* UserInfo)
                                {
                                    if(UserInfo)
                                    {
                                        static_cast<cProgramServiceName *>(UserInfo)->Callback(sender, type);
                                    }
                                },
                                this);

        ESPUI.setPanelStyle(ControlId,   "font-size: 1.5em;");
        ESPUI.setElementStyle(ControlId, "color: black;");
        ESPUI.addControl(ControlType::Min, emptyString.c_str(), "4", ControlColor::None, ControlId);
        ESPUI.addControl(ControlType::Max, emptyString.c_str(), "8", ControlColor::None, ControlId);
/*
        rdsProgNameID = ESPUI.addControl(
*/


        // force a UI Update
        String Temp = ProgramServiceName;
        ProgramServiceName.clear();
        set(Temp);

        // DEBUG_V();

    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void cProgramServiceName::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    set(sender->value);

    // DEBUG_END;
}

// *********************************************************************************************
void cProgramServiceName::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    String Temp;
    ReadFromJSON(Temp, config, RDS_PROG_SERV_STR);
    set(Temp);

    // DEBUG_END;
}

// *********************************************************************************************
void cProgramServiceName::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    config[RDS_PROG_SERV_STR] = ProgramServiceName;

    // DEBUG_END;
}

// *********************************************************************************************
void cProgramServiceName::set(String & value)
{
    // DEBUG_START;

    do // once
    {
        if(ProgramServiceName.equals(value))
        {
            // DEBUG_V("Skip duplicate name");
            break;
        }

        if(value.length() < 4)
        {
            Log.infoln(String(F("Program Service Name: '%s' is too short.")).c_str(), value.c_str());
            ESPUI.updateControlValue(ControlId, ProgramServiceName);
            break;
        }

        if(value.length() > 8)
        {
            Log.infoln(String(F("Program Service Name: '%s' is too long.")).c_str(), value.c_str());
            ESPUI.updateControlValue(ControlId, ProgramServiceName);
            break;
        }

        // DEBUG_V();
        ProgramServiceName = value;
        ESPUI.updateControlValue(ControlId, ProgramServiceName);
        QN8027RadioApi.setProgramServiceName(ProgramServiceName, RfCarrier.get());
        displaySaveWarning();
        Log.infoln(String(F("Program Service Name: '%s'.")).c_str(), ProgramServiceName.c_str());

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
cProgramServiceName ProgramServiceName;

// *********************************************************************************************
// OEF
