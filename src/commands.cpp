/*
   File: commands.cpp
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
#include "Commands.h"
#include "memdebug.h"
#include "language.h"
#include "radio.hpp"

// *************************************************************************************************************************
CommandProcessor::CommandProcessor()
{
}

// *************************************************************************************************************************
bool CommandProcessor::ProcessCommand(String &RawCommand, String & ControllerName)
{
    DEBUG_START;
    bool response = false;

    String cmd;
    String payloadStr;
    payloadStr.trim();
    payloadStr.toLowerCase();

    do // once
    {
        if(cmd.equals(F("aud")))    {response = audioMode           (payloadStr, ControllerName); break;}
        if(cmd.equals(F("freq")))   {response = frequency           (payloadStr, ControllerName); break;}
        if(cmd.equals(F("gpio19"))) {response = gpio19              (payloadStr, ControllerName); break;}
        if(cmd.equals(F("gpio23"))) {response = gpio23              (payloadStr, ControllerName); break;}
        if(cmd.equals(F("gpio33"))) {response = gpio33              (payloadStr, ControllerName); break;}
        if(cmd.equals(F("mute")))   {response = mute                (payloadStr, ControllerName); break;}
        if(cmd.equals(F("pic")))    {response = piCode              (payloadStr, ControllerName); break;}
        if(cmd.equals(F("rtper")))  {response = rdsTimePeriod       (payloadStr, ControllerName); break;}
        if(cmd.equals(F("psn")))    {response = programServiceName  (payloadStr, ControllerName); break;}
        if(cmd.equals(F("pty")))    {response = ptyCode             (payloadStr, ControllerName); break;}
        if(cmd.equals(F("rfc")))    {response = rfCarrier           (payloadStr, ControllerName); break;}
        if(cmd.equals(F("rtm")))    {response = radioText           (payloadStr, ControllerName); break;}
        if(cmd.equals(F("start")))  {response = start               (payloadStr, ControllerName); break;}
        if(cmd.equals(F("stop")))   {response = stop                (payloadStr, ControllerName); break;}
    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
// AudioModeCmd(): Set the Mono/Stereo Audio Mode using the Payload String. On exit, return true if success.
bool CommandProcessor::audioMode(String & payloadStr, String & ControllerName)
{
    DEBUG_START;
    bool response = true;
    bool stereoEnbFlg = true;

    do // once
    {
        if (payloadStr.equals(CMD_MODE_STER_STR))
        {
            stereoEnbFlg = true;
        }
        else if (payloadStr.equals(CMD_MODE_MONO_STR))
        {
            stereoEnbFlg = false;
        }
        else 
        {
            Log.errorln((String(F("->")) + ControllerName + F(": Invalid Audio Mode Payload '") + payloadStr + F("'")).c_str());
            response = false;
            break;
        }
        updateUiAudioMode(stereoEnbFlg);
        // updateRadioAudioMode(stereoEnbFlg);

        Log.verboseln(
            (String(F("->")) + 
            ControllerName + 
            F(": Audio Mode Set to ") + 
            ((stereoEnbFlg) ? F("'STEREO'") : F("'MONO'") )).c_str());

    } while (false);

    DEBUG_END;

    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::frequency(String & payloadStr, String & ControllerName)
{
    DEBUG_START;
    bool response = true;

    do // once
    {
        if (payloadStr.length() > CMD_FREQ_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_FREQ_MAX_SZ);
        }

        int freq = payloadStr.toInt();

        if ((!strIsUint(payloadStr)) ||
            (freq < FM_FREQ_MIN_X10) || 
            (freq > FM_FREQ_MAX_X10)) 
        {
            Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid Radio Frequency Payload (") + payloadStr + F("'")).c_str());
            response = false;
            break;
        }

        updateUiFrequency(freq);
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: Transmit Frequency Set to ") + String(((float(freq)) / 10.0f), 1) + F("Mhz")).c_str());

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::gpio19(String & payloadStr, String & ControllerName)
{
    return gpio(payloadStr, ControllerName, GPIO19_PIN);
}

// *************************************************************************************************************************
bool CommandProcessor::gpio23(String & payloadStr, String & ControllerName)
{
    return gpio(payloadStr, ControllerName, GPIO23_PIN);
}

// *************************************************************************************************************************
bool CommandProcessor::gpio33(String & payloadStr, String & ControllerName)
{
    return gpio(payloadStr, ControllerName, GPIO33_PIN);
}

// *************************************************************************************************************************
// gpioCmd(): Read/Write the User's GPIO Pin States.  On exit, return true if success.
bool CommandProcessor::gpio(String & payloadStr, String & ControllerName, gpio_num_t pin)
{
    DEBUG_START;

    bool response = true;

    do // once
    {
        if (payloadStr.length() > CMD_GPIO_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_GPIO_MAX_SZ);
        }

        if (payloadStr.equals(F(CMD_GPIO_READ_STR)))
        {
            Log.infoln((String(F("-> ")) + ControllerName + F(" Controller: Read GPIO Pin-") + String(pin) + F(", Value= ") + String(digitalRead(pin))).c_str());
            break;
        }

        if (!payloadStr.equals(CMD_GPIO_OUT_HIGH_STR) &&
            !payloadStr.equals(CMD_GPIO_OUT_LOW_STR))
        {
            Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid GPIO Payload (") + payloadStr + F("), Ignored.")).c_str());
            response = false;
            break;
        }

        bool NewPinState = payloadStr.equals(CMD_GPIO_OUT_HIGH_STR) ? HIGH : LOW;
        digitalWrite(pin, NewPinState);
        Log.infoln((String(F("-> ")) + ControllerName + F(" Controller: Setting GPIO Pin-") + String(pin) + F(" to ") + ((pin) ? F(CMD_GPIO_OUT_HIGH_STR) : F(CMD_GPIO_OUT_LOW_STR))).c_str());
        updateUiGpioMsg(pin, ControllerName, NewPinState);

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::info(String & payloadStr, String & ControllerName)
{
    DEBUG_START;
    bool response = true;

    if (payloadStr.length() > CMD_SYS_MAX_SZ)
    {
        payloadStr = payloadStr.substring(0, CMD_SYS_MAX_SZ);
    }

    if (!payloadStr.equals(F(CMD_SYS_CODE_STR)))
    {
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: Info Command")).c_str());
    }
    else 
    {
        Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid INFO Payload (") + payloadStr + F("), Ignored.")).c_str());
        response = false;
    }

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
// logCmd(): Set the Serial Log Level to Silent or reset back to system (Web UI) setting.
// This command is only used by the Serial Controller; The MQTT and HTTP controllers do not observe this command.
bool CommandProcessor::log(String & payloadStr, String & ControllerName)
{
    DEBUG_START;

    bool response = true;

    do // once
    {
        if (payloadStr.length() > CMD_LOG_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_LOG_MAX_SZ);
        }

        if (payloadStr == CMD_LOG_SIL_STR) 
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: Log Level Set to LOG_LEVEL_SILENT.")).c_str());
            Serial.flush();
            Log.begin(LOG_LEVEL_SILENT, &Serial);
            break;
        }

        if (payloadStr == CMD_LOG_RST_STR) 
        {
            String logLevelStr; // TBD
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: Log Level Restored to ") + logLevelStr).c_str());
            Serial.flush();
            Log.begin(getLogLevel(), &Serial);
            break;
        }

        Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid LOG Payload (") + payloadStr + F("), Ignored.")).c_str());
        response = false;

    } while (false);

    Log.setShowLevel(false); // Do not show loglevel, we will do this in the prefix
    Serial.flush();

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::mute(String & payloadStr, String & ControllerName)
{
    DEBUG_START;

    bool response = true;

    do // once
    {
        if (payloadStr.length() > CMD_MUTE_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_MUTE_MAX_SZ);
        }

        if (payloadStr.equals(CMD_MUTE_ON_STR))
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: Mute Set to ON (Audio Off).")).c_str());
            // muteFlg    = true;
            updateUiAudioMute(true);
            break;
        }

        if (payloadStr == CMD_MUTE_OFF_STR) 
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: Mute Set to OFF (Audio On).")).c_str());
            // muteFlg    = false;
            updateUiAudioMute(false);
            break;
        }

        Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid MUTE Payload (") + payloadStr + F("), Ignored.")).c_str());
        response = false;

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::piCode(String & payloadStr, String & ControllerName)
{
   DEBUG_START;

    bool response = true;
    uint32_t tempPiCode;

    do // once
    {
        if (payloadStr.length() > CMD_PI_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_PI_MAX_SZ);
        }

        tempPiCode = strtol(payloadStr.c_str(), NULL, HEX);

        if ((tempPiCode < RDS_PI_CODE_MIN) || (tempPiCode > RDS_PI_CODE_MAX)) 
        {
            Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid RDS PI Code Value (") + payloadStr + F("), Ignored.")).c_str());
            response = false;
            break;
        }
#ifdef OldWay
        if (radio.getPiCode() == (uint16_t)(tempPiCode))
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: PI Code Unchanged (0x") + String(tempPiCode, HEX) + F(").")).c_str());
            break;
        }
#endif // def OldWay

        // New PI Code.
        // Radio.SetPiCode(controller, tempPiCode);
        // ControllerMgr.SetTextFlag(controller, true);

        // displaySaveWarning();
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: PI Code Set to (0x") + String(tempPiCode, HEX) + F(").")).c_str());

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::ptyCode(String & payloadStr, String & ControllerName)
{
   DEBUG_START;

    bool response = true;
    uint32_t tempPtyCode;

    do // once
    {
        if (payloadStr.length() > CMD_PTY_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_PTY_MAX_SZ);
        }

        tempPtyCode = payloadStr.toInt();

        if ((tempPtyCode < RDS_PTY_CODE_MIN) || (tempPtyCode > RDS_PTY_CODE_MAX)) 
        {
            Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid RDS PTY Code Value (") + payloadStr + F("), Ignored.")).c_str());
            response = false;
            break;
        }

#ifdef OldWay
        if (radio.getPTYCode() == (uint8_t)(tempPtyCode))
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: PTY Code Unchanged (0x") + String(tempPtyCode, HEX) + F(").")).c_str());
            break;
        }
#endif // def OldWay

        // New PTY Code.
        // ControllerMgr.SetPtyCode(controller, tempPtyCode);
        // ControllerMgr.SetTextFlag(controller, true);

        // displaySaveWarning();
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: PTY Code Set to (0x") + String(tempPtyCode, HEX) + F(").")).c_str());

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::programServiceName(String & payloadStr, String & ControllerName)
{
   DEBUG_START;

    bool response = true;

    do // once
    {
        if (payloadStr.length() > CMD_PSN_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_PSN_MAX_SZ);
        }

        // ControllerMgr.SetRdsProgramServiceName(controller, payloadStr);
        // ControllerMgr.SetTextFlag(controller, true);

        // displaySaveWarning();
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: RDS PSN Set to ") + payloadStr).c_str());

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::radioText(String & payloadStr, String & ControllerName)
{
    DEBUG_START;

    bool response = true;

    do // once
    {
        if (payloadStr.length() > CMD_RT_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_RT_MAX_SZ);
        }

        // ControllerMgr.SetPayloadText(controller, payloadStr);
        // ControllerMgr.SetTextFlag(controller, true);

        // displaySaveWarning();
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: RadioText Changed to ") + payloadStr).c_str());

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
// rdsTimePeriodCmd(): Set the RadioText Message Display Time. Input value is in seconds.
bool CommandProcessor::rdsTimePeriod(String & payloadStr, String & ControllerName)
{
    DEBUG_START;
    bool response = true;
    bool capFlg = false;
    int32_t rtTime = 0;

    do // once
    {
        if (payloadStr.length() > CMD_RT_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_RT_MAX_SZ);
        }

        rtTime = strtol(payloadStr.c_str(), NULL, 10);

        if ((payloadStr.length() > CMD_TIME_MAX_SZ) || (rtTime <= 0)) 
        {
            Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: RDS Time Period Value is Invalid, Ignored.")).c_str());
            response = false;
            break;
        }

        if (rtTime > RDS_DSP_TM_MAX) 
        {
            capFlg = true;
            rtTime = RDS_DSP_TM_MAX;
        }
        else if (rtTime < RDS_DSP_TM_MIN) 
        {
            capFlg = true;
            rtTime = RDS_DSP_TM_MIN;
        }

        // ControllerMgr.SetPayloadText(controller, payloadStr);
        // ControllerMgr.SetTextFlag(controller, true);
        // ControllerMgr.SetRdsMsgTime(controller, rtTime * 1000);
        String Msg;
        if (capFlg)
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: RDS Time Period Value out-of-range, set to ") + String(rtTime) + " secs.").c_str());
        }
        else 
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: RDS Time Period Value set to ") + String(rtTime) + " secs.").c_str());
        }

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::reboot(String & payloadStr, String & ControllerName)
{
    DEBUG_START
    bool response = true;

    if (payloadStr.length() > CMD_SYS_MAX_SZ)
    {
        payloadStr = payloadStr.substring(0, CMD_SYS_MAX_SZ);
    }

    if (payloadStr.equals(F(CMD_SYS_CODE_STR)))
    {
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: System Rebooted.")).c_str());
        // rebootFlg = true; // Request system reboot.
    }
    else 
    {
        Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid REBOOT Payload (") + payloadStr + F("), Ignored.")).c_str());
        response = false;
    }

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::rfCarrier(String & payloadStr, String & ControllerName)
{
    DEBUG_START;

    bool response = true;

    do // once
    {
        if (payloadStr.length() > CMD_RF_MAX_SZ) 
        {
            payloadStr = payloadStr.substring(0, CMD_RF_MAX_SZ);
        }

        if (payloadStr.equals(CMD_RF_ON_STR))
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: RF Carrier Set to ON.")).c_str());
            // Radio.setRfCarrier(true);
            updateUiRfCarrier();
            break;
        }

        if (payloadStr == CMD_RF_OFF_STR) 
        {
            Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: RF Carrier Set to OFF.")).c_str());
            // rfCarrierFlg  = false;
            // newCarrierFlg = true;
            updateUiRfCarrier();
            break;
        }

        Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid RF Carrier Payload (") + payloadStr + F("), Ignored.")).c_str());
        response = false;

    } while (false);

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::start(String & payloadStr, String & ControllerName)
{
    DEBUG_START;
    bool response = true;

    if (payloadStr.length() > CMD_RDS_MAX_SZ)
    {
        payloadStr = payloadStr.substring(0, CMD_RDS_MAX_SZ);
    }

    if (payloadStr.equals(F(CMD_RDS_CODE_STR)))
    {
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: Start RDS.")).c_str());
        // ControllerMgr.SetTextFlag(controller, true);
    }
    else
    {
        Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid START Payload (") + payloadStr + F("), Ignored.")).c_str());
        response = false;
    }

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
bool CommandProcessor::stop(String & payloadStr, String & ControllerName)
{
    DEBUG_START;
    bool response = true;

    if (payloadStr.length() > CMD_RDS_MAX_SZ)
    {
        payloadStr = payloadStr.substring(0, CMD_RDS_MAX_SZ);
    }

    if (payloadStr.equals(F(CMD_RDS_CODE_STR)))
    {
        Log.verboseln((String(F("-> ")) + ControllerName + F(" Controller: Stop RDS.")).c_str());
        // ControllerMgr.SetStopFlag(controller, true);
    }
    else 
    {
        Log.errorln((String(F("-> ")) + ControllerName + F(" Controller: Invalid STOP Payload (") + payloadStr + F("), Ignored.")).c_str());
    }

    DEBUG_END;
    return response;
}

// *************************************************************************************************************************
// EOF
