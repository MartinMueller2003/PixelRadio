/*
   File: ControllerCommon.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Apr-06-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
   absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

    Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
 */

// *********************************************************************************************
#include "ControllerCommon.h"
#include "Language.h"

 #include "memdebug.h"

// *********************************************************************************************
// class c_ControllerCommon

// *********************************************************************************************
cControllerCommon::cControllerCommon (const String & _Title, CtypeId _Id) :
    TypeId(_Id),
    cBinaryControl(N_ControllerEnabled, _Title, false)
{}

// *********************************************************************************************
cControllerCommon::~cControllerCommon ()
{}

// ************************************************************************************************
void cControllerCommon::AddControls(uint16_t tabId, ControlColor color)
{
    // DEBUG_START;

    SeperatorName = GetTitle() + F(" SETTINGS");
    ESPUI.addControl(ControlType::Separator,
                     SeperatorName.c_str(),
                     emptyString,
                     ControlColor::None,
                     tabId);

    setOffMessageStyle (eCssStyle::CssStyleWhite );
    setOnMessageStyle (eCssStyle::CssStyleWhite);

    cBinaryControl::AddControls(tabId, color);

    // DEBUG_END;
}

// *********************************************************************************************
void cControllerCommon::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cBinaryControl::saveConfiguration(config);
    config[N_name]  = GetTitle();
    config[N_type]  = TypeId;

    // DEBUG_END;
}       // saveConfiguration

// *********************************************************************************************
// EOF
