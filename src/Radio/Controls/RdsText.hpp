#pragma once
/*
   File: RdsText.cpp
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
#include "OldControlCommon.hpp"
#include "ControllerMgr.h"
#include <Arduino.h>

// *********************************************************************************************
class cRdsText : public cOldControlCommon
{
public:

    cRdsText ();
    virtual ~cRdsText ()    {}

    void        AddControls (uint16_t TabId, ControlColor color);
    void        poll ();
    bool        set (String &value, String &Response);

private:

    void        UpdateStatus ();
    void        updateRdsMsgRemainingTime (uint32_t now);

    uint16_t homeTextMsgID      = Control::noParent;
    uint16_t homeRdsTmrID       = Control::noParent;

    c_ControllerMgr::RdsMsgInfo_t RdsMsgInfo;
    uint32_t CurrentMsgEndTime          = 0;
    uint32_t CurrentMsgLastUpdateTime   = 0;
    String LastMessageSent;
};      // class cRdsText

extern cRdsText RdsText;

// *********************************************************************************************
// OEF
