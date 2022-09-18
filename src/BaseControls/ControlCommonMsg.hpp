#pragma once
/*
  *    File: ControlCommonMsg.hpp
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
#include "ControlCommon.hpp"

// *********************************************************************************************
class cControlCommonMsg : public cControlCommon
{
public:

    cControlCommonMsg (
                       const String & ConfigName,
                       ControlType  uiControltype,
                       const String & Title,
                       const String & DefaultValue,
                       uint32_t     MaxStrLen);
    virtual~cControlCommonMsg ();

    virtual void AddControls (uint16_t TabId, ControlColor color);

    virtual void    setMessage (const String & value, eCssStyle style);
    virtual void    setMessage (const String & value);
    virtual void    setMessageStyle (eCssStyle style);


    virtual void setMessagePanelStyle (ePanelStyle style);
protected:
    uint16_t    MessageId           = Control::noParent;
    eCssStyle   MessageStyle        = eCssStyle::CssStyleBlack;
    ePanelStyle MessagePanelStyle   = PanelStyle125;
};  // class cControlCommonMsg

// *********************************************************************************************
// OEF
