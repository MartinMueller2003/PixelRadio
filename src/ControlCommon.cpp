/*
   File: ControlCommon.cpp
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
#include <ESPUI.h>
#include "PixelRadio.h"
#include "ControlCommon.hpp"
#include "memdebug.h"

// *********************************************************************************************
cControlCommon::cControlCommon(String _ConfigName)
{
    //_ DEBUG_START;

    ConfigName = _ConfigName;

    DataValue = 0;
    DataValueStr = F("0");

    //_ DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::AddControls (uint16_t value, 
    ControlType uiControltype, ControlColor color)
{
    // DEBUG_START;

    TabId = value;

    ControlId = ESPUI.addControl(
                            uiControltype,
                            emptyString.c_str(),
                            emptyString,
                            color,
                            value,
                            [](Control* sender, int type, void* UserInfo)
                            {
                                if(UserInfo)
                                {
                                    static_cast<cControlCommon *>(UserInfo)->Callback(sender, type);
                                }
                            },
                            this);

    ESPUI.setPanelStyle(ControlId, "font-size: 1.25em;");
    ESPUI.setElementStyle(ControlId, "color: black;");

    StatusMessageId = ESPUI.addControl(
                            ControlType::Label,
                            emptyString.c_str(),
                            emptyString,
                            ControlColor::None,
                            ControlId);
    ESPUI.setPanelStyle(StatusMessageId, "font-size: 1.25em;");
    ESPUI.setElementStyle(StatusMessageId, CSS_LABEL_STYLE_TRANSPARENT);

    // force a UI Update
    String Response;
    DataValue = !DataValue;
    String TempDataValueStr = DataValueStr;
    DataValueStr.clear();
    set(TempDataValueStr, Response);

    // DEBUG_END;
}

// ************************************************************************************************
void cControlCommon::Callback(Control *sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + String(sender->value));
    // DEBUG_V(String(" type: ") + String(type));

    do // once
    {
        if( B_UP == type)
        {
            // DEBUG_V("Ignore button up events");
            break;
        }

        String Response;
        ESPUI.setElementStyle(StatusMessageId, set(sender->value, Response) ? ActiveLabelStyle : InactiveLabelStyle);
        ESPUI.print(StatusMessageId, Response);

    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    if(!ConfigName.isEmpty())
    {
        String NewValue = DataValueStr;
        ReadFromJSON(NewValue, config, ConfigName);
        String Response;
        set(NewValue, Response);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    if(!ConfigName.isEmpty())
    {
        config[ConfigName] = DataValueStr;
    }

    // DEBUG_END;
}

// *********************************************************************************************

// *********************************************************************************************
// OEF
