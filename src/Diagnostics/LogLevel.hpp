#pragma once
/*
   File: LogLevel.cpp
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
#include "ChoiceListControl.hpp"

// *********************************************************************************************
class cLogLevel : public cChoiceListControl
{
public:

    cLogLevel ();
    virtual ~cLogLevel ()    {}

            void begin();
            void AddControls (uint16_t TabId, ControlColor color);
            void printLogLevel (Print * _logOutput, int logLevel);
    static  void printPrefix (Print * _logOutput, int logLevel);
    static  void printSuffix (Print * _logOutput, int logLevel);
            void printTimestamp (Print * _logOutput);


            bool set (const String & value, String & ResponseMessage, bool ForceUpdate = false);
private:

    // Division constants
    const unsigned long MSECS_IN_SEC    = 1000;
    const unsigned long SECS_IN_MIN     = 60;
    const unsigned long SECS_IN_HOUR    = 3600;
    const unsigned long SECS_IN_DAY     = 86400;


};  // class cLogLevel

extern cLogLevel LogLevel;

// *********************************************************************************************
// OEF
