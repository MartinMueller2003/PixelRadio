#pragma once
/*
   File: PiCode.cpp
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
class cPiCode : public cControlCommon
{
public:

    cPiCode ();
    virtual ~cPiCode ()    {}
    uint32_t    get32 ();
    void        ResetToDefaults ();
    bool        set (const String & value, String & Response, bool ForceUpdate          = false);
    bool        validate (const String & value, String & Response, bool ForceUpdate     = false);
private:
    uint32_t    StringToNumber (const String & value);
};      // class cPiCode

extern cPiCode PiCode;

// *********************************************************************************************
// OEF
