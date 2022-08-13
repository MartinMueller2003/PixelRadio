#pragma once
/*
   File: radio.cpp
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
#include "PixelRadio.h"
#include "language.h"

// *********************************************************************************************
class cRadio
{
public:
                cRadio () {}
    virtual     ~cRadio() {}

    void        AddAdjControls (uint16_t Tab, ControlColor color);
    void        AddDiagControls (uint16_t Tab, ControlColor color);
    void        AddHomeControls (uint16_t Tab, ControlColor color);
    void        AddRadioControls (uint16_t Tab, ControlColor color);
    void        AddRdsControls (uint16_t Tab, ControlColor color);

    void        begin ();
    void        Poll();
    void        restoreConfiguration(JsonObject &json);
    void        saveConfiguration (JsonObject & json);
    
// Callbacks need to be public 
    void        CbRdsRst(Control *sender, int type);

private:
    void        setRfAutoOff(void);
    void        updateOnAirSign(void);

// UI declarations

    uint16_t    adjTab          = Control::noParent;
    uint16_t    diagTab         = Control::noParent;
    uint16_t    homeTab         = Control::noParent;
    uint16_t    radioTab        = Control::noParent;
    uint16_t    rdsTab          = Control::noParent;

    uint16_t    radioAutoID     = Control::noParent;
    uint16_t    radioPwrID      = Control::noParent;
    uint16_t    rdsRstID        = Control::noParent;

    bool        rfAutoFlg       = RF_AUTO_OFF_DEF_FLG;                 // Control, Turn Off RF carrier if no audio for 60Sec. false=Never turn off.

    uint8_t     analogVol = (atoi(ANA_VOL_DEF_STR));               // Control. Unused, for future expansion.
    uint32_t    rdsMsgTime = 0;
    bool        successFlg  = true;
    String      LastMessageSent;
    
#define OFF 0x00
#define ON 0x01

};

extern cRadio  Radio;

// *********************************************************************************************
// OEF
