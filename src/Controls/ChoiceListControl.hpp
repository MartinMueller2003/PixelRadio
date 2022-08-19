#pragma once
/*
   File: ChoiceListControl.cpp
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
#include <map>

// *********************************************************************************************
class cChoiceListControl : public cControlCommon
{
public:
#define ChoiceList_t std::map <String, String>

cChoiceListControl (const String & ConfigName, const String & Title, ChoiceList_t & ChoiceList);
virtual ~cChoiceListControl ()    {}

virtual void            AddControls (uint16_t value, ControlColor color);
virtual bool            validate (const String & value, String & ResponseMessage, bool ForceUpdate);
virtual uint32_t        get32 ();

private:
ChoiceList_t & ChoiceList;
};      // class cChoiceListControl

// *********************************************************************************************
// OEF
