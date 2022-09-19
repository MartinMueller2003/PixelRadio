/*
  *    File: BackupRestore.cpp
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
#include "BackupRestore.hpp"
#include "memdebug.h"

// *********************************************************************************************

static const PROGMEM char   BACKUP_RESTORE_STR  []  = "RESTORE";
static const PROGMEM char   BACKUP_RES_CFG_STR  []  = "RESTORE FROM SD CARD";
static const PROGMEM char   BACKUP_RES_FAIL_STR []  = "RESTORE FAILED: CHECK SD CARD";
static const PROGMEM char   BACKUP_RES_PASS_STR []  = "Restore Complete: Reboot Required";

// *********************************************************************************************
cBackupRestore::cBackupRestore () :   cButtonControl (BACKUP_RES_CFG_STR)
{
    // DEBUG_START;
    // DEBUG_END;
}

// *********************************************************************************************
void cBackupRestore::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;
    // DEBUG_V(String("TabId: ") + String(TabId));
    // DEBUG_V(String("color: ") + String(color));

    cButtonControl::AddControls (TabId, color);
    setControl (BACKUP_RESTORE_STR, eCssStyle::CssStyleWhite_grey);
    setMessageStyle (eCssStyle::CssStyleWhite);

    // DEBUG_END;
}

// *********************************************************************************************
bool cBackupRestore::set (const String &, String & ResponseMessage, bool)
{
    // DEBUG_START;
    // DEBUG_V (String ("DataValueStr: ") + DataValueStr);
    // DEBUG_V (String ("     Booting: ") + String (Booting));

    extern bool restoreConfiguration (uint8_t saveMode, const char * fileName);

    bool Response = true;
    ResponseMessage.clear ();

    if (!Booting)
    {
        if (restoreConfiguration (SD_CARD_MODE, String (F (BACKUP_FILE_NAME)).c_str ()))
        {
            setMessage (BACKUP_RES_PASS_STR);
            Log.infoln (BACKUP_RES_PASS_STR);
        }
        else
        {
            setMessage (BACKUP_RES_FAIL_STR);
            Log.errorln (BACKUP_RES_FAIL_STR);
        }
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cBackupRestore BackupRestore;

// *********************************************************************************************
// OEF
