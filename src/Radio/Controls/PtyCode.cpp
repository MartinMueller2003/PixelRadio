/*
   File: PtyCode.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <vector>
#include "PtyCode.hpp"
#include "PreEmphasis.hpp"
#include "RfCarrier.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

typedef struct 
{
    uint16_t code;
    uint16_t UiId;
    String name[2];
} PtyCodeEntry_t;

// 0 = US, 1 = EU
static std::vector<PtyCodeEntry_t> ListOfPtyCodes =
{
    { 0, Control::noParent, {{"No programme type defined"}, {"No programme type defined"}}},
    { 1, Control::noParent, {{"News"},                      {"News"}}},
    { 2, Control::noParent, {{"Information"},               {"Current affairs"}}},
    { 3, Control::noParent, {{"Sport"},                     {"Information"}}},
    { 4, Control::noParent, {{"Talk"},                      {"Sport"}}},
    { 5, Control::noParent, {{"Rock"},                      {"Education"}}},
    { 6, Control::noParent, {{"Classic Rock"},              {"Drama"}}},
    { 7, Control::noParent, {{"Adult Hits"},                {"Culture"}}},
    { 8, Control::noParent, {{"Soft Rock"},                 {"Soft Science"}}},
    { 9, Control::noParent, {{"Top 40"},                    {"Varied"}}},
    {10, Control::noParent, {{"Country Music"},             {"Popular Music (Pop)"}}},
    {11, Control::noParent, {{"Oldies (Music)"},            {"Rock Music"}}},
    {12, Control::noParent, {{"Soft Music"},                {"Easy Listening"}}},
    {13, Control::noParent, {{"Nostalgia"},                 {"Light Classical"}}},
    {14, Control::noParent, {{"Jazz"},                      {"Serious Classical"}}},
    {15, Control::noParent, {{"Classical"},                 {"Other Music"}}},
    {16, Control::noParent, {{"Rhythm & Blues"},            {"Weather"}}},
    {17, Control::noParent, {{"Soft Rhythm & Blues"},       {"Finance"}}},
    {18, Control::noParent, {{"Language"},                  {"Children's Programmes"}}},
    {19, Control::noParent, {{"Religious Music"},           {"Social Affairs"}}},
    {20, Control::noParent, {{"Religious Talk"},            {"Religion"}}},
    {21, Control::noParent, {{"Personality"},               {"Phone-in"}}},
    {22, Control::noParent, {{"Public"},                    {"Travel"}}},
    {23, Control::noParent, {{"College"},                   {"Leisure"}}},
    {24, Control::noParent, {{"Not assigned 1"},            {"Jazz Music"}}},
    {25, Control::noParent, {{"Not assigned 2"},            {"Country Music"}}},
    {26, Control::noParent, {{"Not assigned 3"},            {"National Music"}}},
    {27, Control::noParent, {{"Not assigned 4"},            {"Oldies Music"}}},
    {28, Control::noParent, {{"Not assigned 5"},            {"Folk Music"}}},
    {29, Control::noParent, {{"Weather"},                   {"Documentary"}}},
    {30, Control::noParent, {{"Emergency Test"},            {"Alarm Test"}}},
    {31, Control::noParent, {{"Emergency"},                 {"Alarm"}}},
};

static const PROGMEM String RDS_PTY_CODE_STR    = "PTY CODE<br>Station Genre";
static const PROGMEM String RDS_PTY_CODE        = "RDS_PTY_CODE";

// *********************************************************************************************
cPtyCode::cPtyCode() : cControlCommon(RDS_PTY_CODE)
{
    //_ DEBUG_START;

    DataValue    = 9;
    DataValueStr = "9";

    //_ DEBUG_END;
}

// *********************************************************************************************
void cPtyCode::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cControlCommon::AddControls(value, ControlType::Select, color);
    ESPUI.updateControlLabel(ControlId, RDS_PTY_CODE_STR.c_str());
    ESPUI.setPanelStyle(ControlId, "font-size: 1.15em;");

    setPtyCodeOptionValues();

    // DEBUG_END;
}

// *********************************************************************************************
void cPtyCode::ResetToDefaults()
{
    DEBUG_START;

    String value = "9";
    String dummy;
    set(value, dummy);
    
    DEBUG_END;
}

// ************************************************************************************************
bool cPtyCode::set(String & value, String & ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("   DataValue: ") + String(DataValue));

    bool Response = false;
    ResponseMessage.reserve(128);
    ResponseMessage.clear();

    uint32_t CurrentRegion = PreEmphasis.get();

    do // once
    {
        if(isdigit(value[0]))
        {
            // DEBUG_V("Value is a number");
            uint32_t NewData = atoi(value.c_str());
            if (ListOfPtyCodes.size() >= NewData)
            {
                // DEBUG_V("Value is a number");
                DataValue = NewData;
                DataValueStr = String(DataValue);
                // DEBUG_V(String("   DataValue: ") + String(DataValue));
                // DEBUG_V(String("DataValueStr: ") + DataValueStr);
                Response = true;
                break;
            }
        }

        for (auto &CurrentEntry : ListOfPtyCodes)
        {
            if(value.equalsIgnoreCase(CurrentEntry.name[CurrentRegion]))
            {
                // DEBUG_V("Value is a string");
                // DEBUG_V(String("   DataValue: ") + String(DataValue));
                // DEBUG_V(String("DataValueStr: ") + DataValueStr);

                DataValue = CurrentEntry.code;
                DataValueStr = String(DataValue);
                Response = true;
                break;
            }
        }

    } while (false);

    if(Response)
    {
        QN8027RadioApi.setPtyCode(DataValue, RfCarrier.get());
        updateUiPtyCode();
    }
    else
    {
        ResponseMessage = String(F("Set PTY Code: BAD VALUE: '")) + value + F("'");
    }

    // DEBUG_V(String("   DataValueStr: ") + DataValueStr);
    // DEBUG_V(String("      DataValue: ") + String(DataValue));
    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V(String("       Response: ") + String(Response));

    // DEBUG_END;
    return Response;
}

// ************************************************************************************************
void cPtyCode::setPtyCodeOptionValues ()
{
    // DEBUG_START;
    
    // DEBUG_V(String("DataValue: ") + String(DataValue));
    // DEBUG_V(String("M0: ") + ListOfPtyCodes[2].name[0]);
    // DEBUG_V(String("M1: ") + ListOfPtyCodes[2].name[1]);

    uint32_t CurrentRegion = PreEmphasis.get();

    if(Control::noParent != ControlId)
    {
        for (auto &CurrentEntry : ListOfPtyCodes)
        {
            // DEBUG_V(String("code: ") + String(CurrentEntry.code));

            if (Control::noParent == CurrentEntry.UiId)
            {
                // DEBUG_V("Create Option");
                CurrentEntry.UiId = ESPUI.addControl(
                                            ControlType::Option, 
                                            emptyString.c_str(), 
                                            emptyString, 
                                            ControlColor::None, 
                                            ControlId);
            }
            Control *control = ESPUI.getControl(CurrentEntry.UiId);
            if(control)
            {
                // DEBUG_V("Update control fields");
                control->value = CurrentEntry.name[CurrentRegion];
                control->label = CurrentEntry.name[CurrentRegion].c_str();
                ESPUI.updateControl(control);
                // DEBUG_V(String("label: ") + String(control->label));
            }
            else
            {
                // DEBUG_V("Could not access control")
            }
        }
        updateUiPtyCode();
        ESPUI.jsonDom(0);
    }

    // DEBUG_END;
}

// ************************************************************************************************
void cPtyCode::updateUiPtyCode()
{
    // DEBUG_START;

    // DEBUG_V(String("  ControlId: ") + String(ControlId));
    // DEBUG_V(String("  DataValue: ") + String(DataValue));
    // DEBUG_V(String("PreEmphasis: ") + String(PreEmphasis.get()));

    ESPUI.updateControlValue(ControlId, ListOfPtyCodes[DataValue].name[PreEmphasis.get()]);

    // DEBUG_END;
}

// *********************************************************************************************
cPtyCode PtyCode;

// *********************************************************************************************
// OEF
