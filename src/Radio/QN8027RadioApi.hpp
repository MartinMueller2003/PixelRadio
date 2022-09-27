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
    uint16_t    GetPeakAudioLevel (bool SkipSemaphore = false);
    void        setAudioImpedance (uint8_t value, bool SkipSemaphore = false);
    void        setAudioMute (bool value, bool SkipSemaphore = false);
    void        setDigitalGain (uint8_t value, bool SkipSemaphore = false);
    void        setFrequency (float frequency, bool Carrier, bool SkipSemaphore = false);
    void        setMonoAudio (bool value, bool SkipSemaphore = false);
    void        setPreEmphasis (uint8_t value, bool carrier, bool SkipSemaphore = false);
    void        setProgramServiceName (String & value, bool carrier, bool SkipSemaphore = false);
    void        setPiCode (uint16_t value, bool carrier, bool SkipSemaphore = false);
    void        setPtyCode (uint8_t value, bool carrier, bool SkipSemaphore = false);
    void        setRdsMessage (String & value, bool SkipSemaphore = false);
    void        setRfAutoOff (bool value, bool carrier, bool SkipSemaphore = false);
    void        setRfCarrierOFF (bool SkipSemaphore = false);
    void        setRfCarrier (bool value, bool SkipSemaphore = false);
    void        setRfPower (uint8_t value, bool carrier, bool SkipSemaphore = false);
    void        setVgaGain (uint8_t value, bool SkipSemaphore = false);

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

    bool    calibrateAntenna (bool SkipSemaphore = false);
    bool    checkRadioIsPresent (bool SkipSemaphore = false);
    void    initRadioChip (bool SkipSemaphore = false);
    void    waitForIdle (uint16_t waitMs, bool SkipSemaphore = false);

    bool                        DeviceIsPresent = false;
    QN8027Radio                 FmRadio;
    SemaphoreHandle_t           RadioSemaphore = NULL;

    QN8027RadioFmTestStatus_e   TestStatus = QN8027RadioFmTestStatus_e::FM_TEST_FAIL;
};  // class cQN8027RadioApi

extern cQN8027RadioApi QN8027RadioApi;

// *********************************************************************************************
// OEF
