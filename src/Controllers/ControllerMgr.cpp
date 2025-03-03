/*
  *    File: ControllerMgr.cpp
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
  *
  */

// *********************************************************************************************
#include <Arduino.h>

#include "ControllerCommon.h"
#include "ControllerFPPD.h"
#include "ControllerHTTP.h"
#include "ControllerLOCAL.h"
#include "ControllerMgr.h"
#include "ControllerMQTT.h"
#include "ControllerNONE.h"
#include "ControllerUsbSERIAL.hpp"
#include "ControllerGpioSERIAL.hpp"
#include "RdsMessageOrder.hpp"
#include "language.h"

#include "memdebug.h"

struct ControllerDefinition_t
{
    c_ControllerMgr::ControllerTypeId_t Type;
    uint16_t                            ActiveBit;
    uint16_t                            SendingBit;
};

static const ControllerDefinition_t ControllerDefinitions[] =
{
    {
        c_ControllerMgr::ControllerTypeId_t::USB_SERIAL_CNTRL,  SERIAL_CONTROLLER_ACTIVE_FLAG,
        0x0008
    },
    {
        c_ControllerMgr::ControllerTypeId_t::GPIO_SERIAL_CNTRL, SERIAL1_CONTROLLER_ACTIVE_FLAG,
        0x0200
    },
    {
        c_ControllerMgr::ControllerTypeId_t::MQTT_CNTRL,  MQTT_CONTROLLER_ACTIVE_FLAG,
        0x0004
    },
    {
        c_ControllerMgr::ControllerTypeId_t::FPPD_CNTRL,  FPPD_CONTROLLER_ACTIVE_FLAG,
        0x0100
    },
    {
        c_ControllerMgr::ControllerTypeId_t::HTTP_CNTRL,  HTTP_CONTROLLER_ACTIVE_FLAG,
        0x0002
    },
    {
        c_ControllerMgr::ControllerTypeId_t::LOCAL_CNTRL, LOCAL_CONTROLLER_ACTIVE_FLAG,
        0x0001
    },
    {
        c_ControllerMgr::ControllerTypeId_t::NO_CNTRL,                                                                         0,
        0x0000
    },
};

// *********************************************************************************************
c_ControllerMgr::c_ControllerMgr ()
{
    // DEBUG_START;

    for (auto & CurrentDefinition : ControllerDefinitions)
    {
        uint32_t index = int(CurrentDefinition.Type);
        ListOfControllers[index].ControllerId   = CurrentDefinition.Type;
        ListOfControllers[index].ActiveBit      = CurrentDefinition.ActiveBit;
        ListOfControllers[index].SendingBit     = CurrentDefinition.SendingBit;

        switch (CurrentDefinition.Type)
        {
            case ControllerTypeId_t::NO_CNTRL:
            {
                // DEBUG_V("NO_CNTRL");
                ListOfControllers[index].pController = new c_ControllerNONE ();
                break;
            }

            case ControllerTypeId_t::GPIO_SERIAL_CNTRL:
            {
                // DEBUG_V("GPIO_SERIAL_CNTRL");
                ListOfControllers[index].pController = new cControllerGpioSERIAL ();
                break;
            }

            case ControllerTypeId_t::USB_SERIAL_CNTRL:
            {
                // DEBUG_V("USB_SERIAL_CNTRL");
                ListOfControllers[index].pController = new cControllerUsbSERIAL ();
                break;
            }

            case ControllerTypeId_t::MQTT_CNTRL:
            {
                // DEBUG_V("MQTT_CNTRL");
                ListOfControllers[index].pController = new c_ControllerMQTT ();
                break;
            }

            case ControllerTypeId_t::FPPD_CNTRL:
            {
                // DEBUG_V("FPPD_CNTRL");
                ListOfControllers[index].pController = new c_ControllerFPPD ();
                break;
            }

            case ControllerTypeId_t::HTTP_CNTRL:
            {
                // DEBUG_V("HTTP_CNTRL");
                ListOfControllers[index].pController = new c_ControllerHTTP ();
                break;
            }

            case ControllerTypeId_t::LOCAL_CNTRL:
            {
                // DEBUG_V("LOCAL_CNTRL");
                ListOfControllers[index].pController = new c_ControllerLOCAL ();
                break;
            }

            default:
            {
                // DEBUG_V("default");
                ListOfControllers[index].pController = new c_ControllerNONE ();
                // post an error
                break;
            }
        }   // end switch(index)
    }

    // DEBUG_END;
}   // c_ControllerMgr

// *********************************************************************************************
c_ControllerMgr::~c_ControllerMgr ()
{
    for (auto & CurrentController : ListOfControllers)
    {
        if (nullptr != CurrentController.pController)
        {
            delete CurrentController.pController;
            CurrentController.pController = nullptr;
        }
    }
}

// *********************************************************************************************
void c_ControllerMgr::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    RdsMessageOrder.AddControls (TabId, color);

    for (auto & CurrentController : ListOfControllers)
    {
        // DEBUG_V(String("Add controls: ") + CurrentController.pController->GetName());
        // DEBUG_V(String("GetDataValueStr: '") + CurrentController.pController->GetTitle() + "':'" + CurrentController.pController->GetDataValueStr() + "'");

        CurrentController.pController->AddControls (TabId, color);
    }

    // DEBUG_END;
}   // AddControls

// *********************************************************************************************
void c_ControllerMgr::begin ()
{
    // DEBUG_START;
    for (auto & CurrentController : ListOfControllers)
    {
        // DEBUG_V(String("Begin: ") + CurrentController.pController->GetName());
        // DEBUG_V(String("pController: 0x") + String(uint32_t(CurrentController.pController), HEX));
        CurrentController.pController->begin ();
    }

    // DEBUG_END;
}   // begin

// *********************************************************************************************
void c_ControllerMgr::ClearAllMessagesPlayedConditions ()
{
    // DEBUG_START;

    for (auto & CurrentController : ListOfControllers)
    {
        // DEBUG_V(String("Controller: '") + CurrentController.pController->GetName() + "' is cleared");
        CurrentController.pController->ClearAllMessagesPlayed ();
    }

    // DEBUG_END;
}  // ClearAllMessagesPlayedConditions

// *********************************************************************************************
cControllerCommon * c_ControllerMgr::GetControllerById (ControllerTypeId_t Id) {return ListOfControllers[Id].pController;}  // GetControllerById

// *********************************************************************************************
bool c_ControllerMgr::GetNextRdsMessage (RdsMsgInfo_t & Response)
{
    // DEBUG_START;
    bool AllMsgsPlayed = true;

    Response.DurationMilliSec   = 0;
    Response.Text               = F ("No Controllers Available");
    CurrentSendingControllerId  = ControllerTypeId_t::NO_CNTRL;

    do  // once
    {
        if (!RdsOutputEnabled)
        {
            // DEBUG_V("Rds Output is disabled");
            break;
        }

        // DEBUG_V("clear the Response found message indicator");
        Response.DurationMilliSec = 0;

        if (RdsMessageOrder.getBool ())
        {
            // DEBUG_V("we are in strict priority order. Always start at the highest priority controller");
            currentControllerIndex = ControllerTypeId_t::NumControllerTypes;
            ClearAllMessagesPlayedConditions ();
        }

        for (uint32_t count = 0;count < ControllerTypeId_t::NumControllerTypes;++count)
        {
            if (currentControllerIndex >= ControllerTypeId_t::NumControllerTypes)
            {
                // DEBUG_V("Wrap");
                currentControllerIndex = ControllerTypeId_t::ControllerIdStart;
                ClearAllMessagesPlayedConditions ();
            }

            auto & CurrentController = ListOfControllers[currentControllerIndex];

            if ((CurrentController.pController->GetAllMessagesSentCondition ()) ||
                (!CurrentController.pController->ControllerIsEnabled ()))
            {
                // DEBUG_V(String("Controller: '") + CurrentController.pController->GetName() + "' is disabled");
                ++currentControllerIndex;
                continue;
            }

            // DEBUG_V(String("Controller: '") + CurrentController.pController->GetName() + "': Get Next Message");
            Response.Text = F ("No Messages Available");
            String Temp = CurrentController.pController->GetName ();
            AllMsgsPlayed = CurrentController.pController->GetNextRdsMessage (Temp, Response);

            if (Response.DurationMilliSec)
            {
                // DEBUG_V("Found a message to send");

                Response.ControllerName     = CurrentController.pController->GetName ();
                CurrentSendingControllerId  = CurrentController.ControllerId;

                // DEBUG_V(String("  Duration (ms): ") + String(Response.DurationMilliSec));
                // DEBUG_V(String("           Text: ") + String(Response.Text));
                // DEBUG_V(String("Controller Name: ") + String(Response.ControllerName));
                break;
            }
            else
            {
                // DEBUG_V("move on to the next controller");
                ++currentControllerIndex;
            }
        }
    } while (false);

    // DEBUG_END;
    return AllMsgsPlayed;
}  // GetNextRdsMessage

// *********************************************************************************************
uint16_t c_ControllerMgr::getControllerStatusSummary ()
{
    // DEBUG_START;

    uint16_t Response = 0;

    for (auto & CurrentController : ListOfControllers)
    {
        if (CurrentController.pController->ControllerIsEnabled ())
        {
            Response |= CurrentController.ActiveBit;
        }

        if (CurrentController.ControllerId == CurrentSendingControllerId)
        {
            Response |= CurrentController.SendingBit;
        }
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
String c_ControllerMgr::GetName (ControllerTypeId_t Id) {return ListOfControllers[Id].pController->GetName ();}  // GetName

// *********************************************************************************************
void c_ControllerMgr::poll ()
{
    // _ DEBUG_START;
    for (auto & CurrentController : ListOfControllers)
    {
        CurrentController.pController->poll ();
    }

    // _ DEBUG_END;
}

// *********************************************************************************************
void c_ControllerMgr::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    // serializeJsonPretty (config, Serial);

    do  // once
    {
        RdsMessageOrder.restoreConfiguration (config);
        ReadFromJSON (RdsOutputEnabled, config, F ("RdsOutputEnabled"));

        if (false == config.containsKey (N_controllers))
        {
            // DEBUG_V("No Config Found in: ");
            // serializeJsonPretty(config, Serial);
            // Serial.println();
            break;
        }

        // DEBUG_V();

        JsonArray ArrayOfControllerConfigs = config[N_controllers];

        for (auto CurrentControllerConfig : ArrayOfControllerConfigs)
        {
            if (false == CurrentControllerConfig.containsKey (N_type))
            {
                // DEBUG_V("No controller type ID found");
                continue;
            }

            uint32_t type = CurrentControllerConfig[N_type];
            // DEBUG_V(String("Type ID: ") + String(type));

            JsonObject Temp = CurrentControllerConfig;
            // DEBUG_V("Individual Controller Config");
            // serializeJsonPretty(Temp, Serial);
            // DEBUG_V("\nIndividual Controller Config");
            ListOfControllers[type].pController->restoreConfiguration (Temp);
        }
    } while (false);

    // DEBUG_END;
}   // restoreConfiguration

// *********************************************************************************************
void c_ControllerMgr::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    do  // once
    {
        RdsMessageOrder.restoreConfiguration (config);
        config[F ("RdsOutputEnabled")] = RdsOutputEnabled;

        if (!config.containsKey (N_controllers))
        {
            // DEBUG_V();
            config.createNestedArray (N_controllers);
        }

        // DEBUG_V();

        JsonArray ControllerConfigs = config[N_controllers];

        for (auto & CurrentController : ListOfControllers)
        {
            JsonObject ControllerConfig = ControllerConfigs.createNestedObject ();
            CurrentController.pController->saveConfiguration (ControllerConfig);
        }
    } while (false);

    // serializeJsonPretty (config, Serial);

    // DEBUG_END;
}   // saveConfiguration

c_ControllerMgr ControllerMgr;

// *********************************************************************************************
// EOF
