/*
  *    File: StatusControl.cpp
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
#include "StatusControl.hpp"
#include "memdebug.h"

// *********************************************************************************************
cStatusControl::cStatusControl (const String & _Title, const String & _Units) :
    Units(_Units),
    cControlCommon (emptyString, ControlType::Label, _Title, emptyString, 0)
{
    // _ DEBUG_START;
    setSaveUpdates (false);
    // _ DEBUG_END;
}

// *********************************************************************************************
cStatusControl::cStatusControl (const String & _Title) :
    Units(emptyString),
    cControlCommon (emptyString, ControlType::Label, emptyString, emptyString, 0)
{
    // _ DEBUG_START;

    setSaveUpdates (false);

    // _ DEBUG_END;
}

// *********************************************************************************************
cStatusControl::cStatusControl () :
    Units(emptyString),
    cControlCommon (emptyString, ControlType::Label, emptyString, emptyString, 0)
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cStatusControl::AddControls (uint16_t GroupId, ControlColor color)
{
    DEBUG_START;

    // Identifier name
    name_id = ESPUI.addControl(Label, emptyString.c_str(), Units, ControlColor::None, GroupId);
    ESPUI.setElementStyle(name_id, CSS_LABEL_STYLE_TRANSPARENT60);

    // value
    cControlCommon::AddControls(GroupId, color);
    setControlStyle(cControlCommon::eCssStyle::CssStyleTransparent30);

    // units
    if((!Units.equals(emptyString)) && (unit_id == Control::noParent))
    {
        unit_id = ESPUI.addControl(Label, emptyString.c_str(), Units, ControlColor::None, GroupId);
        ESPUI.setElementStyle(unit_id, CSS_LABEL_STYLE_TRANSPARENT10);
    }

    DEBUG_END;
} // AddControls

// *********************************************************************************************
void cStatusControl::set (const String & value, eCssStyle style, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    setControlStyle (style);
    set (value, SkipLogOutput, ForceUpdate);

    // DEBUG_END;
}

// *********************************************************************************************
bool cStatusControl::set (const String & value, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    String  Dummy;
    bool    Response = cControlCommon::set (value, Dummy, SkipLogOutput, ForceUpdate);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
