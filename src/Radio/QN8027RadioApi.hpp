#pragma once
/*
  *    File: QN8027RadioApi.cpp
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
#include "QN8027Radio.h"
#include <Arduino.h>

// *********************************************************************************************
class cQN8027RadioApi
{
public:

    cQN8027RadioApi ()    {}
    virtual~cQN8027RadioApi ()    {}

    void        begin ();
    uint16_t    GetPeakAudioLevel (void);
    void        setAudioImpedance (uint8_t value);
    void        setAudioMute (bool value);
    void        setDigitalGain (uint8_t value);
    void        setFrequency (float frequency, bool Carrier);
    void        setMonoAudio (bool value);
    void        setPreEmphasis (uint8_t value, bool carrier);
    void        setProgramServiceName (String & value, bool carrier);
    void        setPiCode (uint16_t value, bool carrier);
    void        setPtyCode (uint8_t value, bool carrier);
    void        setRdsMessage (String & value);
    void        setRfAutoOff (bool value, bool carrier);
    void        setRfCarrierOFF (void);
    void        setRfCarrier (bool value);
    void        setRfPower (uint8_t value, bool carrier);
    void        setVgaGain (uint8_t value);

    // FM Radio: QN8027 Test response Codes
    typedef enum
    {
        FM_TEST_OK = 0,     // QN8027 Is Ok.
        FM_TEST_VSWR,       // QN8027 RF Out has Bad VSWR.
        FM_TEST_MISSING,    // QN8027 Chip missing.
        FM_TEST_FAIL        // QN8027 Chip Bad.
    } QN8027RadioFmTestStatus_e;
    QN8027RadioFmTestStatus_e GetTestStatus () {return TestStatus;}

private:

    bool    calibrateAntenna (void);
    bool    checkRadioIsPresent (void);
    void    initRadioChip (void);
    void    waitForIdle (uint16_t waitMs);

    bool                        DeviceIsPresent = false;
    QN8027Radio                 FmRadio;
    SemaphoreHandle_t           RadioSemaphore = NULL;

    QN8027RadioFmTestStatus_e   TestStatus = QN8027RadioFmTestStatus_e::FM_TEST_FAIL;
};  // class cQN8027RadioApi

extern cQN8027RadioApi QN8027RadioApi;

// *********************************************************************************************
// OEF
