/*
  *    File: ConfigSave.cpp
  *    Project: PixelConfigSave, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelConfigSave.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <vector>

#include "PixelRadio.h"
#include "ConfigSave.hpp"
#include "SaveControl.hpp"
#include "memdebug.h"

// *********************************************************************************************

static std::vector <cSaveControl *> ListOfSaveControls;

// *********************************************************************************************

void cConfigSave::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    // DEBUG_V(String("TabId: ") + String(TabId))
    // DEBUG_V(String("color: ") + String(color))

    ListOfSaveControls.emplace_back (new cSaveControl ());
    ListOfSaveControls.back ()->AddControls (TabId, color);

    // DEBUG_END;
}

// *********************************************************************************************
void cConfigSave::SetSaveNeeded ()
{
    // DEBUG_START;

    for (auto & CurrentControl : ListOfSaveControls)
    {
        CurrentControl->SetSaveNeeded ();
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cConfigSave::ClearSaveNeeded ()
{
    // DEBUG_START;

    for (auto & CurrentControl : ListOfSaveControls)
    {
        CurrentControl->ClearSaveNeeded ();
    }

    // DEBUG_END;
}

// *********************************************************************************************
void cConfigSave::InitiateSaveOperation ()
{
    // DEBUG_START;

    saveConfiguration(LITTLEFS_MODE, BACKUP_FILE_NAME);
    ClearSaveNeeded ();

    // DEBUG_END;
}

// *********************************************************************************************
cConfigSave ConfigSave;

// *********************************************************************************************
// OEF
