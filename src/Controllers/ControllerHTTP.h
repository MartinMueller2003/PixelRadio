/*
  *    File: ControllerHTTP.h
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.0
  *    Creation: Dec-16-2021
  *    Revised:  Apr-06-2022
  *    Public Release:
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *    Revision History: See PixelRadio.cpp
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
  *    absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *     Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
  */

// *********************************************************************************************
#pragma once
#include "ControllerCommon.h"
#include "ControllerMessages.h"
#include "CommandProcessor.hpp"

class c_ControllerHTTP : public cControllerCommon
{
public:

    c_ControllerHTTP ();
    virtual~c_ControllerHTTP ();
    void poll ();
    void GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response);

private:
    void init ();
    cCommandProcessor   CommandProcessor;
    bool HasBeenInitialized = false;
    uint16_t                EspuiControlID = 0;
    c_ControllerMessages    Messages;
};  // c_ControllerHTTP

// *********************************************************************************************
// EOF
