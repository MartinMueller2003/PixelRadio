/*
  *    File: ControlCommonMsg.cpp
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
#include "ControlCommonMsg.hpp"
#include "PixelRadio.h"
#include "memdebug.h"

// *********************************************************************************************
cControlCommonMsg::cControlCommonMsg (
                                      const String  & _ConfigName,
                                      ControlType   _uiControltype,
                                      const String  & _Title,
                                      const String  & _DefaultValue,
                                      uint32_t      _MaxDataLength) :
    cControlCommon (_ConfigName, _uiControltype, _Title, _DefaultValue, _MaxDataLength)
{}

// *********************************************************************************************
cControlCommonMsg::~cControlCommonMsg () {}

// *********************************************************************************************
void cControlCommonMsg::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;
    // DEBUG_V (String ("       Title: ") + GetTitle());

    cControlCommon::AddControls (TabId, color, false);

    MessageId = ESPUI.addControl (
        ControlType::Label,
        emptyString.c_str (),
        emptyString,
        ControlColor::None,
        ControlId);
    // DEBUG_V (   String ("   ControlId: ") + String (ControlId));
    // DEBUG_V (   String ("   MessageId: ") + String (MessageId));
    // DEBUG_V(String("GetDataValueStr: '") + GetTitle() + "':'" + GetDataValueStr() + "'");

    setMessagePanelStyle (MessagePanelStyle);
    setMessageStyle (MessageStyle);

    // force a UI Update
    String Response;
    set (GetDataValueStr (), Response, true, true);

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommonMsg::setMessage (const String & value)
{
    // DEBUG_START;
    // DEBUG_V (String ("       Title: ") + GetTitle());
    // DEBUG_V (String ("value: ") + value);

    setMessage (value, MessageStyle);

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommonMsg::setMessage (const String & value, eCssStyle style)
{
    // DEBUG_START;
    // DEBUG_V (String ("       Title: ") + GetTitle());
    // DEBUG_V (String ("   MessageId: ") + String (MessageId));

    if (value.isEmpty ())
    {
        // DEBUG_V("Empty Value");
        ESPUI.print (MessageId, emptyString);
        setMessageStyle (eCssStyle::CssStyleTransparent);
    }
    else
    {
        // DEBUG_V (String ("       value: ") + value);
        ESPUI.print (MessageId, value);
        // DEBUG_V (String ("       style: ") + String (style));
        setMessageStyle (style);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommonMsg::setMessageStyle (eCssStyle style)
{
    // DEBUG_START;
    // DEBUG_V (String ("style: ") + String (style));

    MessageStyle = style;
    ESPUI.setElementStyle (MessageId, GetCssStyle (style));

    // DEBUG_END;
}

// *********************************************************************************************
void cControlCommonMsg::setMessagePanelStyle (ePanelStyle style)
{
    // DEBUG_START;
    // DEBUG_V (String ("style: ") + String (style));

    MessagePanelStyle = style;
    ESPUI.setPanelStyle (MessageId, GetPanelStyle (style));

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
