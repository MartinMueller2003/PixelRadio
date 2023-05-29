/*
  *    File: ControllerFPPDSequence.cpp
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

#include "ControllerFPPDSequence.h"
#include "Language.h"
#include <ESPUI.h>

#if __has_include ("memdebug.h")
 #include "memdebug.h"
#endif //  __has_include("memdebug.h")

// *********************************************************************************************
c_ControllerFPPDSequence::c_ControllerFPPDSequence ()
{
    // DEBUG_START;

    // DEBUG_END;
}   // c_ControllerFPPDSequence

// *********************************************************************************************
c_ControllerFPPDSequence::~c_ControllerFPPDSequence ()
{
    // DEBUG_START;

    // DEBUG_V(String("Name: ") + Name);

    Messages->clear ();
    // DEBUG_V();

    if (Control::noParent != EspuiElementId)
    {
        // DEBUG_V("remove Controls");
        ESPUI.removeControl (EspuiElementId);
    }

    // DEBUG_END;
}   // ~c_ControllerFPPDSequence

// ************************************************************************************************
void c_ControllerFPPDSequence::Activate (bool value)
{
    // DEBUG_START;
    // DEBUG_V(String("Sequence: ") + Name);
    // DEBUG_V(String("   value: ") + String(value));

    do  // once
    {
        if (!value)
        {
            // DEBUG_V("Deactivate is ignored.");
            break;
        }

        if (Control::noParent == EspuiElementId)
        {
            // DEBUG_V("We have not been set up yet");
            break;
        }

        // DEBUG_V();

        Messages->ActivateMessageSet (Name);
    } while (false);

    // DEBUG_END;
}   // Activate

// ************************************************************************************************
void c_ControllerFPPDSequence::AddControls (uint16_t ctrlTab, uint16_t ParentElementId)
{
    // DEBUG_START;

    EspuiParentElementId    = ParentElementId;
    EspuiRootElementId      = ctrlTab;

    if (Control::noParent == EspuiElementId)
    {
        // DEBUG_V(String("Adding Control for ") + Name);
        EspuiElementId = ESPUI.addControl (
            ControlType::Option,
            Name.c_str (),
            Name,
            ControlColor::None,
            EspuiParentElementId);
    }
    else
    {
        // DEBUG_V(String("NOT Adding Controls"));
    }

    // DEBUG_V(String("EspuiElementId: ") + String(EspuiElementId));

    Messages->SetShowFseqNameSelection (true);
    Messages->AddControls (EspuiRootElementId);

    // DEBUG_END;
}   // AddControls

// *********************************************************************************************
void c_ControllerFPPDSequence::RestoreConfig (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    // serializeJsonPretty(config, Serial);

    if (config.containsKey (N_name))
    {
        // DEBUG_V("Found Name");
        Name = (const char *)config[N_name];
    }

    // DEBUG_V(String("Name: ") + Name);

    // DEBUG_END;
}   // RestoreConfig

// *********************************************************************************************
void c_ControllerFPPDSequence::SaveConfig (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    config[N_name] = Name;

    // DEBUG_END;
}   // SaveConfig

// ************************************************************************************************
void c_ControllerFPPDSequence::SetName (String & value)
{
    // DEBUG_START;

    // DEBUG_V(String("value: ") + value);
    // DEBUG_V(String(" Name: ") + Name);

    Name = value;

    if (Control::noParent != EspuiElementId)
    {
        ESPUI.updateControlValue (EspuiElementId, Name);
    }

    Messages->ActivateMessageSet (Name);

    // DEBUG_END;
}   // SetName

// *********************************************************************************************
// EOF
