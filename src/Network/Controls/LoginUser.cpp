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
#include <Arduino.h>
#include <ArduinoLog.h>
#include "LoginUser.hpp"
#include "memdebug.h"
#include "credentials_user.h"

static const PROGMEM char   WIFI_DEV_USER_NM_STR    []  = "LOGIN USER NAME";
static const PROGMEM char   USER_NAME_STR           []  = "USER_NAME_STR";
static const PROGMEM uint32_t USER_NM_MAX_SZ            = 10;
static const PROGMEM char WIFI_BLANK_MSG_STR      []    = "LEAVE BLANK FOR AUTO LOGIN";

// *********************************************************************************************
cLoginUser::cLoginUser () :   cControlCommon (USER_NAME_STR,
                                              ControlType::Text,
                                              WIFI_DEV_USER_NM_STR,
                                              LOGIN_USER_NAME_STR,
                                              USER_NM_MAX_SZ)
{
    // _ DEBUG_START;
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

    cControlCommon::AddControls (value, color);
    setMessage (WIFI_BLANK_MSG_STR, eCssStyle::CssStyleBlack);

    // DEBUG_END;
}

// *********************************************************************************************
cLoginUser LoginUser;

// *********************************************************************************************
// OEF
