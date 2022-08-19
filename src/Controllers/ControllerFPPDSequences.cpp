/*
   File: ControllerFPPDSequences.cpp
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
#include "ControllerFPPDSequences.h"
#include <map>

#if __has_include ("memdebug.h")
 # include "memdebug.h"
#endif //  __has_include("memdebug.h")

static const String     Name                    = F ("FPPD Sequences");
static const String     DefaultTextFieldValue   = F ("Type New Sequence Name Here");

// *********************************************************************************************
c_ControllerFPPDSequences::c_ControllerFPPDSequences ()
{
    // DEBUG_START;
    // DEBUG_END;
}       // c_ControllerFPPDSequences

// *********************************************************************************************
c_ControllerFPPDSequences::~c_ControllerFPPDSequences ()
{}

// ************************************************************************************************
void c_ControllerFPPDSequences::Activate ()
{
    // DEBUG_START;

    // DEBUG_V(String("Activate SelectedSequenceName ") + SelectedSequenceName);

    for (auto & CurrentSequence : Sequences)
    {
        // DEBUG_V(String("Activate Sequence: ") + CurrentSequence.first);
        CurrentSequence.second.Activate (CurrentSequence.first.equals (SelectedSequenceName));
    }

    // DEBUG_END;
}       // Activate

// ************************************************************************************************
void c_ControllerFPPDSequences::AddControls (uint16_t ctrlTab)
{
    // DEBUG_START;

    EspuiParentElementId = ctrlTab;

    if (Control::noParent == EspuiChoiceListElementId)
    {
        // DEBUG_V("Adding Select");
        EspuiChoiceListElementId = ESPUI.addControl (ControlType::Select,
                                                     "FPPD Sequences",
                                                     emptyString,
                                                     ControlColor::Turquoise,
                                                     EspuiParentElementId,
                                                     [] (Control * sender, int type, void * param)
                                                     {
                                                         if (param)
                                                         {
                                                             reinterpret_cast <c_ControllerFPPDSequences *> (param)->CbChoiceList (sender,
                                                                                                                                   type);
                                                         }
                                                     },
                                                     this);

        // DEBUG_V("Adding Delete button");
        EspuiButtonDeleteElementId = ESPUI.addControl (ControlType::Button,
                                                       emptyString.c_str (),
                                                       " Delete ",
                                                       ControlColor::None,
                                                       EspuiChoiceListElementId,
                                                       [] (Control * sender, int type, void * param)
                                                       {
                                                           if (param)
                                                           {
                                                               reinterpret_cast <c_ControllerFPPDSequences *> (param)->CbButtonDelete (sender,
                                                                                                                                       type);
                                                           }
                                                       },
                                                       this);

        // DEBUG_V("Adding Update button");
        EspuiButtonUpdateElementId = ESPUI.addControl (ControlType::Button,
                                                       emptyString.c_str (),
                                                       " Update ",
                                                       ControlColor::None,
                                                       EspuiChoiceListElementId,
                                                       [] (Control * sender, int type, void * param)
                                                       {
                                                           if (param)
                                                           {
                                                               reinterpret_cast <c_ControllerFPPDSequences *> (param)->CbButtonUpdate (sender,
                                                                                                                                       type);
                                                           }
                                                       },
                                                       this);

        // DEBUG_V("Adding Instruction Msg");
        EspuiInstructionMsgElementId = ESPUI.addControl (ControlType::Label,
                                                         DefaultTextFieldValue.c_str (),
                                                         DefaultTextFieldValue,
                                                         ControlColor::Turquoise,
                                                         EspuiChoiceListElementId);
        ESPUI.setElementStyle (EspuiInstructionMsgElementId, CSS_LABEL_STYLE_BLACK);

        // DEBUG_V("Adding Text Entry");
        EspuiTextEntryElementId = ESPUI.addControl (ControlType::Text,
                                                    emptyString.c_str (),
                                                    DefaultTextFieldValue,
                                                    ControlColor::None,
                                                    EspuiChoiceListElementId,
                                                    [] (Control * sender, int type, void * param)
                                                    {
                                                        if (param)
                                                        {
                                                            reinterpret_cast <c_ControllerFPPDSequences *> (param)->CbTextChange (sender,
                                                                                                                                  type);
                                                        }
                                                    },
                                                    this);

        // DEBUG_V("Adding Max");
        ESPUI.addControl (ControlType::Max, emptyString.c_str (), "64", ControlColor::None, EspuiTextEntryElementId);

        // DEBUG_V("Adding Create");
        EspuiButtonCreateElementId = ESPUI.addControl (ControlType::Button,
                                                       emptyString.c_str (),
                                                       " Create ",
                                                       ControlColor::None,
                                                       EspuiChoiceListElementId,
                                                       [] (Control * sender, int type, void * param)
                                                       {
                                                           if (param)
                                                           {
                                                               reinterpret_cast <c_ControllerFPPDSequences *> (param)->CbButtonCreate (sender,
                                                                                                                                       type);
                                                           }
                                                       },
                                                       this);

        // DEBUG_V("Adding Status Msg");
        EspuiStatusMsgElementId = ESPUI.addControl (ControlType::Label,
                                                    emptyString.c_str (),
                                                    emptyString,
                                                    ControlColor::Turquoise,
                                                    EspuiChoiceListElementId);
        ESPUI.setElementStyle (EspuiStatusMsgElementId, CSS_LABEL_STYLE_BLACK);
    }
    else
    {
        // DEBUG_V("NOT Adding Controls");
    }
    // DEBUG_V(String("Sequences.siz: ") + String(Sequences.size()));

    ControllerMessages.AddControls (EspuiParentElementId);

    for (auto & CurrentSequence : Sequences)
    {
        // DEBUG_V(String("Adding Controls for ") + CurrentSequence.first);
        CurrentSequence.second.AddControls (EspuiParentElementId, EspuiChoiceListElementId);
    }

    // DEBUG_V(String("Activate ") + SelectedSequenceName);
    ESPUI.updateSelect (EspuiChoiceListElementId, SelectedSequenceName);
    Activate ();

    CbTextChange (nullptr, 0);

    // DEBUG_END;
}       // AddControls

// ************************************************************************************************
void c_ControllerFPPDSequences::AddSequence (String & SequenceName)
{
    // DEBUG_START;

    SelectedSequenceName = SequenceName;

    Sequences[SequenceName].SetMessages (&ControllerMessages);  // must be first
    Sequences[SequenceName].SetName (SequenceName);

    if (Control::noParent != EspuiParentElementId)
    {
        Sequences[SequenceName].AddControls (EspuiParentElementId, EspuiChoiceListElementId);
        Sequences[SequenceName].Activate (true);

        CbTextChange (nullptr, 0);
    }
    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerFPPDSequences::begin ()
{
    // DEBUG_START;

    String SequenceName = N_default;

    AddSequence (SequenceName);

    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerFPPDSequences::CbButtonCreate (Control * sender, int type)
{
    // DEBUG_START;

    do  // once
    {
        if (type != B_DOWN)
        {
            // DEBUG_V("Ignore unwanted button action");
            break;
        }
        Control * TextControl = ESPUI.getControl (EspuiTextEntryElementId);

        if (!TextControl)
        {
            // DEBUG_V("Could not locate Text Entry control");
            break;
        }
        SelectedSequenceName = TextControl->value;
        // DEBUG_V(String("value: '") + SelectedSequenceName + "'");

        // DEBUG_V(String("Set the Select List to ") + SelectedSequenceName);
        ESPUI.updateSelect (EspuiChoiceListElementId, SelectedSequenceName);

        // DEBUG_V("Create a Sequence");
        AddSequence (SelectedSequenceName);

        // DEBUG_V("Create a Sequence - Done");
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}       // ButtonCreateCb

// ************************************************************************************************
void c_ControllerFPPDSequences::CbButtonDelete (Control * sender, int type)
{
    // DEBUG_START;

    Control     * ChoiceControl = ESPUI.getControl (EspuiChoiceListElementId);
    String      Key             = ChoiceControl->value;

    // DEBUG_V(String("Choice value: '") + Key + "'");

    do  // once
    {
        if (type != B_DOWN)
        {
            // DEBUG_V("Ignore unwanted button action");
            break;
        }

        if (Sequences.end () == Sequences.find (ChoiceControl->value))
        {
            // DEBUG_V("Name does not exist");
            // DEBUG_V(String("  Key: '") + ChoiceControl->value + "'");
            break;
        }
        // DEBUG_V("now delete it");
        ESPUI.  updateControlValue (    EspuiStatusMsgElementId,        emptyString);
        ESPUI.  updateControlValue (    EspuiTextEntryElementId,        DefaultTextFieldValue);
        Activate ();

        Sequences[Key].Activate (false);
        Sequences.erase (Key);

        // DEBUG_V(String("Set the Select List to ") + N_default);
        ESPUI.updateSelect (EspuiChoiceListElementId, N_default);

        // DEBUG_V("Activate");
        Activate ();

        CbTextChange (nullptr, 0);
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}       // ButtonDeleteCb

// ************************************************************************************************
void c_ControllerFPPDSequences::CbButtonUpdate (Control * sender, int type)
{
    // DEBUG_START;

    Control     * TextControl   = ESPUI.getControl (EspuiTextEntryElementId);
    String      NewSequenceName = TextControl->value;
    // DEBUG_V(String("Text value: '") + NewSequenceName +"'");

    Control     * ChoiceControl         = ESPUI.getControl (EspuiChoiceListElementId);
    String      OriginalSequenceName    = ChoiceControl->value;

    // DEBUG_V(String("Choice value: '") + OriginalSequenceName + "'");

    do  // once
    {
        if (type != B_DOWN)
        {
            // DEBUG_V("Ignore unwanted button action");
            break;
        }
        // DEBUG_V("Copy the old sequence into a new map entry");
        Sequences[NewSequenceName] = Sequences[OriginalSequenceName];

        // DEBUG_V("Update the name in the copy");
        Sequences[OriginalSequenceName].SetName (NewSequenceName);

        // DEBUG_V("Activate the copy");
        Sequences[NewSequenceName].Activate (true);

        // DEBUG_V("Select the copy");
        ESPUI.updateSelect (EspuiChoiceListElementId, NewSequenceName);

        CbTextChange (nullptr, 0);
        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
}       // ButtonUpdateCb

// ************************************************************************************************
void c_ControllerFPPDSequences::CbChoiceList (Control * sender, int type)
{
    // DEBUG_START;

    SelectedSequenceName = sender->value;
    // DEBUG_V(String("Selected: '") + SelectedSequenceName + "'");

    if (SelectedSequenceName.equals (N_default))
    {
        // DEBUG_V("Selected Default message set");
        ESPUI.updateControlValue (EspuiTextEntryElementId, DefaultTextFieldValue);
    }
    else
    {
        // DEBUG_V("Selected Sequence Specific message set");
        ESPUI.updateControlValue (EspuiTextEntryElementId, SelectedSequenceName);
    }
    // DEBUG_V("Activate");
    Activate ();

    CbTextChange (nullptr, 0);

    ESPUI.jsonDom (0);

    // DEBUG_END;
}       // ChoiceListCb

// ************************************************************************************************
void c_ControllerFPPDSequences::CbTextChange (Control *, int)
{
    // DEBUG_START;

    Control * ChoiceList        = ESPUI.getControl (EspuiChoiceListElementId);
    Control * TextControl       = ESPUI.getControl (EspuiTextEntryElementId);

    // DEBUG_V(String("Choice value: ") + ChoiceList->value);
    // DEBUG_V(String("  Text value: ") + TextControl->value);

    bool        DeleteAllowed   = true;
    bool        UpdateAllowed   = true;
    bool        CreateAllowed   = true;

    do  // once
    {
        if (ChoiceList->value.equals (N_default))
        {
            // DEBUG_V("Cant do anything to the default entry")
            DeleteAllowed       = false;
            UpdateAllowed       = false;
            ESPUI.updateControlValue (EspuiStatusMsgElementId, DefaultTextFieldValue);
        }

        if (-1 != TextControl->value.indexOf (DefaultTextFieldValue))
        {
            // DEBUG_V("User did not remove the default text");
            CreateAllowed       = false;
            UpdateAllowed       = false;
            ESPUI.updateControlValue (EspuiStatusMsgElementId, emptyString);
            break;
        }

        if (TextControl->value.equals (emptyString))
        {
            // DEBUG_V("Cant use an entry without a name");
            CreateAllowed       = false;
            UpdateAllowed       = false;
            ESPUI.updateControlValue (EspuiStatusMsgElementId, String (F ("A Blank Name Is Not Allowed")));
            break;
        }

        if (TextControl->value.equals (N_default))
        {
            // DEBUG_V("Cant use the default entry text");
            CreateAllowed       = false;
            UpdateAllowed       = false;
            ESPUI.updateControlValue (EspuiStatusMsgElementId, String (F ("Cannot Create Another Default Sequence")));
            break;
        }

        if (TextControl->value.equals (ChoiceList->value))
        {
            // DEBUG_V("No Change in text");
            CreateAllowed       = false;
            UpdateAllowed       = false;
            ESPUI.updateControlValue (EspuiStatusMsgElementId, emptyString);
            break;
        }

        if (Sequences.end () != Sequences.find (TextControl->value))
        {
            // DEBUG_V("Name exists and it is not the selected name");
            // DEBUG_V(String("  Key: '") + Sequences.find(TextControl->value)->first + "'");

            CreateAllowed       = false;
            UpdateAllowed       = false;
            // ESPUI.updateControlValue(EspuiStatusMsgElementId, String(F("A Sequence With This Name Already Exists")));
            break;
        }
        // DEBUG_V("valid text that could be used for the existing sequence");
        ESPUI.updateControlValue (EspuiStatusMsgElementId, emptyString);
    } while (false);

    // DEBUG_V(String("CreateAllowed: ") + String(CreateAllowed));
    ESPUI.      setEnabled (    EspuiButtonCreateElementId,     CreateAllowed);
    // DEBUG_V(String("DeleteAllowed: ") + String(DeleteAllowed));
    ESPUI.      setEnabled (    EspuiButtonDeleteElementId,     DeleteAllowed);
    // DEBUG_V(String("UpdateAllowed: ") + String(UpdateAllowed));
    ESPUI.      setEnabled (    EspuiButtonUpdateElementId,     UpdateAllowed);

    // DEBUG_END;
}       // TextChangeCb

// *********************************************************************************************
void c_ControllerFPPDSequences::RestoreConfig (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    // Make sure the default sequnce exists
    String SequenceName = N_default;

    AddSequence (SequenceName);

    // serializeJsonPretty(config, Serial);
    // DEBUG_V();

    if (!config.containsKey (N_sequences))
    {
        config.createNestedArray (N_sequences);
    }
    JsonArray SequencesArray = config[N_sequences];

    for (auto CurrentSequenceConfig : SequencesArray)
    {
        String Key;

        if (false == CurrentSequenceConfig.containsKey (N_name))
        {
            // DEBUG_V("Missing name. Cant process record");
            continue;
        }
        Key = (const char *)CurrentSequenceConfig[N_name];
        // DEBUG_V(String("Setting up sequence: ") + Key);
        AddSequence (Key);

        // DEBUG_V();
        JsonObject Temp = CurrentSequenceConfig;
        Sequences[Key].RestoreConfig (Temp);
    }

    ControllerMessages.RestoreConfig (config);

    // DEBUG_END;
}       // RestoreControllerConfiguration

// *********************************************************************************************
void c_ControllerFPPDSequences::SaveConfig (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    if (false == config.containsKey (N_sequences))
    {
        // DEBUG_V("Create List");
        config.createNestedArray (N_sequences);
    }
    JsonArray SequenceConfigs = config[N_sequences];

    // DEBUG_V();

    for (auto & CurrentSequence : Sequences)
    {
        // DEBUG_V(String("Create Sequence entry") + CurrentSequence.first);
        JsonObject SequenceConfig = SequenceConfigs.createNestedObject ();
        CurrentSequence.second.SaveConfig (SequenceConfig);
    }

    ControllerMessages.SaveConfig (config);

    // serializeJsonPretty(config, Serial);

    // DEBUG_END;
}       // SaveControllerConfiguration

// *********************************************************************************************
// EOF
