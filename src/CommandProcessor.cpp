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
#include "CommandProcessor.hpp"
#include "language.h"
#include "memdebug.h"
#include "radio.hpp"
#include <map>

typedef bool (cCommandProcessor::*CmdHandler)(String &, String &, String &);
std::map <String, CmdHandler> ListOfCommands
{
    {
        "aud",     &cCommandProcessor::audioMode
    },
    {
        "freq",    &cCommandProcessor::frequency
    },
    {
        "gpio19",  &cCommandProcessor::gpio19
    },
    {
        "gpio23",  &cCommandProcessor::gpio23
    },
    {
        "gpio33",  &cCommandProcessor::gpio33
    },
    {
        "mute",    &cCommandProcessor::mute
    },
    {
        "pic",     &cCommandProcessor::piCode
    },
    {
        "rtper",   &cCommandProcessor::rdsTimePeriod
    },
    {
        "psn",     &cCommandProcessor::programServiceName
    },
    {
        "pty",     &cCommandProcessor::ptyCode
    },
    {
        "rfc",     &cCommandProcessor::rfCarrier
    },
    {
        "rtm",     &cCommandProcessor::radioText
    },
    {
        "start",   &cCommandProcessor::start
    },
    {
        "stop",    &cCommandProcessor::stop
    },
    {
        "?",       &cCommandProcessor::HelpCommand
    },
    {
        "h",       &cCommandProcessor::HelpCommand
    },
    {
        "help",    &cCommandProcessor::HelpCommand
    },
};

// *************************************************************************************************************************
cCommandProcessor::cCommandProcessor ()
{}

// *************************************************************************************************************************
bool cCommandProcessor::ProcessCommand (
    String      & RawCommand,
    String      & ControllerName,
    String      & Response)
{
    DEBUG_START;

    String      Command;
    String      Parameter;

    bool response = ProcessCommand (Command, Parameter, ControllerName, Response);

    DEBUG_END;

    return response;
}

// *************************************************************************************************************************
bool cCommandProcessor::ProcessCommand (
    String      & Command,
    String      & Parameter,
    String      & ControllerName,
    String      & Response)
{
    DEBUG_START;
    bool response = false;
    Response.reserve (1000);

    Command.trim ();
    Command.toLowerCase ();

    Parameter.trim ();

    do  // once
    {
        if (ListOfCommands.end () == ListOfCommands.find (Command))
        {
            Response = (String (F ("->ERROR: Unknown Command: '")) + Command + F ("'\n"));
            HelpCommand (Parameter, ControllerName, Response);
            response = false;
            break;
        }
        DEBUG_V ();
        Response += String (F ("->")) + ControllerName;
        (this->*ListOfCommands[Command])(Parameter, ControllerName, Response);
    } while (false);

    DEBUG_END;

    return response;
}

// *************************************************************************************************************************
// AudioModeCmd(): Set the Mono/Stereo Audio Mode using the Payload String. On exit, return true if success.
bool cCommandProcessor::audioMode (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;
        bool    response        = true;
        bool    stereoEnbFlg    = true;

        payloadStr.toLowerCase ();

        do      // once
        {
            if (payloadStr.equals (F ("stereo")))
            {
                Response += F (": Audio Mode Set to 'STEREO'\n");
                Radio.setMonoAudio (false);
                break;
            }

            if (payloadStr.equals (F ("mono")))
            {
                Response += F (": Audio Mode Set to 'MONO'\n");
                Radio.setMonoAudio (true);
                break;
            }
            Response    += String (F (": Invalid Audio Mode Parameter '")) + payloadStr + F ("'\n");
            response    = false;
            break;
        } while (false);

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::frequency (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;
        bool response = true;

        do      // once
        {
            int freq = payloadStr.toInt ();

            if ((!strIsUint (payloadStr)) ||
                (freq < FM_FREQ_MIN_X10) ||
                (freq > FM_FREQ_MAX_X10))
            {
                Response        += String (F (": Invalid Radio Frequency Parameter ")) + payloadStr + F ("'\n");
                response        = false;
                break;
            }
            setFrequency (freq);

            Response += String (F (": Transmit Frequency Set to ")) + String (((float(freq)) / 10.0f), 1) + F ("Mhz\n");
        } while (false);

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio19 (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay

        return gpio (payloadStr, ControllerName, GPIO19_PIN, Response);

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio23 (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay

        return gpio (payloadStr, ControllerName, GPIO23_PIN, Response);

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::gpio33 (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay

        return gpio (payloadStr, ControllerName, GPIO33_PIN, Response);

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
// gpioCmd(): Read/Write the User's GPIO Pin States.  On exit, return true if success.
bool cCommandProcessor::gpio (String & payloadStr, String & ControllerName, gpio_num_t pin, String & Response)
{
#ifdef OldWay
        DEBUG_START;

        bool response = true;

        do      // once
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

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// ================================================================================================
// CommandError(): Create an error response.
bool cCommandProcessor::HelpCommand (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;

        Response        += ("\n");
        Response        += ("=========================================\n");
        Response        += ("**      CONTROLLER COMMAND SUMMARY     **\n");
        Response        += ("=========================================\n");
        Response        += (" AUDIO MODE      : aud=mono : stereo\n");
        Response        += String (F (" FREQUENCY X10   : freq=")) + String (FM_FREQ_MIN_X10) + F ("<->") + String (FM_FREQ_MAX_X10) +
                           F ("\n");
        Response        += (" GPIO-19 CONTROL : gpio19=read : outhigh : outlow\n");
        Response        += (" GPIO-23 CONTROL : gpio23=read : outhigh : outlow\n");
        Response        += (" GPIO-33 CONTROL : gpio33=read : outhigh : outlow\n");
        Response        += (" INFORMATION     : info=system\n");
        Response        += (" MUTE AUDIO      : mute=on : off\n");
        Response        += String (" PROG ID CODE    : pic=0x") +
                           String (RDS_PI_CODE_MIN, HEX) + " <-> 0x" + String (RDS_PI_CODE_MAX, HEX) + F ("\n");
        Response        += String (" PROG SERV NAME  : psn=[") + String (CMD_PSN_MAX_SZ) + F (" char name]\n");
        Response        += String (" RADIOTXT MSG    : rtm=[") + String (CMD_RT_MAX_SZ) + F (" char message]\n");
        Response        += String (" RADIOTXT PERIOD : rtper=5 <-> ") + String (RDS_DSP_TM_MAX) + F (" secs\n");
        Response        += (" REBOOT SYSTEM   : reboot=system\n");
        Response        += (" START RDS       : start=rds\n");
        Response        += (" STOP RDS        : stop=rds\n");
        Response        += (" LOG CONTROL     : log=silent : restore\n");
        Response        += (" HELP            : ?  h  help\n");
        Response        += ("=========================================\n");
        Response        += ("\n");

        DEBUG_END;

        return true;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::info (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;
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
        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
// logCmd(): Set the Serial Log Level to Silent or reset back to system (Web UI) setting.
// This command is only used by the Serial Controller; The MQTT and HTTP controllers do not observe this command.
bool cCommandProcessor::log (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;

        bool response = true;

        do      // once
        {
            if (payloadStr.length () > CMD_LOG_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_LOG_MAX_SZ);
            }

            if (payloadStr == CMD_LOG_SIL_STR)
            {
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Log Level Set to LOG_LEVEL_SILENT.")).c_str ());
                Serial.flush ();
                Log.begin (LOG_LEVEL_SILENT, &Serial);
                break;
            }

            if (payloadStr == CMD_LOG_RST_STR)
            {
                String logLevelStr;     // TBD
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Log Level Restored to ") + logLevelStr).c_str ());
                Serial.flush ();
                Log.begin (getLogLevel (), &Serial);
                break;
            }
            Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid LOG Payload (") + payloadStr + F (
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      "), Ignored.")).c_str ());
            response = false;
        } while (false);

        Log.setShowLevel (false);       // Do not show loglevel, we will do this in the prefix
        Serial.flush ();

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::mute (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;

        bool response = true;

        do      // once
        {
            if (payloadStr.length () > CMD_MUTE_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_MUTE_MAX_SZ);
            }

            if (payloadStr.equals (CMD_MUTE_ON_STR))
            {
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Mute Set to ON (Audio Off).")).c_str ());
                // muteFlg    = true;
                updateUiAudioMute (true);
                break;
            }

            if (payloadStr == CMD_MUTE_OFF_STR)
            {
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: Mute Set to OFF (Audio On).")).c_str ());
                // muteFlg    = false;
                updateUiAudioMute (false);
                break;
            }
            Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid MUTE Payload (") + payloadStr + F (
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      "), Ignored.")).c_str ());
            response = false;
        } while (false);

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::piCode (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;

        bool response = true;
        uint32_t tempPiCode;

        do      // once
        {
            if (payloadStr.length () > CMD_PI_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_PI_MAX_SZ);
            }
            tempPiCode = strtol (payloadStr.c_str (), NULL, HEX);

            if ((tempPiCode < RDS_PI_CODE_MIN) || (tempPiCode > RDS_PI_CODE_MAX))
            {
                Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid RDS PI Code Value (") + payloadStr +
                              F ("), Ignored.")).c_str ());
                response = false;
                break;
            }
 # ifdef OldWay

                if (radio.getPiCode () == (uint16_t)(tempPiCode))
                {
                    Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: PI Code Unchanged (0x") +
                                    String (tempPiCode, HEX) + F (").")).c_str ());
                    break;
                }
 # endif // def OldWay

            // New PI Code.
            // Radio.SetPiCode(controller, tempPiCode);
            // ControllerMgr.SetTextFlag(controller, true);

            // displaySaveWarning();
            Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: PI Code Set to (0x") + String (tempPiCode, HEX) + F (
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ").")).c_str ());
        } while (false);

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::ptyCode (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;

        bool response = true;
        uint32_t tempPtyCode;

        do      // once
        {
            if (payloadStr.length () > CMD_PTY_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_PTY_MAX_SZ);
            }
            tempPtyCode = payloadStr.toInt ();

            if ((tempPtyCode < RDS_PTY_CODE_MIN) || (tempPtyCode > RDS_PTY_CODE_MAX))
            {
                Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid RDS PTY Code Value (") + payloadStr +
                              F ("), Ignored.")).c_str ());
                response = false;
                break;
            }
 # ifdef OldWay

                if (radio.getPTYCode () == (uint8_t)(tempPtyCode))
                {
                    Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: PTY Code Unchanged (0x") +
                                    String (tempPtyCode, HEX) + F (").")).c_str ());
                    break;
                }
 # endif // def OldWay

            // New PTY Code.
            // ControllerMgr.SetPtyCode(controller, tempPtyCode);
            // ControllerMgr.SetTextFlag(controller, true);

            // displaySaveWarning();
            Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: PTY Code Set to (0x") +
                            String (tempPtyCode, HEX) + F (").")).c_str ());
        } while (false);

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::programServiceName (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;
        bool response = true;

        do      // once
        {
            if (payloadStr.length () > CMD_PSN_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_PSN_MAX_SZ);
            }
            // ControllerMgr.SetRdsProgramServiceName(controller, payloadStr);
            // ControllerMgr.SetTextFlag(controller, true);

            // displaySaveWarning();
            Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: RDS PSN Set to ") + payloadStr).c_str ());
        } while (false);

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::radioText (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;

        bool response = true;

        do      // once
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

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
// rdsTimePeriodCmd(): Set the RadioText Message Display Time. Input value is in seconds.
bool cCommandProcessor::rdsTimePeriod (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;
        bool    response        = true;
        bool    capFlg          = false;
        int32_t rtTime          = 0;

        do      // once
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

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::reboot (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START
        bool response = true;

        if (payloadStr.length () > CMD_SYS_MAX_SZ)
        {
            payloadStr = payloadStr.substring (0, CMD_SYS_MAX_SZ);
        }

        if (payloadStr.equals (F (CMD_SYS_CODE_STR)))
        {
            Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: System Rebooted.")).c_str ());
            // rebootFlg = true; // Request system reboot.
        }
        else
        {
            Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid REBOOT Payload (") + payloadStr + F (
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      "), Ignored.")).c_str ());
            response = false;
        }
        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::rfCarrier (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;

        bool response = true;

        do      // once
        {
            if (payloadStr.length () > CMD_RF_MAX_SZ)
            {
                payloadStr = payloadStr.substring (0, CMD_RF_MAX_SZ);
            }

            if (payloadStr.equals (CMD_RF_ON_STR))
            {
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: RF Carrier Set to ON.")).c_str ());
                // Radio.setRfCarrier(true);
                updateUiRfCarrier ();
                break;
            }

            if (payloadStr == CMD_RF_OFF_STR)
            {
                Log.verboseln ((String (F ("-> ")) + ControllerName + F (" Controller: RF Carrier Set to OFF.")).c_str ());
                // rfCarrierFlg  = false;
                // newCarrierFlg = true;
                updateUiRfCarrier ();
                break;
            }
            Log.errorln ((String (F ("-> ")) + ControllerName + F (" Controller: Invalid RF Carrier Payload (") + payloadStr +
                          F ("), Ignored.")).c_str ());
            response = false;
        } while (false);

        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::start (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;
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
        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
bool cCommandProcessor::stop (String & payloadStr, String & ControllerName, String & Response)
{
#ifdef OldWay
        DEBUG_START;
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
        DEBUG_END;

        return response;

#endif // def OldWay

    return false;
}

// *************************************************************************************************************************
// EOF
