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

    bool        audioMode          (String & payloadStr, String & ResponseMessage);
    bool        frequency          (String & payloadStr, String & ResponseMessage);
    bool        gpio19             (String & payloadStr, String & ResponseMessage);
    bool        gpio23             (String & payloadStr, String & ResponseMessage);
    bool        gpio33             (String & payloadStr, String & ResponseMessage);
    bool        gpio               (String & payloadStr, gpio_num_t pin, String & Response);
    bool        info               (String & payloadStr, String & ResponseMessage);
    bool        log                (String & payloadStr, String & ResponseMessage);
    bool        mute               (String & payloadStr, String & ResponseMessage);
    bool        piCode             (String & payloadStr, String & ResponseMessage);
    bool        ptyCode            (String & payloadStr, String & ResponseMessage);
    bool        programServiceName (String & payloadStr, String & ResponseMessage);
    bool        radioText          (String & payloadStr, String & ResponseMessage);
    bool        rdsTimePeriod      (String & payloadStr, String & ResponseMessage);
    bool        reboot             (String & payloadStr, String & ResponseMessage);
    bool        rfCarrier          (String & payloadStr, String & ResponseMessage);
    bool        start              (String & payloadStr, String & ResponseMessage);
    bool        stop               (String & payloadStr, String & ResponseMessage);
    bool        HelpCommand        (String & payloadStr, String & ResponseMessage);

public:

    cCommandProcessor ();
    virtual ~cCommandProcessor ()    {}

    // bool        ProcessCommand (const String & RawCommand, String & ResponseMessage);
    bool        ProcessCommand (String & Command, String & parameters, String & ResponseMessage);
};      // CommandProcessor

// *************************************************************************************************************************
// EOF
