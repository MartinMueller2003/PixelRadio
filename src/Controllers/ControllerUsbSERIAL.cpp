/*
  *    File: ControllerUsbSERIAL.cpp
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
#include "ControllerUsbSERIAL.hpp"
#include "language.h"
#include "memdebug.h"

static const PROGMEM char Name [] = "USB SERIAL";
// ================================================================================================
cControllerUsbSERIAL::cControllerUsbSERIAL () :   cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::USB_SERIAL_CNTRL)
{
    SerialControl.initSerialControl (& Serial);
}

// ================================================================================================
cControllerUsbSERIAL::~cControllerUsbSERIAL ()
{}

// ************************************************************************************************
void cControllerUsbSERIAL::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    // DEBUG_V(String("GetDataValueStr: '") + GetTitle() + "':'" + GetDataValueStr() + "'");
    cControllerCommon::AddControls (TabId, color);
    // DEBUG_V(String("GetDataValueStr: '") + GetTitle() + "':'" + GetDataValueStr() + "'");
    SerialControl.AddControls (ControlId, color);
    // DEBUG_V(String("GetDataValueStr: '") + GetTitle() + "':'" + GetDataValueStr() + "'");

    // DEBUG_END;
}   // AddControls

// ************************************************************************************************
void cControllerUsbSERIAL::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response)
{
    if (ControllerIsEnabled ())
    {
        Messages.GetNextRdsMessage (Response);
    }
}

// *********************************************************************************************
void cControllerUsbSERIAL::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::restoreConfiguration (config);
    SerialControl.restoreConfiguration (config);

    // DEBUG_V(String("              Value: ") + GetDataValueStr());
    // DEBUG_V(String("SerialControl Value: ") + SerialControl.GetDataValueStr());

    // DEBUG_END;
}   // restoreConfiguration

// *********************************************************************************************
void cControllerUsbSERIAL::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::saveConfiguration (config);
    SerialControl.saveConfiguration (config);

    // DEBUG_END;
}   // saveConfiguration

// *********************************************************************************************
bool cControllerUsbSERIAL::set (const String & value, String & ResponseMessage, bool SkipLogOutput, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cControllerCommon::set (value, ResponseMessage, SkipLogOutput, ForceUpdate);
    SerialControl.SetControllerEnabled (getBool ());

    // DEBUG_V(String("              Value: ") + GetDataValueStr());
    // DEBUG_V(String("SerialControl Value: ") + SerialControl.GetDataValueStr());

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
// EOF
