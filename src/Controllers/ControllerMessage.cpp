/*
   File: ControllerMessage.cpp
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
#include "ControllerMessage.h"
#include "Language.h"
#include "PixelRadio.h"
#include <map>

#if __has_include ("memdebug.h")
 # include "memdebug.h"
#endif //  __has_include("memdebug.h")

// *********************************************************************************************
c_ControllerMessage::c_ControllerMessage ()
{
    // DEBUG_START;
    // DEBUG_END;
}       // c_ControllerMessage

// *********************************************************************************************
c_ControllerMessage::~c_ControllerMessage ()
{
    // DEBUG_START;

    // DEBUG_V(String("Remove Message: '") + MessageText + "'");

    if (Control::noParent != MessageElementId)
    {
        // DEBUG_V("Remove Message controls ");
        ESPUI.removeControl (MessageElementId);
        MessageElementId = Control::noParent;
    }
    // DEBUG_END;
}       // c_ControllerMessage

// ************************************************************************************************
void c_ControllerMessage::Activate (bool value)
{
    // DEBUG_START;

    // DEBUG_V(String("            MessageText: ") + MessageText);
    // DEBUG_V(String("  ActiveParentElementId: ") + String(MessageElementIds->ActiveChoiceListElementId));
    // DEBUG_V(String("  HiddenParentElementId: ") + String(MessageElementIds->HiddenChoiceListElementId));

    do  // once
    {
        if (nullptr == MessageElementIds)
        {
            // DEBUG_V("No control structure defined yet");
            break;
        }
        Control * MsgSelectControl = ESPUI.getControl (MessageElementId);

        if (!MsgSelectControl)
        {
            // DEBUG_V("Could not find this message control in ESPUI.");
            break;
        }

        if (!value)
        {
            // DEBUG_V("Add message to the hidden choice list");
            MsgSelectControl->parentControl = MessageElementIds->HiddenChoiceListElementId;
        }
        else
        {
            // DEBUG_V("Add message to the active choice list");
            MsgSelectControl->parentControl = MessageElementIds->ActiveChoiceListElementId;
        }
        ESPUI.updateControl (MsgSelectControl);

        // DEBUG_V(String("    MsgSelectControl ID: ") + String(MsgSelectControl->id));
        // DEBUG_V(String("MsgSelectControl Parent: ") + String(MsgSelectControl->parentControl));
    } while (false);

    // DEBUG_END;
}       // Activate

// ************************************************************************************************
void c_ControllerMessage::AddControls (MessageElementIds_t * _MessageElementIds)
{
    // DEBUG_START;

    MessageElementIds = _MessageElementIds;

    // DEBUG_V(String("                  Message: '") + MessageText + "'");
    // DEBUG_V(String("ActiveChoiceListElementId: '") + String(MessageElementIds->ActiveChoiceListElementId) + "'");
    // DEBUG_V(String("HiddenChoiceListElementId: '") + String(MessageElementIds->HiddenChoiceListElementId) + "'");
    // DEBUG_V(String("         EnabledElementId: '") + String(MessageElementIds->EnabledElementId) + "'");
    // DEBUG_V(String(" DisplayDurationElementId: '") + String(MessageElementIds->DisplayDurationElementId) + "'");

    do  // once
    {
        if (nullptr == MessageElementIds)
        {
            // DEBUG_V("No control structure defined yet");
            break;
        }

        if (Control::noParent == MessageElementIds->ActiveChoiceListElementId)
        {
            // DEBUG_V("UI is not set up yet");
            break;
        }

        if (Control::noParent == MessageElementId)
        {
            // DEBUG_V(String("Create Choice List Entry on Active Choice list"));
            MessageElementId = ESPUI.addControl (ControlType::Option,
                                                 MessageText.c_str (),
                                                 MessageText,
                                                 ControlColor::Turquoise,
                                                 MessageElementIds->ActiveChoiceListElementId);
        }
        // DEBUG_V(String(" EspuiMessageElementId: '") + String(MessageElementId) + "'");

        // DEBUG_V("Attach callbacks to the Message Details Pane.");
        Control * DurationControl = ESPUI.getControl (MessageElementIds->DisplayDurationElementId);

        if (DurationControl)
        {
            DurationControl->user               = nullptr;
            DurationControl->extendedCallback   =
                [] (Control * sender, int type, void * parm)
                {
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_ControllerMessage *> (parm)->CbDuration (sender, type);
                    }
                };

            ESPUI.updateControl (DurationControl);
        }
        Control * MsgEnabledControl = ESPUI.getControl (MessageElementIds->EnabledElementId);

        if (MsgEnabledControl)
        {
            MsgEnabledControl->user             = nullptr;
            MsgEnabledControl->extendedCallback =
                [] (Control * sender, int type, void * parm)
                {
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_ControllerMessage *> (parm)->CbEnabled (sender, type);
                    }
                };
            ESPUI.updateControl (MsgEnabledControl);
        }
    } while (false);

    // DEBUG_END;
}       // AddControls

// *********************************************************************************************
void c_ControllerMessage::CbDuration (Control * sender, int type)
{
    // DEBUG_START;

    DurationSec = atoi (sender->value.c_str ());

    displaySaveWarning ();
    Log.infoln ((String (F ("FPPD Message Duration Set to: ")) + String (DurationSec)).c_str ());

    // DEBUG_END;
}       // EnabledCb

// *********************************************************************************************
void c_ControllerMessage::CbEnabled (Control * sender, int type)
{
    // DEBUG_START;

    Enabled = sender->value.equals ("1");
    // DEBUG_V(String("Enabled: ") + String(Enabled));

    displaySaveWarning ();
    Log.infoln ((String (F ("FPPD Message Duration Set to: ")) + String (Enabled ? "On" : "Off")).c_str ());

    // DEBUG_END;
}       // EnabledCb

// *********************************************************************************************
void c_ControllerMessage::GetMessage (c_ControllerMgr::RdsMsgInfo_t & Response)
{
    // DEBUG_START;

    Response.Text               = MessageText;
    Response.DurationMilliSec   = DurationSec * 1000;

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerMessage::RestoreConfig (ArduinoJson::JsonObject config)
{
    // DEBUG_START;

    if (config.containsKey (N_message))
    {
        MessageText = (const char *)config[N_message];
        // DEBUG_V(String("MessageText: ") + MessageText);
    }

    if (config.containsKey (N_durationSec))
    {
        DurationSec = config[N_durationSec];
        // DEBUG_V(String("DurationSec: ") + String(DurationSec));
    }

    if (config.containsKey (N_enabled))
    {
        Enabled = config[N_enabled];
        // DEBUG_V(String("    Enabled: ") + String(Enabled));
    }
    // DEBUG_END;
}       // RestoreConfig

// *********************************************************************************************
void c_ControllerMessage::SaveConfig (ArduinoJson::JsonObject config)
{
    // DEBUG_START;
    // DEBUG_V(String("Message: ") + MessageText);

    config[N_message]           = MessageText;
    config[N_durationSec]       = DurationSec;
    config[N_enabled]           = Enabled;

    // DEBUG_END;
}       // SaveConfig

// ************************************************************************************************
void c_ControllerMessage::SelectMessage ()
{
    // DEBUG_START;

    // DEBUG_V(String("    Message: '") + MessageText + "'");

    do  // once
    {
        if (nullptr == MessageElementIds)
        {
            // DEBUG_V("No control structure defined yet");
            break;
        }
        Activate (true);

        Control * control = ESPUI.getControl (MessageElementIds->ActiveChoiceListElementId);

        if (control)
        {
            // DEBUG_V("Update Selected item");
            control->value = MessageText;
            ESPUI.updateControl (control);
        }
        // DEBUG_V(String("Active List: '") + control->value + "'");

        control = ESPUI.getControl (MessageElementIds->DisplayDurationElementId);

        if (control)
        {
            // DEBUG_V("Set up Duration");
            control->value      = String (DurationSec);
            control->user       = this;
            ESPUI.updateControl (MessageElementIds->DisplayDurationElementId);
        }
        control = ESPUI.getControl (MessageElementIds->EnabledElementId);

        if (control)
        {
            // DEBUG_V("Set up enabled CB");
            control->value      = String (Enabled ? "1" : "0");
            control->user       = this;
            ESPUI.updateControl (MessageElementIds->EnabledElementId);
        }
    } while (false);

    // DEBUG_END;
}       // SelectMessage

// *********************************************************************************************
void c_ControllerMessage::SetMessage (String & value)
{
    // DEBUG_START;

    // DEBUG_V(String("Message: '") + MessageText + "'");
    // DEBUG_V(String("  value: '") + value + "'");

    MessageText = value;

    // DEBUG_END;
}       // SetMessage

// *********************************************************************************************
// EOF
