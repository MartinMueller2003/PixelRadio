/*
  *    File: ControllerGpioSERIAL.cpp
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
#include <ArduinoLog.h>
#include "ControllerGpioSERIAL.hpp"
#include "language.h"
#include "memdebug.h"

static const PROGMEM char Name [] = "GPIO SERIAL";

// *********************************************************************************************
cControllerGpioSERIAL::cControllerGpioSERIAL () :   cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::GPIO_SERIAL_CNTRL)
{
    SerialControl.initSerialControl (& Serial1);
    SetTitle (Name);
}

// *********************************************************************************************
cControllerGpioSERIAL::~cControllerGpioSERIAL ()
{}

// ************************************************************************************************
void cControllerGpioSERIAL::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    // DEBUG_V(String("    Name: ") + Name);
    // DEBUG_V(String("GetTitle: ") + GetTitle());

    SetTitle (Name);

    // DEBUG_V(String("    Name: ") + Name);
    // DEBUG_V(String("GetTitle: ") + GetTitle());

    cControllerCommon::AddControls (TabId, color);
    SerialControl.AddControls (ControlId, color);

    // DEBUG_END;
}

// ************************************************************************************************
bool cControllerGpioSERIAL::GetNextRdsMessage (const String & value, c_ControllerMgr::RdsMsgInfo_t & Response)
{
    // DEBUG_START;

    if (!AllMessagesPlayed || ControllerIsEnabled ())
    {
        AllMessagesPlayed = Messages.GetNextRdsMessage (value, Response);
    }

    // DEBUG_END;
    return AllMessagesPlayed;
}

// *********************************************************************************************
void cControllerGpioSERIAL::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::restoreConfiguration (config);
    SerialControl.restoreConfiguration (config);

    // DEBUG_END;
}   // restoreConfiguration

// *********************************************************************************************
void cControllerGpioSERIAL::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::saveConfiguration (config);
    SerialControl.saveConfiguration (config);

    // DEBUG_END;
}   // saveConfiguration

// *********************************************************************************************
bool cControllerGpioSERIAL::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cControllerCommon::set (value, ResponseMessage, SkipLogOutput, ForceUpdate);

    SerialControl.SetControllerEnabled (getBool ());

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
// EOF
