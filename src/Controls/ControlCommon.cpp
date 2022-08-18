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
#include "ControlCommon.hpp"
#include "PixelRadio.h"
#include "memdebug.h"

// *********************************************************************************************

const PROGMEM char * CssStyles [] =
{
    CSS_LABEL_STYLE_BLACK,
    CSS_LABEL_STYLE_GREEN,
    CSS_LABEL_STYLE_MAROON,
    CSS_LABEL_STYLE_RED,
    CSS_LABEL_STYLE_TRANSPARENT,
    CSS_LABEL_STYLE_WHITE,
};

const PROGMEM String PanelStyles [] =
{
    {F ("font-size: 1.15em;")},
    {F ("font-size: 1.25em;")},
    {F ("font-size: 1.35em;")},
    {F ("font-size: 3.0em;") },
};

// *********************************************************************************************
cControlCommon::cControlCommon (const String    &_ConfigName,
    ControlType                                 _uiControltype,
    const String                                &_Title) :
    ConfigName (_ConfigName),
    uiControltype (_uiControltype),
    Title (_Title)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cControlCommon::~cControlCommon () {}

// *********************************************************************************************
void cControlCommon::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    ControlId = ESPUI.addControl (uiControltype,
            Title.c_str (),
            Title,
            color,
            TabId,
            [] (Control * sender, int type, void * UserInfo)
            {
                if (UserInfo)
                {
                    static_cast <cControlCommon *> (UserInfo)->Callback (sender, type);
                }
            },
            this);

    ESPUI.setPanelStyle (ControlId, PanelStyles[int(ControlPanelStyle)]);
    ESPUI.setElementStyle (ControlId, CssStyles[int(ControlStyle)]);

    StatusMessageId = ESPUI.addControl (ControlType::Label,
            emptyString.c_str (),
            emptyString,
            ControlColor::None,
            ControlId);
    ESPUI.setPanelStyle (StatusMessageId, PanelStyles[int(MessagePanelStyle)]);
    ESPUI.setElementStyle (StatusMessageId, CssStyles[int(InactiveLabelStyle)]);

    // force a UI Update
    String Response;
    set (DataValueStr, Response, true);

    // DEBUG_END;
}

// ************************************************************************************************
void cControlCommon::Callback (Control * sender, int type)
{
    // DEBUG_START;

    // DEBUG_V (       String ("value: ") + String (sender->value));
    // DEBUG_V (       String (" type: ") + String (type));

    do  // once
    {
        if ((P_CENTER_UP >= type) && (B_UP <= type))
        {
            // DEBUG_V ("Ignore button up events");
            break;
        }
        String Response;
        eCssStyle SelectedStyle = set (sender->value, Response) ? ActiveLabelStyle : InactiveLabelStyle;
        ESPUI.setElementStyle (StatusMessageId, CssStyles[int(SelectedStyle)]);
        ESPUI.print (StatusMessageId, Response);
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
String& cControlCommon::get () {return DataValueStr;}

// *********************************************************************************************
void    cControlCommon::restoreConfiguration (JsonObject &config)
{
    // DEBUG_START;

    if (!ConfigName.isEmpty ())
    {
        String NewValue = DataValueStr;
        ReadFromJSON (NewValue, config, ConfigName);
        String Response;
        set (NewValue, Response);
    }
    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::saveConfiguration (JsonObject &config)
{
    // DEBUG_START;

    if (!ConfigName.isEmpty ())
    {
        config[ConfigName] = DataValueStr;
    }
    // DEBUG_END;
}

// *********************************************************************************************
bool cControlCommon::set (const String &value, String &ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (       String ("      value: ") + value);
    // DEBUG_V (       String ("ForceUpdate: ") + String (ForceUpdate));

    bool Response = true;
    ResponseMessage.reserve (128);

    do  // once
    {
        if (!validate (value, ResponseMessage))
        {
            if (ResponseMessage.isEmpty ())
            {
                ResponseMessage = Title + F (": New value '") + value + F ("' is not valid.");
            }
            Log.errorln (ResponseMessage.c_str ());
            Response = false;
            break;
        }
        // DEBUG_V ("value is valid");

        ResponseMessage = Title + F (": Set To '") + value + F ("'");

        if (!SkipSetLog)
        {
            Log.infoln (ResponseMessage.c_str ());
        }

        if (value.equals (DataValueStr) && !ForceUpdate)
        {
            // DEBUG_V ("Dont not set duplicate value");
            break;
        }
        // DEBUG_V ("Saving value");

        DataValueStr = value;
        ESPUI.print (ControlId, DataValueStr);
    } while (false);

    // DEBUG_V (       String ("ResponseMsg: '") + ResponseMessage + "'");
    // DEBUG_V (       String ("   Response: ")  + String (Response));

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
bool cControlCommon::validate (const String &, String &, bool)
{
    // DEBUG_START;

    return true;

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
