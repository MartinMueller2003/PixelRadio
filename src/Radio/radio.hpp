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
#include "QN8027Radio.h"

// *********************************************************************************************
class cRadio
{
public:
                cRadio () {}
    virtual     ~cRadio() {}

    void        AddControls (uint16_t ctrlTab);
    void        begin ();
    void        restoreConfiguration(JsonObject &json);
    void        saveConfiguration (JsonObject & json);

#ifdef OldWay
    void        updateRadioSettings(void);
#endif // def OldWay

private:
    bool        calibrateAntenna(void);
    void        CbAdjFmFreq(Control *sender, int type);
    void        CbDigitalGainAdjust(Control *sender, int type);
    void        CbImpedanceAdjust(Control *sender, int type);
    void        CbMute(Control *sender, int type);
    void        CbRadioEmphasis(Control *sender, int type);
    void        CbRfCarrier(Control *sender, int type);
    void        CbRfPowerCallback(Control *sender, int type);
    void        CbTestMode(Control *sender, int type);
    void        CbVgaGainAdjust(Control *sender, int type);
    bool        checkRadioIsPresent(void);
    int8_t      getAudioGain(void);
    uint8_t     initRadioChip(void);
    uint16_t    measureAudioLevel(void);
    void        setAudioImpedance(void);
    void        setAudioMute(void);
    void        setDigitalGain(void);
    void        setFrequency(void);
    void        setMonoAudio(void);
    void        setPreEmphasis(void);
    void        setRfAutoOff(void);
    void        setRfCarrier(void);
    void        setRfPower(void);
    void        setVgaGain(void);
    void        updateOnAirSign(void);
    void        waitForIdle(uint16_t waitMs);





    void        sendStationName(String value) { FmRadio.sendStationName(value); }
    void        sendRadioText(String value)   { FmRadio.sendRadioText(value); }

    QN8027Radio FmRadio;
    SemaphoreHandle_t RadioSemaphore = NULL;

    uint16_t    radioAudioID    = Control::noParent;
    uint16_t    radioAudioMsgID = Control::noParent;
    uint16_t    radioVgaGainID  = Control::noParent;
    uint16_t    radioDgainID    = Control::noParent;
    uint16_t    radioAutoID     = Control::noParent;
    uint16_t    radioEmphID     = Control::noParent;
    uint16_t    radioFreqID     = Control::noParent;
    uint16_t    radioGainID     = Control::noParent;
    uint16_t    radioImpID      = Control::noParent;
    uint16_t    radioPwrID      = Control::noParent;
    uint16_t    radioRfEnbID    = Control::noParent;
    uint16_t    radioSaveID     = Control::noParent;
    uint16_t    radioSaveMsgID  = Control::noParent;
    uint16_t    adjMuteID       = Control::noParent;
    uint16_t    homeOnAirID     = Control::noParent;
    uint16_t    adjTestModeID   = Control::noParent;

    bool        testModeFlg;
    bool        muteFlg         = RADIO_MUTE_DEF_FLG;                  // Control, Mute audio if true.
    bool        rfAutoFlg       = RF_AUTO_OFF_DEF_FLG;                 // Control, Turn Off RF carrier if no audio for 60Sec. false=Never turn off.
    bool        rfCarrierFlg    = RF_CARRIER_DEF_FLG;                  // Control, Turn off RF if false.
    bool        stereoEnbFlg    = STEREO_ENB_DEF_FLG;                  // Control, Enable Stereo FM if true (false = Mono).

    uint8_t     analogVol = (atoi(ANA_VOL_DEF_STR));               // Control. Unused, for future expansion.
    uint32_t    rdsMsgTime = 0;
    uint16_t    PiCode = 0;
    uint16_t    PtyCode = 0;
    uint16_t    fmFreqX10 = FM_FREQ_DEF_X10;                        // Control. FM MHz Frequency X 10 (881 - 1079).
    bool        successFlg  = true;
    String      ProgramServiceName;

// FM Radio: QN8027 Test Codes
#define  FM_TEST_OK   0          // QN8027 Is Ok.
#define  FM_TEST_VSWR 1          // QN8027 RF Out has Bad VSWR.
#define  FM_TEST_FAIL 2          // QN8027 Chip Bad or missing.
    uint8_t     fmRadioTestCode = FM_TEST_OK;        // FM Radio Module Test Result Code.

#define PRE_EMPH_USA_STR     F("North America (75uS)") // North America / Japan.
#define PRE_EMPH_EUR_STR     F("Europe (50uS)")        // Europe, Australia, China.
#define PRE_EMPH_DEF_STR     PRE_EMPH_USA_STR;
    String preEmphasisStr = PRE_EMPH_DEF_STR;                  // Control.

#define DIG_GAIN0_STR     F("0 dB (default)")
#define DIG_GAIN1_STR     F("1 dB")
#define DIG_GAIN2_STR     F("2 dB")
#define DIG_GAIN_DEF_STR  DIG_GAIN0_STR;
    String digitalGainStr = DIG_GAIN_DEF_STR;                  // Control.
    
#define INP_IMP05K_STR   F("5K Ohms")
#define INP_IMP10K_STR   F("10K Ohms")
#define INP_IMP20K_STR   F("20K Ohms (default)")
#define INP_IMP40K_STR   F("40K Ohms")
#define INP_IMP_DEF_STR  INP_IMP20K_STR;
    String inpImpedStr = INP_IMP_DEF_STR;                   // Control.
    
#define VGA_GAIN0_STR     F("3dB")
#define VGA_GAIN1_STR     F("6dB")
#define VGA_GAIN2_STR     F("9dB")
#define VGA_GAIN3_STR     F("12dB (default)")
#define VGA_GAIN4_STR     F("15dB")
#define VGA_GAIN5_STR     F("18dB")
#define VGA_GAIN_DEF_STR  VGA_GAIN3_STR;
    String vgaGainStr = VGA_GAIN_DEF_STR;                  // Control.
    
#define RF_PWR_LOW_STR   F("Low")
#define RF_PWR_MED_STR   F("Med")
#define RF_PWR_HIGH_STR  F("High (default)")
#define RF_PWR_DEF_STR   RF_PWR_HIGH_STR;
    String rfPowerStr = RF_PWR_DEF_STR;                    // Control.

};

extern cRadio  Radio;

// *********************************************************************************************
// OEF
