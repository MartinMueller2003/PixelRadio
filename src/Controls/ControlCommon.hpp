#pragma once
/*
   File: ControlCommon.hpp
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
#include <ESPUI.h>
#include "language.h"

// *********************************************************************************************
class cControlCommon
{
public:

    cControlCommon (const String &ConfigName, ControlType uiControltype, const String &Title);
    virtual ~cControlCommon ();

    virtual void        AddControls (uint16_t TabId, ControlColor color);
    virtual void        Callback (Control * sender, int type);
    virtual String&     get ();
    virtual void        restoreConfiguration (JsonObject &json);
    virtual void        saveConfiguration (JsonObject &json);
    virtual bool        set (const String &value, String &ResponseMessage, bool ForceUpdate             = true);
    virtual bool        validate (const String &value, String &ResponseMessage, bool ForceUpdate        = true);

protected:

    enum eCssStyle
    {
        CssStyleBlack = 0,
        CssStyleGreen,
        CssStyleMaroon,
        CssStyleRed,
        CssStyleTransparent,
        CssStyleWhite,
    };

    uint16_t ControlId          = Control::noParent;
    uint16_t StatusMessageId    = Control::noParent;

    eCssStyle ControlStyle              = CssStyleBlack;
    eCssStyle ActiveLabelStyle          = CssStyleBlack;
    eCssStyle InactiveLabelStyle        = CssStyleBlack;

    enum ePanelStyle
    {
        PanelStyle115 = 0,
        PanelStyle125,
        PanelStyle135,
        PanelStyle300,
    };
    ePanelStyle ControlPanelStyle       = PanelStyle125;
    ePanelStyle MessagePanelStyle       = PanelStyle125;

    String DataValueStr;

    bool SkipSetLog = false;

private:
    ControlType uiControltype;
    const String &ConfigName;
    const String &Title;
};      // class cControlCommon

// *********************************************************************************************
// OEF
