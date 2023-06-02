#pragma once
/*
  *    File: ChoiceListControl.hpp
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
#include "ControlCommon.hpp"
#include <map>
#include <vector>

#define ChoiceListVector_t std::vector <std::pair <String, String>>

// *********************************************************************************************
class cChoiceListControl : public cControlCommon
{
public:

    cChoiceListControl (
                        const String                & ConfigName,
                        const String                & Title,
                        const String                & DefaultValue,
                        const ChoiceListVector_t    * ChoiceList);
    virtual~cChoiceListControl ()    {}

    virtual void        AddControls (uint16_t value, ControlColor color);
    virtual uint32_t    get32 ();
    uint32_t            getIndex ();
    void                RefreshOptionList (const ChoiceListVector_t * OptionList);
    // virtual bool        set32 ();
    virtual bool    setIndex (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate);
    virtual bool    setIndex (uint32_t value, String & ResponseMessage, bool SkipLogOutput,  bool ForceUpdate);
    virtual bool    validate (const String & value, String & ResponseMessage, bool ForceUpdate);

private:

    struct ChoiceListEntry
    {
        uint32_t    VectorIndex;
        uint16_t    UiId;
    };

    std::map <String, ChoiceListEntry>  KeyToChoiceVectorMap;
    const ChoiceListVector_t            * ChoiceVector;
};  // class cChoiceListControl

// *********************************************************************************************
// OEF
