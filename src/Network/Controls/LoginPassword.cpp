/*
   File: LoginPassword.cpp
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

#include "LoginPassword.hpp"
#include "WiFiDriver.hpp"
#include "credentials_user.h"
#include "memdebug.h"

static const PROGMEM String     WIFI_DEV_USER_PW_STR    = "LOGIN USER PASSWORD";
static const PROGMEM String     USER_PW_STR             = "USER_PW_STR";
static const PROGMEM uint32_t   USER_PW_MAX_SZ          = 10;
static const PROGMEM String     WIFI_BLANK_MSG_STR      = "LEAVE BLANK FOR AUTO LOGIN";

// *********************************************************************************************
cLoginPassword::cLoginPassword () :   cPasswordControl (USER_PW_STR,
                                                        WIFI_DEV_USER_PW_STR,
                                                        LOGIN_USER_PW_STR,
                                                        USER_PW_MAX_SZ)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
cLoginPassword::~cLoginPassword ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cLoginPassword::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cPasswordControl::AddControls (value, color);
    setMessage (WIFI_BLANK_MSG_STR, eCssStyle::CssStyleBlack);

    // DEBUG_END;
}

// *********************************************************************************************
cLoginPassword LoginPassword;

// *********************************************************************************************
// OEF
