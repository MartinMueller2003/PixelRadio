/*
   File: CommandProcessor.cpp
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
#include <Arduino.h>
#include <map>

#include "CommandProcessor.hpp"
// #include "language.h"
#include "memdebug.h"
// #include "radio.hpp"

#include "AudioMode.hpp"
#include "AudioMute.hpp"
#include "FrequencyAdjust.hpp"
#include "PiCode.hpp"
#include "ProgramServiceName.hpp"
#include "PtyCode.hpp"
#include "RdsReset.hpp"
#include "RfCarrier.hpp"

typedef bool (cCommandProcessor::*CmdHandler)(String & Parameter, String & ResponseMessage);
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

    bool response = AudioMode.set (payloadStr, ResponseMessage);

    // DEBUG_END;

    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::frequency (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = FrequencyAdjust.set (payloadStr, ResponseMessage);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio19 (String & payloadStr, String & ResponseMessage)
{
#ifdef OldWay

        return gpio (payloadStr, ControllerName, GPIO19_PIN, Response);

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio23 (String & payloadStr, String & ResponseMessage)
{
#ifdef OldWay

        return gpio (payloadStr, ControllerName, GPIO23_PIN, Response);

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio33 (String & payloadStr, String & ResponseMessage)
{
#ifdef OldWay

        return gpio (payloadStr, ControllerName, GPIO33_PIN, Response);

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
// gpioCmd(): Read/Write the User's GPIO Pin States.  On exit, return true if success.
bool cCommandProcessor::gpio (String & payloadStr, gpio_num_t pin, String & ResponseMessage)
{
#ifdef OldWay
        // DEBUG_START;

        bool response = true;

        do  // once
        {
            if (payloadStr.length () > CMD_GPIO_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_GPIO_MAX_SZ);
            }

            if (payloadStr.equals (F (CMD_GPIO_READ_STR)))
            {
                Log.infoln ((String (F ("-> ")) + ControllerName + F (" Controller: Read GPIO Pin-") + String (pin) + F (", Value= ") +
                             String (digitalRead (pin))).c_str ());
                break;
            }

            if (!payloadStr.equals (CMD_GPIO_OUT_HIGH_STR) &&
                !payloadStr.equals (CMD_GPIO_OUT_LOW_STR))
            {
                Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid GPIO Payload (") + payloadStr + F (
                                "), Ignored.")).c_str ());
                response = false;
                break;
            }
            bool NewPinState = payloadStr.equals (CMD_GPIO_OUT_HIGH_STR) ? HIGH : LOW;
            digitalWrite (pin, NewPinState);
            Log.infoln ((String (F ("-> ")) + ControllerName + F (" Controller: Setting GPIO Pin-") + String (pin) + F (" to ") +
                         ((pin) ? F (CMD_GPIO_OUT_HIGH_STR) : F (CMD_GPIO_OUT_LOW_STR))).c_str ());
 # ifdef OldWay
                updateUiGpioMsg (pin, ControllerName, NewPinState);
 # endif // def OldWay
        } while (false);

        // DEBUG_END;

        return response;

#endif // def OldWay

    return false;
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
#ifdef OldWay
        // DEBUG_START;
        bool response = true;

        if (payloadStr.length () > CMD_SYS_MAX_SZ)
        {
            payloadStr = payloadStr.substring (0, CMD_SYS_MAX_SZ);
        }

        if (!payloadStr.equals (F (CMD_SYS_CODE_STR)))
        {
            Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Info Command")).c_str ());
        }
        else
        {
            Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid INFO Payload (") + payloadStr + F (
"), Ignored.")).c_str ());
            response = false;
        }
        // DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
// logCmd(): Set the Serial Log Level to Silent or reset back to system (Web UI) setting.
// This command is only used by the Serial Controller; The MQTT and HTTP controllers do not observe this command.
bool cCommandProcessor::log (String & payloadStr, String & ResponseMessage)
{
#ifdef OldWay
        // DEBUG_START;

        bool response = true;

        do  // once
        {
            if (payloadStr.length () > CMD_LOG_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_LOG_MAX_SZ);
            }

            if (payloadStr == CMD_LOG_SIL_STR)
            {
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Log Level Set to LOG_LEVEL_SILENT.")).c_str ());
                Serial.flush ();
                Log.begin (LOG_LEVEL_SILENT, & Serial);
                break;
            }

            if (payloadStr == CMD_LOG_RST_STR)
            {
                String logLevelStr; // TBD
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Log Level Restored to ") + logLevelStr).c_str ());
                Serial.flush ();
                Log.begin (getLogLevel (), & Serial);
                break;
            }
            Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid LOG Payload (") + payloadStr + F (
"), Ignored.")).c_str ());
            response = false;
        } while (false);

        Log.setShowLevel (false);   // Do not show loglevel, we will do this in the prefix
        Serial.flush ();

        // DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::mute (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = AudioMute.set (payloadStr, ResponseMessage);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::piCode (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = PiCode.set (payloadStr, ResponseMessage);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::ptyCode (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = PtyCode.set (payloadStr, ResponseMessage);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::programServiceName (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = ProgramServiceName.set (payloadStr, ResponseMessage);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::radioText (String & payloadStr, String & ResponseMessage)
{
#ifdef OldWay
        // DEBUG_START;

        bool response = true;

        do  // once
        {
            if (payloadStr.length () > CMD_RT_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_RT_MAX_SZ);
            }
            // ControllerMgr.SetPayloadText(controller, payloadStr);
            // ControllerMgr.SetTextFlag(controller, true);

            // displaySaveWarning();
            Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: RadioText Changed to ") + payloadStr).c_str ());
        } while (false);

        // DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
// rdsTimePeriodCmd(): Set the RadioText Message Display Time. Input value is in seconds.
bool cCommandProcessor::rdsTimePeriod (String & payloadStr, String & ResponseMessage)
{
#ifdef OldWay
        // DEBUG_START;
        bool    response    = true;
        bool    capFlg      = false;
        int32_t rtTime      = 0;

        do  // once
        {
            if (payloadStr.length () > CMD_RT_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_RT_MAX_SZ);
            }
            rtTime = strtol (payloadStr.c_str (), NULL, 10);

            if ((payloadStr.length () > CMD_TIME_MAX_SZ) || (rtTime <= 0))
            {
                Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: RDS Time Period Value is Invalid, Ignored.")).c_str ());
                response = false;
                break;
            }

            if (rtTime > RDS_DSP_TM_MAX)
            {
                capFlg  = true;
                rtTime  = RDS_DSP_TM_MAX;
            }
            else if (rtTime < RDS_DSP_TM_MIN)
            {
                capFlg  = true;
                rtTime  = RDS_DSP_TM_MIN;
            }
            // ControllerMgr.SetPayloadText(controller, payloadStr);
            // ControllerMgr.SetTextFlag(controller, true);
            // ControllerMgr.SetRdsMsgTime(controller, rtTime * 1000);
            String Msg;

            if (capFlg)
            {
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: RDS Time Period Value out-of-range, set to ") +
                                String (rtTime) + " secs.").c_str ());
            }
            else
            {
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: RDS Time Period Value set to ") + String (rtTime) +
                                " secs.").c_str ());
            }
        } while (false);

        // DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::reboot (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = false;  // RebootControl.set(payloadStr, ResponseMessage);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::rfCarrier (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = RfCarrier.set (payloadStr, ResponseMessage);

    // DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::start (String & payloadStr, String & ResponseMessage)
{
#ifdef OldWay
        // DEBUG_START;
        bool response = true;

        if (payloadStr.length () > CMD_RDS_MAX_SZ)
        {
            payloadStr = payloadStr.substring (0, CMD_RDS_MAX_SZ);
        }

        if (payloadStr.equals (F (CMD_RDS_CODE_STR)))
        {
            Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Start RDS.")).c_str ());
            // ControllerMgr.SetTextFlag(controller, true);
        }
        else
        {
            Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid START Payload (") + payloadStr + F (
"), Ignored.")).c_str ());
            response = false;
        }
        // DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::stop (String & payloadStr, String & ResponseMessage)
{
#ifdef OldWay
        // DEBUG_START;
        bool response = true;

        if (payloadStr.length () > CMD_RDS_MAX_SZ)
        {
            payloadStr = payloadStr.substring (0, CMD_RDS_MAX_SZ);
        }

        if (payloadStr.equals (F (CMD_RDS_CODE_STR)))
        {
            Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Stop RDS.")).c_str ());
            // ControllerMgr.SetStopFlag(controller, true);
        }
        else
        {
            Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid STOP Payload (") + payloadStr + F (
"), Ignored.")).c_str ());
        }
        // DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
// EOF
