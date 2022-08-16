/*
   File: ControllerCommon.cpp
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
#include "ControllerCommon.h"
#include "Language.h"

#if __has_include ("memdebug.h")
# include "memdebug.h"
#endif //  __has_include("memdebug.h")

// *********************************************************************************************
// class c_ControllerCommon

// *********************************************************************************************
c_ControllerCommon::c_ControllerCommon (String _Name, c_ControllerMgr::ControllerTypeId_t _Id)
{
    Name        = _Name;
    TypeId      = _Id;
}       // c_ControllerCommon

// *********************************************************************************************
c_ControllerCommon::~c_ControllerCommon ()
{
}

// ************************************************************************************************
void c_ControllerCommon::AddControls (uint16_t tabId)
{
    // DEBUG_START;

    EspuiParentElementId = tabId;

    ControlName         = Name + F (" CONTROL");
    SeperatorName       = ControlName + F (" SETTINGS");

    ESPUI.addControl (
        ControlType::Separator,
        SeperatorName.c_str (),
        emptyString,
        ControlColor::None,
        tabId);

    ControlLabelElementId = ESPUI.addControl (
            ControlType::Label,
            ControlName.c_str (),
            "ENABLE",
            ControlColor::Turquoise,
            EspuiParentElementId);
    ESPUI.setElementStyle (ControlLabelElementId, CSS_LABEL_STYLE_WHITE);

    ControlerEnabledElementId = ESPUI.addControl (
            ControlType::Switcher,
            "Enable",
            String (ControllerEnabled ? F ("1") : F ("0")),
            ControlColor::Turquoise,
            ControlLabelElementId,
            [] (Control * sender, int type, void * param)
            {
                if (param)
                {
                    reinterpret_cast <c_ControllerCommon *> (param)->CbControllerEnabled (sender, type);
                }
            },
            this);
    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerCommon::CbControllerEnabled (Control * sender, int type)
{
    // DEBUG_START;

    ControllerEnabled = (type == S_ACTIVE);

    // DEBUG_V();
    displaySaveWarning ();
    String  Temp = Name;
    Temp        += F (" Controller Set to: ");
    Temp        += ControllerEnabled ? F ("On") : F ("Off");
    Log.infoln (Temp.c_str ());

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerCommon::restoreConfiguration (ArduinoJson::JsonObject &config)
{
    // DEBUG_START;

    ReadFromJSON (ControllerEnabled, config, N_ControllerEnabled);

    // DEBUG_END;
}       // RestoreConfiguration

// *********************************************************************************************
void c_ControllerCommon::saveConfiguration (ArduinoJson::JsonObject &config)
{
    // DEBUG_START;

    config[N_name]              = Name;
    config[N_type]              = TypeId;
    config[N_ControllerEnabled] = ControllerEnabled;

    // DEBUG_END;
}       // saveConfiguration

// *********************************************************************************************
// EOF
