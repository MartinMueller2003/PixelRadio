#pragma once
/*
   File: BaudrateControl.h
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
#include <Arduino.h>
#include "ChoiceListControl.hpp"

class cBaudrateControl : public cChoiceListControl
{
public:

    cBaudrateControl ();
    virtual ~cBaudrateControl ();
    virtual void AddControls (uint16_t value, ControlColor color);
}; // class cBaudrateControl

// *********************************************************************************************
// EOF
