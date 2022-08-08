/*
   File: commands.h
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
#include "config.h"
#include "PixelRadio.h"

class CommandProcessor
{
private:
    static bool audioMode          (String & payloadStr, String & ControllerName);
    static bool frequency          (String & payloadStr, String & ControllerName);
    static bool gpio19             (String & payloadStr, String & ControllerName);
    static bool gpio23             (String & payloadStr, String & ControllerName);
    static bool gpio33             (String & payloadStr, String & ControllerName);
    static bool gpio               (String & payloadStr, String & ControllerName, gpio_num_t pin);
    static bool info               (String & payloadStr, String & ControllerName);
    static bool log                (String & payloadStr, String & ControllerName);
    static bool mute               (String & payloadStr, String & ControllerName);
    static bool piCode             (String & payloadStr, String & ControllerName);
    static bool ptyCode            (String & payloadStr, String & ControllerName);
    static bool programServiceName (String & payloadStr, String & ControllerName);
    static bool radioText          (String & payloadStr, String & ControllerName);
    static bool rdsTimePeriod      (String & payloadStr, String & ControllerName);
    static bool reboot             (String & payloadStr, String & ControllerName);
    static bool rfCarrier          (String & payloadStr, String & ControllerName);
    static bool start              (String & payloadStr, String & ControllerName);
    static bool stop               (String & payloadStr, String & ControllerName);

public:
    CommandProcessor();
    virtual ~CommandProcessor() {}

    bool ProcessCommand(String &RawCommand, String & ControllerName);

}; // CommandProcessor

// *************************************************************************************************************************
// EOF
