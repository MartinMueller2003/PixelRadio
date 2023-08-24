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
    // DEBUG_START;

    setSaveUpdates (false);

    // DEBUG_END;
}

// *********************************************************************************************
void cStatusControl::AddControls (uint16_t GroupId, ControlColor color)
{
    // DEBUG_START;

    // DEBUG_V(String("  Title: '") + GetTitle() + "'");
    // DEBUG_V(String("  Units: '") + Units + "'");
    // DEBUG_V(String("GroupId: ") + String(GroupId));
    // DEBUG_V(String("  color: ") + String(color));
    // DEBUG_V(String("unit_id: ") + String(unit_id));

    // DEBUG_V("Set Identifier name");
    name_id = ESPUI.addControl(ControlType::Label, emptyString.c_str(), GetTitle(), ControlColor::None, GroupId);
    // DEBUG_V(String("name_id: ") + String(name_id));
    ESPUI.setElementStyle(name_id, CSS_LABEL_STYLE_TRANSPARENT40R);

    // DEBUG_V("Set value");
    cControlCommon::AddControls(GroupId, color);
    setControlStyle(cControlCommon::eCssStyle::CssStyleTransparent40C);

    // DEBUG_V("Check units");
    if((!Units.equals(emptyString)) && (unit_id == Control::noParent))
    {
        // DEBUG_V("Create units");
        unit_id = ESPUI.addControl(Label, emptyString.c_str(), Units, ControlColor::None, GroupId);
        // DEBUG_V(String("unit_id: ") + String(unit_id));
        ESPUI.setElementStyle(unit_id, CSS_LABEL_STYLE_TRANSPARENT20);
    }

    // DEBUG_END;
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
