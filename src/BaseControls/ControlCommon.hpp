#pragma once
/*
  *    File: ControlCommon.hpp
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
#include <ESPUI.h>
#include "language.h"

// *********************************************************************************************
class cControlCommon
{
public:

    cControlCommon (
                    const String    & ConfigName,
                    ControlType     uiControltype,
                    const String    & Title,
                    const String    & DefaultValue,
                    uint32_t        MaxStrLen);
    virtual~cControlCommon ();

    virtual void    AddControls (uint16_t TabId, ControlColor color);
    virtual void    Callback (Control * sender, int type);
    virtual String  &get ();
    virtual String  getDefault ()   {return DefaultValue;}
    virtual String  GetTitle ()     {return Title;}
    virtual bool    GetAndResetValueChangedFlag ();
    virtual void    ResetToDefaults ();
    virtual void    restoreConfiguration (JsonObject & json);
    virtual void    saveConfiguration (JsonObject & json);
    virtual bool    set (const String & value, String & ResponseMessage, bool ForceUpdate = false);
    virtual void    setSaveUpdates (bool value)     {SaveUpdate = value;}
    virtual void    SetTitle (const String & value) {Title = value;}
    virtual bool    validate (const String & value, String & ResponseMessage, bool ForceUpdate);

    enum eCssStyle
    {
        CssStyleBlack = 0,
        CssStyleGreen,
        CssStyleMaroon,
        CssStyleRed,
        CssStyleTransparent,
        CssStyleWhite,
        CssStyleBlack_bw,
        CssStyleGreen_bw,
        CssStyleMaroon_bw,
        CssStyleRed_bw,
        CssStyleTransparent_bw,
        CssStyleWhite_bb,
        CssStyleWhite_grey
    };

    String          GetCssStyle (eCssStyle Style);
    virtual void    setControl (const String & value, eCssStyle style);
    virtual void    setControlStyle (eCssStyle style);
    virtual void    setControlLabel (const String & value);

    uint16_t    ControlId       = Control::noParent;
    eCssStyle   ControlStyle    = eCssStyle::CssStyleBlack;

    enum ePanelStyle
    {
        PanelStyle115 = 0,
        PanelStyle125,
        PanelStyle135,
        PanelStyle300,
        PanelStyle115_black,
        PanelStyle125_black,
        PanelStyle135_black,
        PanelStyle300_black,
        PanelStyle115_white,
        PanelStyle125_white,
        PanelStyle135_white,
        PanelStyle300_white
    };
    String          GetPanelStyle (ePanelStyle Style);
    virtual void    setControlPanelStyle (ePanelStyle style);
protected:

    ePanelStyle     ControlPanelStyle = PanelStyle125;
    String          DataValueStr;

    bool            SaveUpdate  = true;
    bool            SkipSetLog  = false;
    bool            Booting     = true;
    const String    ConfigName;
    const String    DefaultValue;
    bool            ValueChanged = false;

private:
    String          Title = emptyString;
    ControlType     uiControltype;
    uint32_t        MaxDataLength = 0;
};  // class cControlCommon

// *********************************************************************************************
// OEF
