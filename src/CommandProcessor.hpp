#pragma once
/*
   File: CommandProcessor.hpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
   license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: The command functions are shared by the three remote controllers: Serial, MQTT, HTTP.

 */

// *************************************************************************************************************************
#include <ArduinoLog.h>

class cCommandProcessor
{
public:

bool    audioMode          (String & payloadStr, String & ControllerName, String & Response);
bool    frequency          (String & payloadStr, String & ControllerName, String & Response);
bool    gpio19             (String & payloadStr, String & ControllerName, String & Response);
bool    gpio23             (String & payloadStr, String & ControllerName, String & Response);
bool    gpio33             (String & payloadStr, String & ControllerName, String & Response);
bool    gpio               (String & payloadStr, String & ControllerName, gpio_num_t pin, String & Response);
bool    info               (String & payloadStr, String & ControllerName, String & Response);
bool    log                (String & payloadStr, String & ControllerName, String & Response);
bool    mute               (String & payloadStr, String & ControllerName, String & Response);
bool    piCode             (String & payloadStr, String & ControllerName, String & Response);
bool    ptyCode            (String & payloadStr, String & ControllerName, String & Response);
bool    programServiceName (String & payloadStr, String & ControllerName, String & Response);
bool    radioText          (String & payloadStr, String & ControllerName, String & Response);
bool    rdsTimePeriod      (String & payloadStr, String & ControllerName, String & Response);
bool    reboot             (String & payloadStr, String & ControllerName, String & Response);
bool    rfCarrier          (String & payloadStr, String & ControllerName, String & Response);
bool    start              (String & payloadStr, String & ControllerName, String & Response);
bool    stop               (String & payloadStr, String & ControllerName, String & Response);
bool    HelpCommand        (String & payloadStr, String & ControllerName, String & Response);

public:

cCommandProcessor ();
virtual ~cCommandProcessor ()    {}

bool    ProcessCommand (String & RawCommand, String & ControllerName, String & Response);
bool    ProcessCommand (String & Command, String & parameters, String & ControllerName, String & Response);
};      // CommandProcessor

// *************************************************************************************************************************
// EOF
