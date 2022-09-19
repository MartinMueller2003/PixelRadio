/*
  *    File: ChoiceListControl.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include "ChoiceListControl.hpp"
#include "memdebug.h"

// *********************************************************************************************
cChoiceListControl::cChoiceListControl (const String                & ConfigName,
                                        const String                & Title,
                                        const String                & DefaultValue,
                                        const ChoiceListVector_t    * _ChoiceVector) :
    ChoiceVector (_ChoiceVector),
    cControlCommon (ConfigName, ControlType::Select, Title, DefaultValue, 64)
{
    // _ DEBUG_START;

    setControlStyle (CssStyleBlack_bw);

    String Dummy;
    KeyToChoiceVectorMap.clear ();

    // _ DEBUG_END;
}

// *********************************************************************************************
void cChoiceListControl::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;
    // DEBUG_V(String(": ") + GetTitle());

    cControlCommon::AddControls (value, color, true);
    setControlStyle (CssStyleBlack_bw);

    // DEBUG_V();
    RefreshOptionList (ChoiceVector);

    // DEBUG_END;
}

// *********************************************************************************************
uint32_t cChoiceListControl::get32 ()
{
    // DEBUG_START;

    uint32_t Response = uint32_t(-1);
    // DEBUG_V (String ("get: '") + DataValueStr + "'");

    if (KeyToChoiceVectorMap.end () != KeyToChoiceVectorMap.find (DataValueStr))
    {
        Response = (uint32_t)((*ChoiceVector)[getIndex ()].second.toInt ());
    }

    // DEBUG_V (String ("Response: ") + String (Response));
    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
uint32_t cChoiceListControl::getIndex ()
{
    // DEBUG_START;

    uint32_t Response = uint32_t (-1);

    if (KeyToChoiceVectorMap.end () == KeyToChoiceVectorMap.find (DataValueStr))
    {
        // DEBUG_V (String ("Could not find '") + DataValueStr + "' in the map");
    }
    else
    {
        Response = KeyToChoiceVectorMap[DataValueStr].VectorIndex;
    }

    // DEBUG_V (String ("Response: ") + String (Response));
    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
void cChoiceListControl::RefreshOptionList (const ChoiceListVector_t * OptionList)
{
    // DEBUG_START;

    // DEBUG_V ( String ("                    Title: ") + GetTitle());
    // DEBUG_V ( String ("KeyToChoiceVectorMap.size: ") + String (KeyToChoiceVectorMap.size ()));
    // DEBUG_V ( String ("         OptionList->size: ") + String (OptionList->size ()));
    // DEBUG_V ( String ("                ControlId: ") + String (ControlId));

    do  // once
    {
        // DEBUG_V ( String ("   OldDataValueStr: '") + DataValueStr + "'");
        uint32_t CurrentIndex = getIndex ();
        // DEBUG_V ( String ("      CurrentIndex: ") + String (CurrentIndex));
        ChoiceVector = OptionList;
        // DEBUG_V ( String ("ChoiceVector->size: ") + String (ChoiceVector->size ()));

        for (auto & CurrentOption : KeyToChoiceVectorMap)
        {
            // DEBUG_V (String ("Delete: ") + CurrentOption.first);

            if (Control::noParent != CurrentOption.second.UiId)
            {
                ESPUI.removeControl (CurrentOption.second.UiId);
                CurrentOption.second.UiId = Control::noParent;
            }
        }

        // DEBUG_V ("Delete all of the entries");
        KeyToChoiceVectorMap.clear ();

        // DEBUG_V ("Rebuild the map");
        uint32_t Index = 0;

        for (auto & CurrentOption : *ChoiceVector)
        {
            // DEBUG_V (String ("         first: ") + CurrentOption.first);
            ChoiceListEntry NewEntry;
            NewEntry.VectorIndex    = Index++;
            NewEntry.UiId           = ESPUI.addControl (ControlType::Option,
                emptyString.c_str (),
                emptyString,
                ControlColor::None,
                ControlId);
            // DEBUG_V (String ("   NewEntry.UiId: ") + String (NewEntry.UiId));

            ESPUI.updateControlLabel (NewEntry.UiId, CurrentOption.first.c_str ());
            ESPUI.updateControlValue (NewEntry.UiId, CurrentOption.first);

            KeyToChoiceVectorMap[CurrentOption.first] = NewEntry;
        }

        // DEBUG_V ( String ("KeyToChoiceVectorMap.size: ") + KeyToChoiceVectorMap.size ());

        String Dummy;
        // DEBUG_V();
        setIndex (CurrentIndex, Dummy, true);

        // DEBUG_V (String ("NewDataValueStr: ") + DataValueStr);

        ESPUI.updateControlValue (ControlId, DataValueStr);
    } while (false);

    // DEBUG_END;
}

// *********************************************************************************************
bool cChoiceListControl::setIndex (const String & NewIndex, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (String ("NewIndex:") + NewIndex);

    bool Response = setIndex (uint32_t (NewIndex.toInt ()), ResponseMessage, ForceUpdate);

    // DEBUG_V (String ("NewDataValueStr: ") + DataValueStr);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
bool cChoiceListControl::setIndex (uint32_t value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (String ("NewIndex: ") + String (value));

    bool Response = true;

    do  // once
    {
        if (ChoiceVector->size () <= value)
        {
            ResponseMessage = GetTitle () + F (": Set: BAD VALUE: '") + String (value) + F ("'");
            // Log.errorln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        String  Dummy;
        String  NewValue = (*ChoiceVector)[value].first;
        // DEBUG_V (String ("       NewValue: ") + NewValue);
        Response = set (NewValue, Dummy, ForceUpdate);

        // DEBUG_V (String ("NewDataValueStr: ") + DataValueStr);
    } while (false);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
bool cChoiceListControl::validate (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;
    // DEBUG_V (String ("value: ") + value);

    bool Response = true;

    do  // once
    {
        if (!ForceUpdate && DataValueStr.equals (value))
        {
            // DEBUG_V ("duplicate setting");

            if (KeyToChoiceVectorMap.end () != KeyToChoiceVectorMap.find (value))
            {
                // DEBUG_V ("Ignore duplicate setting");
                ResponseMessage = GetTitle () + F (": Ignoring duplicate setting for '") + value + "'";
                Response        = false;
                break;
            }

            // DEBUG_V ("duplicate but not valid");
        }

        if (KeyToChoiceVectorMap.end () != KeyToChoiceVectorMap.find (value))
        {
            // DEBUG_V ("Found a match for the value");
            DataValueStr = value;
            break;
        }

        // DEBUG_V ("No exact match for the value.");

        ResponseMessage = GetTitle () + F (": Set: BAD VALUE: '") + value + F ("'");
        // Log.errorln (ResponseMessage.c_str ());
        Response = false;
        break;
    } while (false);

    // DEBUG_V (String ("Response: ") + String (Response));
    // DEBUG_V (String (" Message: ") + ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *********************************************************************************************
// OEF
