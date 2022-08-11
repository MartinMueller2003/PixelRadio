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
                        cControlCommon (String _ConfigName);
    virtual             ~cControlCommon() {}

    virtual void        AddControls (uint16_t TabId) = 0;

    virtual uint32_t    get() { return DataValue; }
    virtual void        restoreConfiguration(JsonObject &json);
    virtual void        saveConfiguration (JsonObject & json);
    virtual bool        set(String & value, String & Response) = 0;
    
// Callbacks need to be public 
    virtual void        Callback(Control *sender, int type);
    
protected:
    void        AddControls (uint16_t value, ControlType uiControltype, ControlColor color);
    uint16_t    TabId           = Control::noParent;
    uint16_t    ControlId       = Control::noParent;
    uint16_t    StatusMessageId = Control::noParent;

    uint32_t    DataValue = 0;
    String      DataValueStr;

    String ActiveLabelStyle     = CSS_LABEL_STYLE_TRANSPARENT;
    String InactiveLabelStyle   = CSS_LABEL_STYLE_BLACK;

private:
    String      ConfigName;
};

// *********************************************************************************************
// OEF
