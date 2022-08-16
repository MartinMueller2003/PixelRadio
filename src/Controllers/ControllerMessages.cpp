/*
   File: ControllerMessages.cpp
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
#include "ControllerMessages.h"
#include <map>
#include "Language.h"

#if __has_include ("memdebug.h")
# include "memdebug.h"
#endif //  __has_include("memdebug.h")

static const String     DefaultTextFieldValue   = F ("Type New RDS Message Text Here. 64 Char Max");
static const String     DefaultTextWarningMsg   = F ("WARN: Instruction text cannot be in the message");
static const String     EmptyMsgWarning         = F ("WARN: Empty message is not allowed");

// *********************************************************************************************
c_ControllerMessages::c_ControllerMessages ()
{
    // DEBUG_START;

    MessageSetsSemaphore = xSemaphoreCreateMutex ();

    // DEBUG_END;
}       // c_ControllerMessages

// *********************************************************************************************
c_ControllerMessages::~c_ControllerMessages ()
{
    // DEBUG_START;

    // DEBUG_END;
}       // c_ControllerMessages

// ************************************************************************************************
void c_ControllerMessages::ActivateMessageSet (String MsgSetName)
{
    // DEBUG_START;
    // DEBUG_V(String("  MsgSetName: '") + MsgSetName + "'");

    do  // once
    {
        // DEBUG_V("Turn off all message sets");
        for (auto &MessageSet : MessageSets)
        {
            MessageSet.second.Activate (false);
        }

        CurrentMsgSetName = MsgSetName;

        if (MessageSets.end () == MessageSets.find (MsgSetName))
        {
            // DEBUG_V("Desired message set not found. Create it.");
            AddMessageSet (MsgSetName);
        }
        // DEBUG_V(String("Activate the desired message set: ") + MsgSetName);
        MessageSets[MsgSetName].Activate (true);

        if (Control::noParent == MessageElementIds.ActiveChoiceListElementId)
        {
            // DEBUG_V("No Element IDs available. Not setting the messages");
            break;
        }
        // DEBUG_V("Set Title");
        Control  * control = ESPUI.getControl (MessageElementIds.ActiveChoiceListElementId);

        if (control)
        {
            Title               = MsgSetName + F (" ") + N_Messages;
            control->label      = Title.c_str ();
            ESPUI.updateControl (control);
        }
        // DEBUG_V();
        CbTextChange (nullptr, 0);
    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerMessages::AddControls (uint16_t ctrlTab)
{
    // DEBUG_START;

    ParentElementId = ctrlTab;

    if (Control::noParent == MessageElementIds.ActiveChoiceListElementId)
    {
        // DEBUG_V(String("Add Select Hidden"));
        MessageElementIds.HiddenChoiceListElementId = ESPUI.addControl (
                ControlType::Select,
                emptyString.c_str (),
                emptyString,
                ControlColor::Turquoise,
                ctrlTab);
        ESPUI.updateVisibility (MessageElementIds.HiddenChoiceListElementId, false);

        // DEBUG_V(String("Add Select Active messages choice list"));
        MessageElementIds.ActiveChoiceListElementId = ESPUI.addControl (
                ControlType::Select,
                Title.c_str (),
                emptyString,
                ControlColor::Turquoise,
                ctrlTab,
                [] (Control * sender, int type, void * parm)
                {
                    // Serial.println(String("ChoiceList parm: 0x") + String(uint32_t(parm), HEX));
                    if (parm)
                    {
                        reinterpret_cast <c_ControllerMessages *> (parm)->CbChoiceList (sender, type);
                    }
                },
                this);

        // DEBUG_V(String("Add Delete Button"));
        ButtonDeleteElementId = ESPUI.addControl (
                ControlType::Button,
                emptyString.c_str (),
                " Delete ",
                ControlColor::None,
                MessageElementIds.ActiveChoiceListElementId,
                [] (Control * sender, int type, void * parm)
                {
                    // Serial.println(String("Delete Button parm: 0x") + String(uint32_t(parm), HEX));
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_ControllerMessages *> (parm)->CbButtonDelete (sender, type);
                    }
                },
                this);

        // DEBUG_V(String("Add Update Button"));
        ButtonUpdateElementId = ESPUI.addControl (
                ControlType::Button,
                emptyString.c_str (),
                " Update ",
                ControlColor::None,
                MessageElementIds.ActiveChoiceListElementId,
                [] (Control * sender, int type, void * parm)
                {
                    // Serial.println(String("Update Button parm: 0x") + String(uint32_t(parm), HEX));
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_ControllerMessages *> (parm)->CbButtonUpdate (sender, type);
                    }
                },
                this);

        // DEBUG_V(String("Add Instruction lable"));
        InstructionElementId = ESPUI.addControl (
                ControlType::Label,
                DefaultTextFieldValue.c_str (),
                DefaultTextFieldValue,
                ControlColor::Turquoise,
                MessageElementIds.ActiveChoiceListElementId);
        ESPUI.setElementStyle (InstructionElementId, CSS_LABEL_STYLE_BLACK);

        // DEBUG_V(String("Add Text Entry"));
        TextEntryElementId = ESPUI.addControl (
                ControlType::Text,
                emptyString.c_str (),
                DefaultTextFieldValue,
                ControlColor::None,
                MessageElementIds.ActiveChoiceListElementId,
                [] (Control * sender, int type, void * parm)
                {
                    // Serial.println(String("Text Entry parm: 0x") + String(uint32_t(parm), HEX));
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_ControllerMessages *> (parm)->CbTextChange (sender, type);
                    }
                },
                this);

        // DEBUG_V(String("Add Text Size limit"));
        ESPUI.addControl (
            ControlType::Max,
            emptyString.c_str (),
            "64",
            ControlColor::None,
            TextEntryElementId);

        // DEBUG_V(String("Add Create Button"));
        ButtonCreateElementId = ESPUI.addControl (
                ControlType::Button,
                emptyString.c_str (),
                " Create ",
                ControlColor::None,
                MessageElementIds.ActiveChoiceListElementId,
                [] (Control * sender, int type, void * parm)
                {
                    // Serial.println(String("Create Button parm: 0x") + String(uint32_t(parm), HEX));
                    if (nullptr != parm)
                    {
                        reinterpret_cast <c_ControllerMessages *> (parm)->CbButtonCreate (sender, type);
                    }
                },
                this);

        // DEBUG_V(String("Add Label for Status"));
        StatusMsgElementId = ESPUI.addControl (
                ControlType::Label,
                emptyString.c_str (),
                emptyString,
                ControlColor::Turquoise,
                MessageElementIds.ActiveChoiceListElementId);
        ESPUI.setElementStyle (StatusMsgElementId, CSS_LABEL_STYLE_BLACK);

        if (ShowFseqNameSelection)
        {
            // DEBUG_V(String("Add Label 2"));
            DisplayFseqNameLabelElementId = ESPUI.addControl (
                    ControlType::Label,
                    emptyString.c_str (),
                    "Display FSEQ File Name",
                    ControlColor::Turquoise,
                    MessageElementIds.ActiveChoiceListElementId);
            ESPUI.setElementStyle (
                DisplayFseqNameLabelElementId,
                CSS_LABEL_STYLE_BLACK);

            // DEBUG_V(String("Add Display fseq name Switcher"));
            // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName));
            DisplayFseqNameElementId = ESPUI.addControl (
                    ControlType::Switcher,
                    emptyString.c_str (),
                    DisplayFseqName ? "1" : "0",
                    ControlColor::None,
                    MessageElementIds.ActiveChoiceListElementId,
                    [] (Control * sender, int type, void * parm)
                    {
                        if (nullptr != parm)
                        {
                            reinterpret_cast <c_ControllerMessages *> (parm)->CbSwitchDisplayFseqName (sender, type);
                        }
                    },
                    nullptr);
        }
        // DEBUG_V("Add msg details configuration pane");
        // DEBUG_V(String("Add Title"));
        MessageElementIds.MessageDetailsElementId = ESPUI.addControl (
                ControlType::Label,
                emptyString.c_str (),
                N_Enable,
                ControlColor::Turquoise,
                ctrlTab);
        ESPUI.setElementStyle (MessageElementIds.MessageDetailsElementId, CSS_LABEL_STYLE_BLACK);
        // DEBUG_V(String("MessageElementIds: 0x") + String(uint32_t(&MessageElementIds), HEX));
        // DEBUG_V(String("MessageDetailsElementId: 0x") + String(MessageElementIds.MessageDetailsElementId, HEX));

        // DEBUG_V(String("Add Enabled field"));
        MessageElementIds.EnabledElementId = ESPUI.addControl (
                ControlType::Switcher,
                emptyString.c_str (),
                "0",
                ControlColor::Turquoise,
                MessageElementIds.MessageDetailsElementId);

        // DEBUG_V(String("Add Seprator"));
        SeperatorMsgElementId = ESPUI.addControl (
                ControlType::Label,
                emptyString.c_str (),
                F ("RDS Display Duration (SECS)"),
                ControlColor::Turquoise,
                MessageElementIds.MessageDetailsElementId);
        ESPUI.setElementStyle (SeperatorMsgElementId, CSS_LABEL_STYLE_BLACK);

        // DEBUG_V(String("Add Number field"));
        MessageElementIds.DisplayDurationElementId = ESPUI.addControl (
                ControlType::Number,
                emptyString.c_str (),
                "0",
                ControlColor::Turquoise,
                MessageElementIds.MessageDetailsElementId);
        ESPUI.addControl (
            ControlType::Min,
            "Min",
            String (5),
            ControlColor::None,
            MessageElementIds.DisplayDurationElementId);
        ESPUI.addControl (
            ControlType::Max,
            "Max",
            String (900),
            ControlColor::None,
            MessageElementIds.DisplayDurationElementId);
    }
    else
    {
        // DEBUG_V(String("NOT Adding Controls"));
    }

    // DEBUG_V();
    for (auto &CurrentMessageSet : MessageSets)
    {
        // DEBUG_V();
        CurrentMessageSet.second.AddControls (&MessageElementIds);
    }

    if (MessageSets.empty ())
    {
        // DEBUG_V("Turn off msg details control");
        ESPUI.updateVisibility (MessageElementIds.MessageDetailsElementId, false);
    }
    else
    {
        // DEBUG_V("Select the default message set.");
        ActivateMessageSet (MessageSets.begin ()->first);
    }
    // DEBUG_V();
    CbTextChange (nullptr, 0);

    // DEBUG_END;
}       // AddControls

// ************************************************************************************************
void c_ControllerMessages::AddMessage (String MsgSetName, String MsgText)
{
    // DEBUG_START;
    // DEBUG_V(String("message set name: '") + MsgSetName + "'");
    // DEBUG_V(String("    message name: '") + MsgText + "'");

    do  // once
    {
        if (!AddMessageSet (MsgSetName))
        {
            // DEBUG_V(String("Cant use message set name: '") + MsgSetName + "'");
            break;
        }
        // DEBUG_V(String(" Add '" + MsgText + "' to message set: '") + MsgSetName + "'");
        xSemaphoreTake (MessageSetsSemaphore, portMAX_DELAY);
        MessageSets[MsgSetName].AddMessage (MsgText);
        xSemaphoreGive (MessageSetsSemaphore);

        MessageSets[MsgSetName].ActivateMessage (MsgText);

        if (Control::noParent == ParentElementId)
        {
            // DEBUG_V("Cannot set up the UI connections yet");
            break;
        }
        // DEBUG_V("Set up the buttons");
        CbTextChange (nullptr, 0);
    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
bool c_ControllerMessages::AddMessageSet (String MsgSetName)
{
    // DEBUG_START;

    bool  Response = true;

    // DEBUG_V(String("message set: '") + MsgSetName + "'");

    do  // once
    {
        if (MsgSetName.isEmpty ())
        {
            // DEBUG_V("Message set name is empty. Abort");
            Response = false;
            break;
        }

        if (MessageSets.end () != MessageSets.find (MsgSetName))
        {
            // DEBUG_V("Message set Already exists");
            break;
        }
        // DEBUG_V("Add new message set entry");
        c_ControllerMessageSet  temp;

        xSemaphoreTake (MessageSetsSemaphore, portMAX_DELAY);
        MessageSets[MsgSetName] = temp;
        xSemaphoreGive (MessageSetsSemaphore);

        MessageSets[MsgSetName].AddControls (&MessageElementIds);
        MessageSets[MsgSetName].SetName (MsgSetName);

        if (CurrentMsgSetName.isEmpty ())
        {
            // DEBUG_V("Make this the active message set");
            ActivateMessageSet (MsgSetName);
        }
    } while (false);

    // DEBUG_END;
    return Response;
}

// ************************************************************************************************
void c_ControllerMessages::CbButtonCreate (Control * sender, int type)
{
    // DEBUG_START;

    Control  * TextControl = ESPUI.getControl (TextEntryElementId);

    do  // once
    {
        if (type != B_DOWN)
        {
            // DEBUG_V("Ignore unwanted button action");
            break;
        }
        // DEBUG_V("Create a new message");
        AddMessage (CurrentMsgSetName, TextControl->value);

        displaySaveWarning ();

        // refresh the UI
        ESPUI.jsonDom (0);
    } while (false);

    // DEBUG_END;
}       // ButtonCreateCb

// ************************************************************************************************
void c_ControllerMessages::CbButtonDelete (Control * sender, int type)
{
    // DEBUG_START;
    // DEBUG_V(String("       Title: '") + Title + "'");

    Control     * ChoiceControl = ESPUI.getControl (MessageElementIds.ActiveChoiceListElementId);
    String      SelectedMsgName = ChoiceControl->value;

    // DEBUG_V(String("Choice value: '") + SelectedMsgName + "'");

    do  // once
    {
        if (type != B_DOWN)
        {
            // DEBUG_V("Ignore unwanted button action");
            break;
        }
        // DEBUG_V("Erase message from the set of messages");
        xSemaphoreTake (MessageSetsSemaphore, portMAX_DELAY);
        MessageSets[CurrentMsgSetName].EraseMsg (SelectedMsgName);
        xSemaphoreGive (MessageSetsSemaphore);

        // DEBUG_V();
        CbTextChange (nullptr, 0);
        // DEBUG_V();
        displaySaveWarning ();

        // refresh the UI
        ESPUI.jsonDom (0);
    } while (false);

    // DEBUG_END;
}       // ButtonDeleteCb

// ************************************************************************************************
void c_ControllerMessages::CbButtonUpdate (Control * sender, int type)
{
    // DEBUG_START;

    // DEBUG_V(String(" CurrentMsgSetName: '") + CurrentMsgSetName + "'");

    do  // once
    {
        if (type != B_DOWN)
        {
            // DEBUG_V("Ignore unwanted button actions");
            break;
        }
        Control         * TextControl   = ESPUI.getControl (TextEntryElementId);
        String          NewMessageText  = TextControl->value;
        // DEBUG_V(String("     NewMessageText: '") + NewMessageText + "'");

        Control         * ChoiceControl         = ESPUI.getControl (MessageElementIds.ActiveChoiceListElementId);
        String          OriginalMessageText     = ChoiceControl->value;
        // DEBUG_V(String("OriginalMessageText: '") + OriginalMessageText + "'");

        MessageSets[CurrentMsgSetName].UpdateMsgText (OriginalMessageText, NewMessageText);

        CbTextChange (nullptr, 0);
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}       // CbButtonUpdate

// ************************************************************************************************
void c_ControllerMessages::CbChoiceList (Control * sender, int type)
{
    // DEBUG_START;
    // DEBUG_V(String("       Title: '") + Title + "'");

    String  CurrentSeletedMessageName = sender->value;

    // DEBUG_V(String("Selected: '") + CurrentSeletedMessageName + "'");

    do  // once
    {
        if (CurrentSeletedMessageName.equals (emptyString))
        {
            // DEBUG_V("Selected message is empty. Abort");
            ESPUI.updateText (TextEntryElementId, DefaultTextFieldValue);
            break;
        }
        // DEBUG_V("Set the input text to equal the selected message.");
        ESPUI.updateText (TextEntryElementId, CurrentSeletedMessageName);

        // tell the message it has been selected
        MessageSets[CurrentMsgSetName].ActivateMessage (CurrentSeletedMessageName);

        // Update the warning and text fields
        CbTextChange (nullptr, 0);
    } while (false);

    // DEBUG_END;
}       // CbChoiceList

// ************************************************************************************************
void c_ControllerMessages::CbSwitchDisplayFseqName (Control * sender, int type)
{
    // DEBUG_START;

    DisplayFseqName = (S_ACTIVE == type);
    // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName))

    displaySaveWarning ();
    Log.infoln ((String (F ("FPPD Display fseq Name Set to: ")) + String (DisplayFseqName ? "On" : "Off")).c_str ());

    // DEBUG_END;
}       // DisplayFseqNameCb

// ************************************************************************************************
void c_ControllerMessages::CbTextChange (Control *, int)
{
    // DEBUG_START;
    // DEBUG_V(String(" CurrentMsgSetName: '") + CurrentMsgSetName + "'");

    Control     * ChoiceList    = ESPUI.getControl (MessageElementIds.ActiveChoiceListElementId);
    Control     * TextControl   = ESPUI.getControl (TextEntryElementId);

    // DEBUG_V(String("        Text value: '") + TextControl->value + "'");
    // DEBUG_V(String("      Choice value: '") + ChoiceList->value + "'");

    bool        EnableCreate    = true;
    bool        EnableDelete    = true;
    bool        EnableUpdate    = true;

    do  // once
    {
        if (ChoiceList->value.isEmpty () || MessageSets[CurrentMsgSetName].empty ())
        {
            // DEBUG_V("Disable delete/update buttons");
            EnableDelete        = false;
            EnableUpdate        = false;
        }

        if (TextControl->value.equals (DefaultTextFieldValue))
        {
            // DEBUG_V("User did not remove the default text");
            EnableCreate        = false;
            EnableUpdate        = false;
            ESPUI.print (StatusMsgElementId, emptyString);
            break;
        }

        if (-1 != TextControl->value.indexOf (DefaultTextFieldValue))
        {
            // DEBUG_V("User did not remove the default text");
            EnableCreate        = false;
            EnableUpdate        = false;
            ESPUI.print (StatusMsgElementId, DefaultTextWarningMsg);
            break;
        }

        if (TextControl->value.isEmpty ())
        {
            // DEBUG_V("Cant update an entry without a message");
            EnableCreate        = false;
            EnableUpdate        = false;
            ESPUI.updateText (TextEntryElementId, DefaultTextFieldValue);
            ESPUI.print (StatusMsgElementId, EmptyMsgWarning);
            break;
        }

        if (TextControl->value.equals (ChoiceList->value))
        {
            // DEBUG_V("No Change In Text");
            EnableCreate        = false;
            EnableUpdate        = false;
            // ESPUI.updateControlValue(TextEntryElementId, DefaultTextFieldValue);
            break;
        }

        if (MessageSets[CurrentMsgSetName].HasMsg (TextControl->value))
        {
            // DEBUG_V("Msg exists in active set. No Create or update allowed.");
            EnableCreate        = false;
            EnableUpdate        = false;
            ESPUI.print (StatusMsgElementId, String (F ("This message already exists.")));
            break;
        }
        // DEBUG_V("valid text that could be used for the existing Message");
        ESPUI.print (StatusMsgElementId, emptyString);
    } while (false);

    // DEBUG_V("Update Buttons");
    ESPUI.      setEnabled (    ButtonCreateElementId,  EnableCreate);
    ESPUI.      setEnabled (    ButtonDeleteElementId,  EnableDelete);
    ESPUI.      setEnabled (    ButtonUpdateElementId,  EnableUpdate);

    // DEBUG_END;
}       // TextChangeCb

// ************************************************************************************************
void c_ControllerMessages::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t &Response)
{
    // DEBUG_START;

    do  // once
    {
        if (0 == MessageSets.size ())
        {
            // DEBUG_V("No messages to send");
            break;
        }

        if (MessageSets.end () == MessageSets.find (CurrentMsgSetName))
        {
            // no such message set
            break;
        }
        xSemaphoreTake (MessageSetsSemaphore, portMAX_DELAY);
        MessageSets[CurrentMsgSetName].GetNextRdsMessage (Response);
        xSemaphoreGive (MessageSetsSemaphore);
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerMessages::RestoreConfig (ArduinoJson::JsonObject &config)
{
    // DEBUG_START;

    // serializeJsonPretty(config, Serial);

    if (ShowFseqNameSelection)
    {
        if (config.containsKey (N_DisplayFseqName))
        {
            DisplayFseqName = config[N_DisplayFseqName];
            // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName));
        }
    }
    // DEBUG_V();

    if (!config.containsKey (N_messages))
    {
        // DEBUG_V("Create missing Message Set array");
        config.createNestedArray (N_messages);
    }
    // DEBUG_V();
    JsonArray  ListOfMessageSetConfigs = config[N_messages];

    for (auto CurrentMessageSetConfig : ListOfMessageSetConfigs)
    {
        if (!CurrentMessageSetConfig.containsKey (N_name))
        {
            // DEBUG_V("Missing Message Set Name");
            continue;
        }
        String  MessageSetName = (const char *)CurrentMessageSetConfig[N_name];
        // DEBUG_V(String("MessageSetName: '") + MessageSetName + "'");

        // DEBUG_V("create the message set");
        AddMessageSet (MessageSetName);

        // DEBUG_V("Send the message set the config");
        JsonObject  Temp = CurrentMessageSetConfig;
        MessageSets[MessageSetName].RestoreConfig (Temp);
    }

    // DEBUG_END;
}       // RestoreConfig

// *********************************************************************************************
void c_ControllerMessages::SaveConfig (ArduinoJson::JsonObject &config)
{
    // DEBUG_START;

    if (ShowFseqNameSelection)
    {
        config[N_DisplayFseqName] = DisplayFseqName;
        // DEBUG_V(String("DisplayFseqName: ") + String(DisplayFseqName));
    }
    // DEBUG_V("Create List of Msg Sets");
    JsonArray  MessageSetArray = config.createNestedArray (N_messages);

    // DEBUG_V();

    for (auto &CurrentMessageSet : MessageSets)
    {
        JsonObject  MessageSetConfig = MessageSetArray.createNestedObject ();
        MessageSets[CurrentMessageSet.first].SaveConfig (MessageSetConfig);
    }

    // DEBUG_V("Final");
    // serializeJsonPretty(config, Serial);

    // DEBUG_END;
}       // SaveConfig

// *********************************************************************************************
void c_ControllerMessages::SaveConfig (ArduinoJson::JsonObject &config, String &SetName)
{
    // DEBUG_START;

    MessageSets[SetName].SaveConfig (config);

    // DEBUG_END;
}       // SaveConfig

// *********************************************************************************************
void c_ControllerMessages::SetShowFseqNameSelection (bool value)
{
    // DEBUG_START;

    ShowFseqNameSelection = value;

    // DEBUG_END;
}

// *********************************************************************************************
// EOF
