#pragma once
/*
   File: NumberControl.cpp
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
class cNumberControl : public cControlCommon
{
public:

    cNumberControl (
    const String    & ConfigName,
    const String    & Title,
    uint32_t        DefaultValue,
    uint32_t        _MinValue,
    uint32_t        _MaxValue);

    virtual ~cNumberControl ()    {}
    virtual uint32_t    get32 () {return StringToNumber (get ());}
    virtual bool        validate (const String & value, String & Response, bool ForceUpdate = false);
private:
    uint32_t            StringToNumber (const String & value);
    uint32_t MinValue;
    uint32_t MaxValue;
};  // class cNumberControl


// *********************************************************************************************
// OEF
