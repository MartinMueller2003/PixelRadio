#pragma once
/*
   File: RssiStatus.cpp
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
#include "ControlCommon.hpp"

// *********************************************************************************************
class cRssiStatus : public cControlCommon
{
public:

    cRssiStatus ();
    virtual ~cRssiStatus ()
    {
    }

    void restoreConfiguration (JsonObject &)
    {
    }

    void saveConfiguration (JsonObject &)
    {
    }

    void        AddControls (uint16_t Tab, ControlColor color);
    int32_t     getRssi ();
    bool        set (String &, String &)
    {
        return true;
    }

    void set ();
};

// *********************************************************************************************
// OEF
