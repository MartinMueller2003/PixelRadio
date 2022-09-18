/*
  *    File: PeakAudio.cpp
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
#include "memdebug.h"
#include "PeakAudio.hpp"
#include "QN8027RadioApi.hpp"

static const PROGMEM char RADIO_AUDLVL_STR    []    = "PEAK AUDIO LEVEL";
static const PROGMEM uint32_t   AUDIO_MEAS_TIME     = uint32_t (2000);
static const PROGMEM uint32_t   AUDIO_LEVEL_MAX     = uint32_t (675);

// *********************************************************************************************
cPeakAudio::cPeakAudio () :   cStatusControl (RADIO_AUDLVL_STR)
{
    // _ DEBUG_START;

    // _ DEBUG_END;
}

// *********************************************************************************************
void cPeakAudio::poll ()
{
    // _ DEBUG_START;

    uint32_t    now             = millis ();
    uint16_t    currentReading  = 0;
    String      Result;

    Result.reserve (128);

    do  // once
    {
        if (now <= NextReadingTime)
        {
            // Keep waiting
            break;
        }

        NextReadingTime = now + AUDIO_MEAS_TIME;

        currentReading = QN8027RadioApi.GetPeakAudioLevel ();

        if (currentReading >= AUDIO_LEVEL_MAX)
        {
            Result = ">";
        }

        Result  += String (currentReading);
        Result  += F (" mV");

        if (!get ().equals (Result))
        {
            set (Result);
            Log.verboseln ((String (F ("Peak Audio Amplitude: ")) + Result).c_str ());
        }
    } while (false);

    // _ DEBUG_END;
}

// *********************************************************************************************
cPeakAudio PeakAudio;

// *********************************************************************************************
// OEF
