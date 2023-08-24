/*
  *    File: SequenceName.cpp
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
#include "SequenceName.hpp"
#include "memdebug.h"
#include "credentials_user.h"

static const PROGMEM char DefaultTextFieldValue [] = "Type New Sequence Name Here";

// *********************************************************************************************
cSequenceName::cSequenceName () :   
    InstructionLabel(emptyString, emptyString),
    StatusMessage(emptyString, emptyString),
    cControlCommon (emptyString,
        ControlType::Text,
        emptyString,
        DefaultTextFieldValue,
        64)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cSequenceName::~cSequenceName ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cSequenceName::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    InstructionLabel.AddControls (value, color);
    InstructionLabel.setControl (DefaultTextFieldValue, eCssStyle::CssStyleBlack);

    cControlCommon::AddControls (value, color);
    StatusMessage.AddControls (value, color);
    StatusMessage.setControlStyle (eCssStyle::CssStyleTransparent);

    // DEBUG_END;
}

// *********************************************************************************************
bool cSequenceName::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cControlCommon::validate (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        if (value.isEmpty ())
        {
            Response        = false;
            ResponseMessage = GetTitle () + F (": BAD_VALUE: '") + value + F ("'");
        }
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cSequenceName SequenceName;

// *********************************************************************************************
// OEF
