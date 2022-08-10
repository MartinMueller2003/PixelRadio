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

    void        AddAdjControls (uint16_t Tab);
    void        AddDiagControls (uint16_t Tab);
    void        AddHomeControls (uint16_t Tab);
    void        AddRadioControls (uint16_t Tab);
    void        AddRdsControls(uint16_t Tab);

    void        begin ();
    void        Poll();
    void        restoreConfiguration(JsonObject &json);
    void        saveConfiguration (JsonObject & json);
    
// Callbacks need to be public 
    void        CbDigitalGainAdjust(Control *sender, int type);
    void        CbImpedanceAdjust(Control *sender, int type);
    void        CbProgramServiceName(Control *sender, int type);
    void        CbRadioEmphasis(Control *sender, int type);
    void        CbRfPowerCallback(Control *sender, int type);
    void        CbRdsRst(Control *sender, int type);
    void        CbSetPiCode(Control *sender, int type);
    void        CbSetPtyCode(Control *sender, int type);
    void        CbVgaGainAdjust(Control *sender, int type);

private:
    int8_t      getAudioGain(void);

    void        setDigitalGain(void);
    void        setPiCode(uint16_t value);
    void        setPiCode();
    void        setProgramServiceName();
    void        setPtyCode();
    void        setPtyCode(String & ptyStr);
    void        setPtyCodeOptionValues ();
    void        setRfAutoOff(void);
    void        setRfPower(void);
    void        setVgaGain(void);
    void        updateOnAirSign(void);
    void        updateRdsMsgRemainingTime(unsigned long now);
    void        updateUiAudioLevel(void);
    void        updateUiPtyCode();
    void        updateUiRdsText(String & Text);
    void        waitForIdle(uint16_t waitMs);

// UI declarations
#ifdef OldWay
    void        sendStationName(String value) { FmRadio.sendStationName(value); }
    void        sendRadioText(String value)   { FmRadio.sendRadioText(value); }
#endif // def OldWay
    void        sendStationName(String value) { }
    void        sendRadioText(String value)   { }

    uint16_t    adjTab          = Control::noParent;
    uint16_t    diagTab         = Control::noParent;
    uint16_t    homeTab         = Control::noParent;
    uint16_t    radioTab        = Control::noParent;
    uint16_t    rdsTab          = Control::noParent;

    uint16_t    radioVgaGainID  = Control::noParent;
    uint16_t    radioDgainID    = Control::noParent;
    uint16_t    radioAutoID     = Control::noParent;
    uint16_t    radioGainID     = Control::noParent;
    uint16_t    radioPwrID      = Control::noParent;
    uint16_t    radioRfEnbID    = Control::noParent;
    uint16_t    adjFmDispID     = Control::noParent;
    uint16_t    radioSoundID    = Control::noParent;
    uint16_t    rdsPiID         = Control::noParent;
    uint16_t    rdsPtyID        = Control::noParent;
    uint16_t    rdsProgNameID   = Control::noParent;
    uint16_t    rdsRstID        = Control::noParent;

    bool        rfAutoFlg       = RF_AUTO_OFF_DEF_FLG;                 // Control, Turn Off RF carrier if no audio for 60Sec. false=Never turn off.

    uint8_t     analogVol = (atoi(ANA_VOL_DEF_STR));               // Control. Unused, for future expansion.
    uint32_t    rdsMsgTime = 0;
    bool        successFlg  = true;
    uint16_t    PiCode = 0x6400;
    uint16_t    PtyCode = 0;
    String      ProgramServiceName = F("PixeyFM");
    String      LastMessageSent;
    
#define OFF 0x00
#define ON 0x01

#define DIG_GAIN0_STR     "0 dB (default)"
#define DIG_GAIN1_STR     "1 dB"
#define DIG_GAIN2_STR     "2 dB"
#define DIG_GAIN_DEF_STR  DIG_GAIN0_STR;
    String digitalGainStr = DIG_GAIN_DEF_STR;                  // Control.


#define VGA_GAIN0_STR     "3dB"
#define VGA_GAIN1_STR     "6dB"
#define VGA_GAIN2_STR     "9dB"
#define VGA_GAIN3_STR     "12dB (default)"
#define VGA_GAIN4_STR     "15dB"
#define VGA_GAIN5_STR     "18dB"
#define VGA_GAIN_DEF_STR  VGA_GAIN3_STR;
    String vgaGainStr = VGA_GAIN_DEF_STR;                  // Control.
    
#define RF_PWR_LOW_STR   "Low"
#define RF_PWR_MED_STR   "Med"
#define RF_PWR_HIGH_STR  "High (default)"
#define RF_PWR_DEF_STR   RF_PWR_HIGH_STR;
    String rfPowerStr = RF_PWR_DEF_STR;                    // Control.

};

extern cRadio  Radio;

// *********************************************************************************************
// OEF
