/*
   File: ControllerUsbSERIAL.cpp
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
#include <ArduinoLog.h>
#include "ControllerUsbSERIAL.hpp"
#include "language.h"
#include "memdebug.h"

static const PROGMEM String Name = F("USB SERIAL");
// ================================================================================================
cControllerUsbSERIAL::cControllerUsbSERIAL () : cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::USB_SERIAL_CNTRL)
{
    SerialControl.initSerialControl(&Serial);
}

// ================================================================================================
cControllerUsbSERIAL::~cControllerUsbSERIAL ()
{}

// ************************************************************************************************
void cControllerUsbSERIAL::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cControllerCommon::AddControls (TabId, color);
    SerialControl.AddControls (ControlId, color);

    /*
    #ifdef OldWay
            extern String logLevelStr;
            EspuiMsgId = ESPUI.addControl (ControlType::Label,
                                           "SERIAL_MSG",
                                           (logLevelStr.equals (F (DIAG_LOG_SILENT_STR))) ? F (CTLR_SERIAL_MSG_STR) : emptyString,
                                           ControlColor::Turquoise,
                                           ControlLabelElementId);
            ESPUI.setElementStyle (EspuiMsgId, CSS_LABEL_STYLE_BLACK);
    #endif // def OldWay
    */
    // DEBUG_END;
}       // AddControls

// ************************************************************************************************
void cControllerUsbSERIAL::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response)
{
    if (ControllerIsEnabled())
    {
        Messages.GetNextRdsMessage (Response);
    }
}

// ************************************************************************************************
void cControllerUsbSERIAL::gpioSerialControl (String paramStr, uint8_t pin)
{
    // DEBUG_START;

    bool successFlg = false;
    String Response;

    // DEBUG_V(String("Serial Controller: Received GPIO Pin-") + String(pin) + " Command");
    // Log.infoln(charBuff);

#ifdef OldWay
        successFlg = gpioCmd (paramStr, TypeId, pin);
#endif // def OldWay

    Response = String (F ("{\"")) + CMD_GPIO_STR + String (pin) + F ("\": \"");

    if (!successFlg)
    {
        Response += F ("fail\"}");
    }
    else if (paramStr == CMD_GPIO_READ_STR)
    {
        Response += String (digitalRead (pin)) + F ("\"}");
    }
    else
    {
        Response += F ("ok\"}");
    }
#ifdef OldWay
    serial_manager.println (Response.c_str ());
#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
void cControllerUsbSERIAL::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::restoreConfiguration (config);
    SerialControl.restoreConfiguration(config);

    // DEBUG_END;
}       // restoreConfiguration

// *********************************************************************************************
void cControllerUsbSERIAL::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::saveConfiguration (config);
    SerialControl.saveConfiguration(config);

    // DEBUG_END;
}       // saveConfiguration

// *********************************************************************************************
bool cControllerUsbSERIAL::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
 // DEBUG_START;

    bool Response = cControllerCommon::set(value, ResponseMessage, ForceUpdate);

    SerialControl.SetControllerEnabled(getBool());

 // DEBUG_END;
    return Response;
}

// *********************************************************************************************
// EOF
