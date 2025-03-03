#pragma once
/*
  *    File: TestTone.cpp
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
#include "BinaryControl.hpp"
#include <Arduino.h>

class fsm_Tone_state;

// *********************************************************************************************
class cTestTone : public cBinaryControl
{
public:

    cTestTone ();
    virtual~cTestTone ()    {}

    void    Init ();
    void    poll ();

protected:

    void    UpdateRdsTimeMsg ();
    void    toneOn (uint16_t freq);
    void    toneOff ();

    uint8_t         hours   = 0;
    uint8_t         minutes = 0;
    uint8_t         seconds = 0;

    friend class fsm_Tone_state_Idle;
    friend class fsm_Tone_state_SendingTone;
    fsm_Tone_state  * pCurrentFsmState      = nullptr;
    uint32_t        FsmTimerExpirationTime  = 0;
};  // class cTestTone

class fsm_Tone_state
{
protected:

    cTestTone                           * pTestTone = nullptr;
    uint32_t                            ToneExpirationTime;
    std::vector <uint16_t>::iterator    CurrentTone;

public:

    fsm_Tone_state ()
    {}

    virtual~fsm_Tone_state ()
    {}

    virtual void    Poll (uint32_t) = 0;
    virtual void    Init (void)     = 0;
    uint16_t        getCurrentToneFrequency () {return *CurrentTone;}

    void SetParent (cTestTone * parent) {pTestTone = parent;}
};  // fsm_Tone_state

/*****************************************************************************/
// Wait for polling to start.
class fsm_Tone_state_Idle : public fsm_Tone_state
{
public:

    fsm_Tone_state_Idle ()
    {}

    virtual~fsm_Tone_state_Idle ()
    {}

    virtual void    Poll (uint32_t now);
    virtual void    Init (void);
};  // class fsm_Tone_state_Idle

/*****************************************************************************/
class fsm_Tone_state_SendingTone : public fsm_Tone_state
{
public:

    fsm_Tone_state_SendingTone ()
    {}

    virtual~fsm_Tone_state_SendingTone ()
    {}

    virtual void    Poll (uint32_t now);
    virtual void    Init (void);
};  // class fsm_Tone_state_SendingTone


extern cTestTone TestTone;

// *********************************************************************************************
// OEF
