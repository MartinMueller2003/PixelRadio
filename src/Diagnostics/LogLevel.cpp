/*
  *    File: LogLevel.cpp
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
#include <map>

#include "LogLevel.hpp"
#include "memdebug.h"

#define DIAG_LOG_SILENT_STR  "LOG_LEVEL_SILENT (default)"
#define DIAG_LOG_FATAL_STR   "LOG_LEVEL_FATAL"
#define DIAG_LOG_ERROR_STR   "LOG_LEVEL_ERROR"
#define DIAG_LOG_WARN_STR    "LOG_LEVEL_WARNING"
#define DIAG_LOG_INFO_STR    "LOG_LEVEL_INFO"
#define DIAG_LOG_TRACE_STR   "LOG_LEVEL_TRACE"
#define DIAG_LOG_VERB_STR    "LOG_LEVEL_VERBOSE"
#define DIAG_LOG_DEF_STR      DIAG_LOG_VERB_STR

static const ChoiceListVector_t MapOfLogLevels
{
    {DIAG_LOG_SILENT_STR,   String (LOG_LEVEL_SILENT)},
    {DIAG_LOG_FATAL_STR,    String (LOG_LEVEL_FATAL)},
    {DIAG_LOG_ERROR_STR,    String (LOG_LEVEL_ERROR)},
    {DIAG_LOG_WARN_STR,     String (LOG_LEVEL_WARNING)},
    {DIAG_LOG_INFO_STR,     String (LOG_LEVEL_INFO)},
    {DIAG_LOG_TRACE_STR,    String (LOG_LEVEL_TRACE)},
    {DIAG_LOG_VERB_STR,     String (LOG_LEVEL_VERBOSE)},
};

std::vector <String> LogLevelPrefixes =
{
    {F ("{SILENT } ")},
    {F ("{FATAL  } ")},
    {F ("{ERROR  } ")},
    {F ("{WARNING} ")},
    {F ("{INFORM } ")},
    {F ("{TRACE  } ")},
    {F ("{VERBOSE} ")},
};

const PROGMEM char  ConfigName     []   = "LOG_LEVEL_STR";
const PROGMEM char  ControlTitle   []   = "SERIAL LOG LEVEL";

// *********************************************************************************************
cLogLevel::cLogLevel () :   cChoiceListControl (
        ConfigName,
        ControlTitle,
        DIAG_LOG_DEF_STR,
        & MapOfLogLevels)
{}

// *********************************************************************************************
void cLogLevel::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cChoiceListControl::AddControls (TabId, color);
    setControlPanelStyle (ePanelStyle::PanelStyle135_black);

    // DEBUG_END;
}

// *************************************************************************************************************************
void cLogLevel::begin ()
{
    // DEBUG_START;

    Log.setPrefix (printPrefix);    // set prefix similar to NLog
    Log.setSuffix (printSuffix);    // set suffix

    // Log.begin (LOG_LEVEL_VERBOSE, & Serial, false);
    Log.begin (LOG_LEVEL_INFO, & Serial, false);

    // DEBUG_END;
}

// *************************************************************************************************************************
void cLogLevel::printLogLevel (Print * _logOutput, int _logLevel)
{
    // DEBUG_START;

    _logOutput->print (LogLevelPrefixes[_logLevel].c_str ());

    // DEBUG_END;
}

// *************************************************************************************************************************
void cLogLevel::printPrefix (Print * _logOutput, int _logLevel)
{
    // DEBUG_START;

    LogLevel.printTimestamp (_logOutput);
    LogLevel.printLogLevel (_logOutput, _logLevel);

    // DEBUG_END;
}

// *************************************************************************************************************************
void cLogLevel::printSuffix (Print * _logOutput, int)
{
    // DEBUG_START;

    _logOutput->print ("");

    // DEBUG_END;
}

// *************************************************************************************************************************
void cLogLevel::printTimestamp (Print * _logOutput)
{
    // DEBUG_START;

    // Total time
    const unsigned long msecs   = millis ();
    const unsigned long secs    = msecs / MSECS_IN_SEC;

    // Time in components
    const unsigned long MiliSeconds = msecs % MSECS_IN_SEC;
    const unsigned long Seconds     = secs % SECS_IN_MIN;
    const unsigned long Minutes     = (secs / SECS_IN_MIN) % SECS_IN_MIN;
    const unsigned long Hours       = (secs % SECS_IN_DAY) / SECS_IN_HOUR;

    // Time as string
    char timestamp[20];

    snprintf (timestamp, sizeof (timestamp) - 1, "%02lu:%02lu:%02lu.%03lu ", Hours, Minutes, Seconds, MiliSeconds);
    _logOutput->print (timestamp);

    // DEBUG_END;
}

// *********************************************************************************************
bool cLogLevel::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    bool Response = cChoiceListControl::set (value, ResponseMessage, ForceUpdate);

    if (Response)
    {
        // DEBUG_V(String("      value: ") + value);
        // DEBUG_V(String("      get32: ") + String(get32()));
        // DEBUG_V(String("GetLogLevel: ") + String(Log.getLevel()));

        // Do not show loglevel, we will do this in the prefix
        Log.setLevel (get32 ());
        // DEBUG_V(String("GetLogLevel: ") + String(Log.getLevel()));
    }

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
cLogLevel LogLevel;

// *********************************************************************************************
// OEF
