/*
   File: LoginUser.cpp
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
#include "LoginUser.hpp"
#include "memdebug.h"
#include "WiFiDriver.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>
#include "credentials_user.h"

static const PROGMEM String WIFI_DEV_USER_NM_STR        = "LOGIN USER NAME";
static const PROGMEM uint32_t USER_NM_MAX_SZ            = 10;

// *********************************************************************************************
cLoginUser::cLoginUser () : cOldControlCommon ("USER_NAME_STR")
{
    // _ DEBUG_START;

    DataValueStr.reserve (USER_NM_MAX_SZ + 2);
    DataValueStr = LOGIN_USER_NAME_STR;

    // _ DEBUG_END;
}

// *********************************************************************************************
cLoginUser::~cLoginUser ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cLoginUser::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cOldControlCommon::AddControls (value, ControlType::Text, color);
    ESPUI.updateControlLabel (ControlId, WIFI_DEV_USER_NM_STR.c_str ());
    ESPUI.addControl (ControlType::Max, emptyString.c_str (), String (USER_NM_MAX_SZ), ControlColor::None, ControlId);

    // DEBUG_END;
}

// *********************************************************************************************
void cLoginUser::ResetToDefaults ()
{
    // DEBUG_START;

    String      value = LOGIN_USER_NAME_STR;
    String      dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cLoginUser::set (String & value, String & ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    do  // once
    {
        if (value.length () > USER_NM_MAX_SZ)
        {
            ResponseMessage     = String (F ("Device User Name New Value is too long: \""))  + value + String (F ("\""));
            Response            = false;
            Log.warningln (ResponseMessage.c_str ());
            break;
        }

        if (value.equals (DataValueStr))
        {
            // DEBUG_V("Name did not change");
            Log.infoln (String (F ("Device User Name Unchanged")).c_str ());
            break;
        }
        DataValueStr = value;

        ESPUI.updateControlValue (ControlId, DataValueStr);
        Log.infoln ((String (F ("Device User Name Set to: '")) + DataValueStr + "'").c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cLoginUser LoginUser;

// *********************************************************************************************
// OEF
