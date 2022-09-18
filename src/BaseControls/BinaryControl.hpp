#pragma once
/*
  *    File: BinaryControl.cpp
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
#include <map>
#include "ControlCommonMsg.hpp"

// *********************************************************************************************
class cBinaryControl : public cControlCommonMsg
{
public:

    cBinaryControl (const String & ConfigName, const String & Title, bool DefaultValue);
    virtual~cBinaryControl ()    {}

    void            addInputCondition (const String & Name, bool value);
    virtual bool    getBool () {return DataValue;}
    virtual void    restoreConfiguration (JsonObject & json);
    virtual void    saveConfiguration (JsonObject & json);
    virtual bool    set (const String & value, String & ResponseMessage, bool ForceUpdate = false);
    virtual void    setOffMessage (const String & value, eCssStyle style);
    virtual void    setOffMessageStyle (eCssStyle style);
    virtual void    setOnMessage (const String & value, eCssStyle style);
    virtual void    setOnMessageStyle (eCssStyle style);
    virtual bool    validate (const String & value, String & ResponseMessage, bool ForceUpdate);

private:
    bool        DataValue = false;

    String      OnString;
    String      OffString;
    eCssStyle   OnStyle     = eCssStyle::CssStyleBlack;
    eCssStyle   OffStyle    = eCssStyle::CssStyleBlack;
};  // class cBinaryControl

// *********************************************************************************************
// OEF
