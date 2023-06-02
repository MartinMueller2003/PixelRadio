/*
  *    File: BackupSave.cpp
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

#include "PixelRadio.h"
#include "ConfigSave.hpp"
#include "BackupSave.hpp"
#include "memdebug.h"

// *********************************************************************************************

static const PROGMEM char   BACKUP_SAVE_STR     []  = "SAVE";
static const PROGMEM char   BACKUP_SAV_CFG_STR  []  = "SAVE TO SD CARD";
static const PROGMEM char   BACKUP_SAV_FAIL_STR []  = "SAVE FAILED: CHECK SD CARD";
static const PROGMEM char   BACKUP_SAV_PASS_STR []  = "Configuration Saved";

// *********************************************************************************************
cBackupSave::cBackupSave () :   cButtonControl (BACKUP_SAV_CFG_STR)
{
    // DEBUG_START;
    // DEBUG_END;
}

// *********************************************************************************************
void cBackupSave::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;
    // DEBUG_V(String("TabId: ") + String(TabId));
    // DEBUG_V(String("color: ") + String(color));

    cButtonControl::AddControls (TabId, color);
    setControl (BACKUP_SAVE_STR, eCssStyle::CssStyleWhite_grey);
    setMessageStyle (eCssStyle::CssStyleWhite);

    // DEBUG_END;
}

// *********************************************************************************************
bool cBackupSave::set (const String &, String & ResponseMessage, bool, bool)
{
    // DEBUG_START;
    // DEBUG_V (String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (String ("AddingControls: ") + String (AddingControls));

    extern bool saveConfiguration (uint8_t saveMode, const char * fileName);

    bool Response = true;
    ResponseMessage.clear ();

    if (!SystemBooting)
    {
        if (saveConfiguration (SD_CARD_MODE, String (F (BACKUP_FILE_NAME)).c_str ()))
        {
            setMessage (BACKUP_SAV_PASS_STR);
            Log.infoln (BACKUP_SAV_PASS_STR);
        }
        else
        {
            setMessage (BACKUP_SAV_FAIL_STR);
            Log.errorln (BACKUP_SAV_FAIL_STR);
        }
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cBackupSave BackupSave;

// *********************************************************************************************
// OEF
