/*
  *    File: CommandProcessor.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech, dkulp
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
  *    license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *    Note 1: The command functions are shared by the three remote controllers: Serial, MQTT, HTTP.
  *
  */

// *************************************************************************************************************************
#include <Arduino.h>
#include <map>

#include "CommandProcessor.hpp"
#include "memdebug.h"

#include "AudioMode.hpp"
#include "AudioMute.hpp"
#include "ControllerMgr.h"
#include "ControllerLocal.h"
#include "FrequencyAdjust.hpp"
#include "Gpio19.hpp"
#include "Gpio23.hpp"
#include "Gpio33.hpp"
#include "LogLevel.hpp"
#include "PiCode.hpp"
#include "ProgramServiceName.hpp"
#include "PtyCode.hpp"
#include "RdsReset.hpp"
#include "RebootControl.hpp"
#include "RfCarrier.hpp"

typedef bool(cCommandProcessor::*CmdHandler)(String & Parameter, String & ResponseMessage);
std::map <String, CmdHandler> ListOfCommands
{
    {"aud",     & cCommandProcessor::audioMode},
    {"freq",    & cCommandProcessor::frequency},
    {"gpio19",  & cCommandProcessor::gpio19},
    {"gpio23",  & cCommandProcessor::gpio23},
    {"gpio33",  & cCommandProcessor::gpio33},
    {"mute",    & cCommandProcessor::mute},
    {"pic",     & cCommandProcessor::piCode},
    {"rtper",   & cCommandProcessor::rdsTimePeriod},
    {"psn",     & cCommandProcessor::programServiceName},
    {"pty",     & cCommandProcessor::ptyCode},
    {"reboot",  & cCommandProcessor::reboot},
    {"rfc",     & cCommandProcessor::rfCarrier},
    {"rtm",     & cCommandProcessor::radioText},
    {"start",   & cCommandProcessor::start},
    {"stop",    & cCommandProcessor::stop},
    {"?",       & cCommandProcessor::HelpCommand},
    {"h",       & cCommandProcessor::HelpCommand},
    {"help",    & cCommandProcessor::HelpCommand},
};

#define CMD_LOG_RST_STR F ("restore")
#define CMD_LOG_SIL_STR F ("silent")

// *************************************************************************************************************************
cCommandProcessor::cCommandProcessor ()
{}

// *************************************************************************************************************************
bool cCommandProcessor::ProcessCommand (
                                        String  & Command,
                                        String  & Parameter,
                                        String  & ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("        Command: ") + Command);
    // DEBUG_V(String("      Parameter: ") + Parameter);
    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);

    bool response = false;

    Command.trim ();
    Command.toLowerCase ();

    Parameter.trim ();

    do  // once
    {
        if (ListOfCommands.end () == ListOfCommands.find (Command))
        {
            ResponseMessage = (String (F ("->ERROR: Unknown Command: '")) + Command + F ("'\n"));
            HelpCommand (Parameter, ResponseMessage);
            response = false;
            break;
        }

        // DEBUG_V ();
        response = (this->*ListOfCommands[Command])(Parameter, ResponseMessage);
    } while (false);

    // DEBUG_V(String("ResponseMessage: ") + ResponseMessage);
    // DEBUG_END;

    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::audioMode (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = AudioMode.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;

    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::frequency (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = FrequencyAdjust.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio19 (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = Gpio19.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio23 (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = Gpio23.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio33 (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = Gpio33.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::HelpCommand (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    ResponseMessage.reserve (1024);

    ResponseMessage += ("\n");
    ResponseMessage += ("=========================================\n");
    ResponseMessage += ("**      CONTROLLER COMMAND SUMMARY     **\n");
    ResponseMessage += ("=========================================\n");
    ResponseMessage += (" AUDIO MODE      : aud=mono : stereo\n");
    ResponseMessage += (" FREQUENCY       : freq=88.1<->107.9\n");
    ResponseMessage += (" GPIO-19 CONTROL : gpio19=read : outhigh : outlow\n");
    ResponseMessage += (" GPIO-23 CONTROL : gpio23=read : outhigh : outlow\n");
    ResponseMessage += (" GPIO-33 CONTROL : gpio33=read : outhigh : outlow\n");
    ResponseMessage += (" INFORMATION     : info=system\n");
    ResponseMessage += (" MUTE AUDIO      : mute=mute : unmute\n");
    ResponseMessage += (" PROG ID CODE    : pic=0x00FF <-> 0xFFFF\n");
    ResponseMessage += (" PROG SERV NAME  : psn=[8 char station name]\n");
    ResponseMessage += (" RADIOTXT MSG    : rtm=[64 char message]\n");
    ResponseMessage += (" RADIOTXT PERIOD : rtper=5 <-> 900 secs\n");
    ResponseMessage += (" REBOOT SYSTEM   : reboot=system\n");
    ResponseMessage += (" START RDS       : start=rds\n");
    ResponseMessage += (" STOP RDS        : stop=rds\n");
    ResponseMessage += (" LOG CONTROL     : log=silent : restore\n");
    ResponseMessage += (" HELP            : ?  h  help\n");
    ResponseMessage += ("=========================================\n");
    ResponseMessage += ("\n");

    // DEBUG_END;

    return true;
}

// *************************************************************************************************************************
bool cCommandProcessor::info (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool Response = HelpCommand (payloadStr, ResponseMessage);

    // DEBUG_END;

    return Response;
}

// *************************************************************************************************************************
// logCmd(): Set the Serial Log Level to Silent or reset back to system (Web UI) setting.
// This command is only used by the Serial Controller; The MQTT and HTTP controllers do not observe this command.
bool cCommandProcessor::log (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = LogLevel.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::mute (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = AudioMute.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::piCode (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = PiCode.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::ptyCode (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = PtyCode.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::programServiceName (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = ProgramServiceName.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::radioText (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    c_ControllerLOCAL * control = (c_ControllerLOCAL *)(ControllerMgr.GetControllerById (LocalControllerId));

    bool response = control->SetRdsText (payloadStr, ResponseMessage);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
// rdsTimePeriodCmd(): Set the RadioText Message Display Time. Input value is in seconds.
bool cCommandProcessor::rdsTimePeriod (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    c_ControllerLOCAL * control = (c_ControllerLOCAL *)(ControllerMgr.GetControllerById (LocalControllerId));

    bool response = control->SetRdsTime (payloadStr, ResponseMessage);

    // DEBUG_END;

    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::reboot (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = RebootControl.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::rfCarrier (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = RfCarrier.set (payloadStr, ResponseMessage, false, false);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::start (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    ControllerMgr.SetRdsOutputEnabled (true);

    // DEBUG_END;
    return true;
}

// *************************************************************************************************************************
bool cCommandProcessor::stop (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    ControllerMgr.SetRdsOutputEnabled (false);

    // DEBUG_END;
    return false;
}

// *************************************************************************************************************************
// EOF
