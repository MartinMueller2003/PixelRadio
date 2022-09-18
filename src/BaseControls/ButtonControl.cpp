/*
   File: ButtonControl.cpp
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

#include "ButtonControl.hpp"
#include "memdebug.h"

// *********************************************************************************************

// *********************************************************************************************
cButtonControl::cButtonControl (const String & Title) :
    cControlCommonMsg (emptyString, ControlType::Button, Title, emptyString, 10)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cButtonControl::~cButtonControl () {}

// *********************************************************************************************
void cButtonControl::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    // DEBUG_V (String ("TabId: ") + String (TabId))
    // DEBUG_V (String ("color: ") + String (color))

    cControlCommon::AddControls (TabId, color);
    setControlPanelStyle (ePanelStyle::PanelStyle135);
    setControlStyle (eCssStyle::CssStyleWhite);
/*
    ESPUI.      setElementStyle (       ControlId,
                                        String (F ("background-color: grey; color: white; margin-top: .1rem; margin-bottom: .1rem;")));
 */

    // DEBUG_END;
}

// *********************************************************************************************
// OEF
