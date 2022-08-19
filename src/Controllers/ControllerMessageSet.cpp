/*
   File: ControllerMessageSet.cpp
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
#include "ControllerMessageSet.h"
#include "Language.h"

#if __has_include ("memdebug.h")
 # include "memdebug.h"
#endif //  __has_include("memdebug.h")

// *********************************************************************************************
c_ControllerMessageSet::c_ControllerMessageSet ()
{
    // DEBUG_START;

    MessagesSemaphore = xSemaphoreCreateMutex ();

    // DEBUG_END;
}       // c_ControllerMessageSet

// *********************************************************************************************
c_ControllerMessageSet::~c_ControllerMessageSet ()
{
    // DEBUG_START;

    // DEBUG_END;
}       // c_ControllerMessageSet

// *********************************************************************************************
void c_ControllerMessageSet::Activate (bool Activating)
{
    // DEBUG_START;

    for (auto & CurrentMessage : Messages)
    {
        CurrentMessage.second.Activate (Activating);
    }

    if (Activating)
    {
        if (Messages.empty ())
        {
            // DEBUG_V("No messages. Do not show message details pane");
            ShowMsgDetailsPane (false);
        }
        else
        {
            // DEBUG_V("Activate first message");
            ActivateMessage (Messages.begin ()->first);
            ShowMsgDetailsPane (true);
        }
    }
    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerMessageSet::ActivateMessage (String MsgName)
{
    // DEBUG_START;
    // DEBUG_V(String("  MsgSetName: '") + MsgSetName + "'");
    // DEBUG_V(String("     MsgName: '") + MsgName + "'");

    do  // once
    {
        if (Messages.end () == Messages.find (MsgName))
        {
            // DEBUG_V("Desired message not found. Add it.");
            AddMessage (MsgName);
        }
        // DEBUG_V(String("Update the choice list and populate message details"));
        Messages[MsgName].SelectMessage ();

        // DEBUG_V(String("Show the message details pane"));
        CurrentMsgName = MsgName;
        ShowMsgDetailsPane (true);

        CurrentMsgName = MsgName;
    } while (false);

    // DEBUG_END;
}       // ActivateMessage

// ************************************************************************************************
void c_ControllerMessageSet::AddControls (c_ControllerMessage::MessageElementIds_t * _MessageElementIds)
{
    // DEBUG_START;

    MessageElementIds = _MessageElementIds;

    do  // once
    {
        if (nullptr == MessageElementIds)
        {
            // DEBUG_V("No control structure defined yet");
            break;
        }

        // DEBUG_V();
        for (auto & CurrentMessage : Messages)
        {
            CurrentMessage.second.AddControls (MessageElementIds);
        }
        // DEBUG_V();

        if (Messages.empty ())
        {
            // DEBUG_V("Turn off msg details control");
            ShowMsgDetailsPane (false);
            break;
        }
        // DEBUG_V("Select the default message set.");
        Messages.begin ()->second.Activate (true);
        // DEBUG_V("Turn on msg details control");
        CurrentMsgName = Messages.begin ()->first;
        ShowMsgDetailsPane (true);
    } while (false);

    // DEBUG_END;
}       // AddControls

// ************************************************************************************************
void c_ControllerMessageSet::AddMessage (String MsgText)
{
    // DEBUG_START;
    // DEBUG_V(String("     message set: '") + MsgSetName + "'");
    // DEBUG_V(String("    message name: '") + MsgText + "'");

    do  // once
    {
        if (MsgText.isEmpty ())
        {
            // DEBUG_V("Cant add an empty message");
            break;
        }

        if (Messages.end () != Messages.find (MsgText))
        {
            // DEBUG_V("Cant add a duplicate message.");
            break;
        }
        // DEBUG_V("Create the message");
        xSemaphoreTake (MessagesSemaphore, portMAX_DELAY);
        Messages[MsgText].SetMessage (MsgText);
        MessagesIterator = nullMessagesIterator;
        xSemaphoreGive (MessagesSemaphore);

        CurrentMsgName = MsgText;

        if (Control::noParent == MessageElementIds->ActiveChoiceListElementId)
        {
            // DEBUG_V("Defer setting up UI");
            break;
        }
        // DEBUG_V("Set up the UI connections");
        Messages[MsgText].AddControls (MessageElementIds);
        // DEBUG_V("Make the new message the selected message");
        Messages[MsgText].SelectMessage ();
    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerMessageSet::EraseMsg (String MsgTxt)
{
    // DEBUG_START;

    // DEBUG_V(String("MsgTxt: '") + MsgTxt + "'");

    do  // once
    {
        if (Messages.end () == Messages.find (MsgTxt))
        {
            // DEBUG_V("Message not found");
            break;
        }
        xSemaphoreTake (MessagesSemaphore, portMAX_DELAY);
        Messages.erase (MsgTxt);
        MessagesIterator = nullMessagesIterator;
        xSemaphoreGive (MessagesSemaphore);

        if (!MsgTxt.equals (CurrentMsgName))
        {
            // DEBUG_V("Deleted message is not the current active message");
            break;
        }

        if (Messages.empty ())
        {
            // DEBUG_V("No messages left to display");
            ShowMsgDetailsPane (false);
            break;
        }
        // DEBUG_V("Display the first message in the list");
        Messages.begin ()->second.Activate (true);
        CurrentMsgName = Messages.begin ()->first;
        ShowMsgDetailsPane (true);
    } while (false);

    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerMessageSet::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response)
{
    // DEBUG_START;

    do  // once
    {
        if (0 == Messages.size ())
        {
            // DEBUG_V("No messages to send");
            break;
        }
        xSemaphoreTake (MessagesSemaphore, portMAX_DELAY);

        for (uint32_t count = Messages.size (); 0 < count; --count)
        {
            if (nullMessagesIterator == MessagesIterator)
            {
                MessagesIterator = Messages.begin ();
            }
            else
            {
                // Advance to the next message
                ++MessagesIterator;

                if (MessagesIterator == Messages.end ())
                {
                    MessagesIterator = Messages.begin ();
                }
            }

            if (!MessagesIterator->second.IsEnabled ())
            {
                continue;
            }
            // DEBUG_V("Found a message");
            MessagesIterator->second.GetMessage (Response);
            // DEBUG_V();
            break;
        }

        xSemaphoreGive (MessagesSemaphore);
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerMessageSet::RestoreConfig (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    // DEBUG_V(String("MsgSetName: ") + MsgSetName);

    if (!config.containsKey (N_list))
    {
        // DEBUG_V("Create missing Message array");
        config.createNestedArray (N_list);
    }
    // DEBUG_V();
    JsonArray ListOfMessages = config[N_list];

    // DEBUG_V("add each message to the current message set");
    for (auto CurrentMessageConfig : ListOfMessages)
    {
        String MessageName;

        if (!CurrentMessageConfig.containsKey (N_message))
        {
            // DEBUG_V("Cannot process message config entry without a name");
            continue;
        }
        MessageName = (const char *)CurrentMessageConfig[N_message];

        if (MessageName.isEmpty ())
        {
            // DEBUG_V("Cannot process message config entry with an empty name");
            continue;
        }

        if (Messages.end () != Messages.find (MessageName))
        {
            // DEBUG_V(String("Cannot add a duplicate entry: '") + MessageName + "'");
            continue;
        }
        // DEBUG_V(String("Add message to the message set: '") + MessageName + "'");
        AddMessage (MessageName);
        Messages[MessageName].RestoreConfig (CurrentMessageConfig);
    }

    // DEBUG_END;
}       // RestoreConfig

// *********************************************************************************************
void c_ControllerMessageSet::SaveConfig (ArduinoJson::JsonObject & MsgSetConfig)
{
    // DEBUG_START;

    MsgSetConfig[N_name] = MsgSetName;

    // DEBUG_V("Create List");
    JsonArray MessageArray = MsgSetConfig.createNestedArray (N_list);

    // DEBUG_V();
    for (auto & currentMessage : Messages)
    {
        JsonObject MessageConfig = MessageArray.createNestedObject ();
        currentMessage.second.SaveConfig (MessageConfig);
    }

    // DEBUG_END;
}       // SaveConfig

// *********************************************************************************************
void c_ControllerMessageSet::ShowMsgDetailsPane (bool value)
{
    // DEBUG_START;

    // DEBUG_V(String("CurrentMsgName: '") + CurrentMsgName + "'");
    // DEBUG_V(String("         value: '") + String(value) + "'");

    do  // once
    {
        // DEBUG_V(String("      MessageElementIds: 0x") + String(uint32_t(MessageElementIds), HEX));

        // have the controls been added yet?
        if (nullptr == MessageElementIds)
        {
            // DEBUG_V("Controls not added yet");
            break;
        }
        // DEBUG_V(String("MessageDetailsElementId: 0x") + String(MessageElementIds->MessageDetailsElementId, HEX));

        Control * control = ESPUI.getControl (MessageElementIds->MessageDetailsElementId);

        if (control)
        {
            control->visible    = value;
            control->label      = CurrentMsgName.c_str ();
            ESPUI.updateControl (control);
        }
        else
        {
            // DEBUG_V("ERROR: No such control.");
        }
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerMessageSet::UpdateMsgText (String & OriginalMessageText, String & NewMessageText)
{
    // DEBUG_START;

    do  // once
    {
        if (NewMessageText.isEmpty ())
        {
            // DEBUG_V("New message is empty");
            break;
        }

        if (Messages.end () == Messages.find (OriginalMessageText))
        {
            // DEBUG_V("Cant find old text in the map");
            break;
        }
        AddMessage (NewMessageText);
        // DEBUG_V("Copy settings");
        Messages[NewMessageText] = Messages[OriginalMessageText];
        Messages[NewMessageText].AddControls (MessageElementIds);

        // DEBUG_V("Delete the original");
        Messages.erase (OriginalMessageText);

        if (OriginalMessageText.equals (CurrentMsgName))
        {
            // DEBUG_V("We are the current selected message");
            CurrentMsgName = NewMessageText;
            Messages[NewMessageText].Activate (true);
        }
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
// EOF
