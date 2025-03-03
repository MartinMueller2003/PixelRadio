/*
  *    File: ControlCommon.cpp
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
#include <ESPUI.h>
#include "ControlCommon.hpp"
#include "PixelRadio.h"
#include "memdebug.h"

// *********************************************************************************************

const PROGMEM char CSS_LABEL_STYLE_BLACK_BW             [] =
    "background-color: white; color: black; margin-top: .1rem; margin-bottom: .1rem;";
const PROGMEM char CSS_LABEL_STYLE_GREEN_BW             [] =
    "background-color: white; color: green; margin-top: .1rem; margin-bottom: .1rem;";
const PROGMEM char CSS_LABEL_STYLE_MAROON_BW            [] =
    "background-color: white; color: Maroon; margin-top: .1rem; margin-bottom: .1rem;";
const PROGMEM char CSS_LABEL_STYLE_RED_BW               [] =
    "background-color: white; color: red; margin-top: .1rem; margin-bottom: .1rem;";
const PROGMEM char CSS_LABEL_STYLE_TRANSPARENT_BW       [] =
    "background-color: unset; color: unset; margin-top: .1rem; margin-bottom: .1rem;";
static const PROGMEM char CSS_LABEL_STYLE_WHITE_BB      [] =
    "background-color: black; color: white; margin-top: .1rem; margin-bottom: .1rem;";
static const PROGMEM char CSS_LABEL_STYLE_WHITE_grey    [] =
    "background-color: grey; color: white; margin-top: .1rem; margin-bottom: .1rem;";

static const PROGMEM char * CssStyles [] =
{
    CSS_LABEL_STYLE_BLACK,
    CSS_LABEL_STYLE_GREEN,
    CSS_LABEL_STYLE_MAROON,
    CSS_LABEL_STYLE_RED,
    CSS_LABEL_STYLE_TRANSPARENT,
    CSS_LABEL_STYLE_WHITE,
    CSS_LABEL_STYLE_BLACK_BW,
    CSS_LABEL_STYLE_GREEN_BW,
    CSS_LABEL_STYLE_MAROON_BW,
    CSS_LABEL_STYLE_RED_BW,
    CSS_LABEL_STYLE_TRANSPARENT_BW,
    CSS_LABEL_STYLE_WHITE_BB,
    CSS_LABEL_STYLE_WHITE_grey,
    CSS_LABEL_STYLE_TRANSPARENT20,
    CSS_LABEL_STYLE_TRANSPARENT40R,
    CSS_LABEL_STYLE_TRANSPARENT40C,
};

static const String PanelStyles [] =
{
    {"font-size: 1.15em;"              },
    {"font-size: 1.25em;"              },
    {"font-size: 1.35em;"              },
    {"font-size: 3.0em;"               },
    {"font-size: 1.15em; color: black;"},
    {"font-size: 1.25em; color: black;"},
    {"font-size: 1.35em; color: black;"},
    {"font-size: 3.0em; color: black;" },
    {"font-size: 1.15em; color: white;"},
    {"font-size: 1.25em; color: white;"},
    {"font-size: 1.35em; color: white;"},
    {"font-size: 3.0em; color: white;" },
};

// *********************************************************************************************
cControlCommon::cControlCommon (const String    & _ConfigName,
                                ControlType     _uiControltype,
                                const String    & _Title,
                                const String    & _DefaultValue,
                                uint32_t        _MaxDataLength) :
    ConfigName (_ConfigName),
    uiControltype (_uiControltype),
    Title (_Title),
    DefaultValue (_DefaultValue),
    MaxDataLength (_MaxDataLength)
{
    // _ DEBUG_START;

    Title = _Title;
    SetDataValueStr (DefaultValue);

    // DEBUG_V(String("GetDataValueStr: '") + GetTitle() + "':'" + GetDataValueStr() + "'");

    // _ DEBUG_END;
}

// *********************************************************************************************
cControlCommon::~cControlCommon () {}

// *********************************************************************************************
void cControlCommon::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    AddControls (TabId, color, false);

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::AddControls (uint16_t TabId, ControlColor color, bool skipSet)
{
    // DEBUG_START;
    // DEBUG_V (   String ("       Title: ") + Title);
    // DEBUG_V (   String ("DefaultValue: ") + DefaultValue);
    // DEBUG_V(String("GetDataValueStr: '") + GetTitle() + "':'" + GetDataValueStr() + "'");

    ControlId = ESPUI.addControl (
        uiControltype,
        Title.c_str (),
        GetDataValueStr (),
        color,
        TabId,
        [&] (Control * sender, int type)
        {
            Callback (sender, type);
        });

    if (MaxDataLength)
    {
        // DataValueStr.reserve (MaxDataLength + 2);
        ESPUI.addControl (ControlType::Max, emptyString.c_str (), String (MaxDataLength), color, ControlId);
    }

    setControlPanelStyle (ControlPanelStyle);
    // setControlStyle (ControlStyle);

    if (!skipSet)
    {
        // force a UI Update
        String Response;
        set (GetDataValueStr (), Response, true, true);
    }

    // DEBUG_END;
}

// ************************************************************************************************
void cControlCommon::Callback (Control * sender, int type)
{
    // DEBUG_START;

    // DEBUG_V (String ("value: ") + String (sender->value));
    // DEBUG_V (String (" type: ") + String (type));

    do  // once
    {
        if ((P_CENTER_UP >= type) && (B_UP <= type))
        {
            // DEBUG_V ("Ignore button up events");
            break;
        }

        String Dummy;
        set (sender->value, Dummy, false, false);
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
const String &cControlCommon::get () {return GetDataValueStr ();}

// *********************************************************************************************
bool cControlCommon::GetAndResetValueChangedFlag ()
{
    // DEBUG_START;

    bool Response = ValueChanged;
    ValueChanged = false;

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
String cControlCommon::GetCssStyle (eCssStyle Style) {return CssStyles[int(Style)];}

// *********************************************************************************************
const String &cControlCommon::GetDataValueStr ()
{
    // DEBUG_START;
    Control * MyControl = ESPUI.getControl (ControlId);
    /*
      *     if (MyControl)
      *     {
      *         // DEBUG_V (String ("value: ") + MyControl->value);
      *     }
      *     else
      *     {
      *         // DEBUG_V ("Control not valid");
      *     }
      */
    // DEBUG_END;

    return (MyControl) ? MyControl->value : LocalValue;
}  // GetDataValueString

// *********************************************************************************************
String cControlCommon::GetPanelStyle (ePanelStyle Style) {return PanelStyles[int(Style)];}

// *********************************************************************************************
void cControlCommon::ResetToDefaults ()
{
    // DEBUG_START;

    String dummy;
    set (DefaultValue, dummy, SystemBooting, false);

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::restoreConfiguration (JsonObject & config)
{
    // DEBUG_START;

    if (!ConfigName.isEmpty ())
    {
        String NewValue = GetDataValueStr ();
        ReadFromJSON (NewValue, config, ConfigName);
        String Response;
        set (NewValue, Response, SystemBooting, true);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::saveConfiguration (JsonObject & config)
{
    // DEBUG_START;

    if (!ConfigName.isEmpty ())
    {
        config[ConfigName] = GetDataValueStr ();
    }

    // DEBUG_END;
}

// *********************************************************************************************
bool cControlCommon::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (   String ("      value: ") + value);
    // DEBUG_V (   String ("ForceUpdate: ") + String (ForceUpdate));

    bool Response = true;
    ResponseMessage.reserve (128);

    do  // once
    {
        if (!validate (value, ResponseMessage, ForceUpdate))
        {
            if (ResponseMessage.isEmpty ())
            {
                ResponseMessage = String (Title + F (": Set: BAD VALUE: '")) + value + F ("'");
            }

            if (!SkipLogOutput)
            {
                Log.errorln (ResponseMessage.c_str ());
            }

            Response = false;
            break;
        }

        // DEBUG_V ("value is valid");

        ResponseMessage = Title + F (": Set To '") + value + F ("'");
        ESPUI.print (ControlId, value);

        if (!SkipLogOutput)
        {
            Log.infoln (ResponseMessage.c_str ());
        }

        if (!SystemBooting)
        {
            // DEBUG_V ("Saving value");
            ValueChanged = true;

            if (SaveUpdate)
            {
                displaySaveWarning ();
            }
        }
    } while (false);

    // DEBUG_V (String ("ResponseMsg: '") + ResponseMessage + "'");
    // DEBUG_V (String ("   Response: ")  + String (Response));

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
void cControlCommon::setControl (const String & value, eCssStyle style)
{
    // DEBUG_START;

    // DEBUG_V (String ("value: ") + value);
    // DEBUG_V (String ("style: ") + String (style));
    ESPUI.print (ControlId, value);
    setControlStyle (style);

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::setControlLabel (const String & value)
{
    // DEBUG_START;
    ESPUI.updateControlLabel (ControlId, value.c_str ());
    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::setControlStyle (eCssStyle style)
{
    // DEBUG_START;
    // DEBUG_V (String ("style: ") + String (style));

    ControlStyle = style;
    ESPUI.setElementStyle (ControlId, CssStyles[int(style)]);

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::setControlPanelStyle (ePanelStyle style)
{
    // DEBUG_START;
    // DEBUG_V (String ("style: ") + String (style));

    ControlPanelStyle = style;
    ESPUI.setPanelStyle (ControlId, PanelStyles[int(style)]);

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommon::SetDataValueStr (const String & value)
{
    // DEBUG_START;
    // DEBUG_V (String ("value: ") + value);

    Control * MyControl = ESPUI.getControl (ControlId);

    if (MyControl)
    {
        MyControl->value = value;
        LocalValue.clear ();
    }
    else
    {
        LocalValue = value;
        // DEBUG_V ("Control not valid");
    }

    // DEBUG_END;
}  // GetDataValueString

// *********************************************************************************************
bool cControlCommon::validate (const String & value, String &, bool)
{
    // DEBUG_START;

    bool Response = true;

    if (MaxDataLength)
    {
        if (value.length () > MaxDataLength)
        {
            Response = false;
        }
        else
        {
            SetDataValueStr (value);
        }
    }
    else
    {
        SetDataValueStr (value);
    }

    return Response;

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
