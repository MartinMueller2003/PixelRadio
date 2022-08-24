/*
   File: SerialControl.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Apr-06-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
   absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

    Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include "SerialControl.hpp"
#include "memdebug.h"

// ================================================================================================
cSerialControl::cSerialControl () : cBaudrateControl ()
{
    cmdStr.reserve (40);    // Minimize memory re-allocations.
    paramStr.reserve (80);  // Minimize memory re-allocations.
}

// ================================================================================================
cSerialControl::~cSerialControl ()
{}

#ifdef OldWay
// ************************************************************************************************
void cSerialControl::SetSerialPort(HardwareSerial * port)
{
    // DEBUG_START;
    
    SerialPort = port;

    SerialPort->flush ();                   // Purge pending serial chars.
    SerialPort->end ();
#ifdef OldWay
    serial_manager.start ();                // Start Command Line Processor.
    serial_manager.setFlag (CMD_EOL_TERM);  // EOL Termination character.
    serial_manager.setDelimiter ('=');      // Parameter delimiter character.
#endif // def OldWay
    SerialPort->flush ();                   // Repeat the flushing.

    // DEBUG_END;
}
#endif // def OldWay

#ifdef OldWay

// ************************************************************************************************
void cSerialControl::CbBaudrateControl (Control * sender, int type)
{
    // DEBUG_START;
    uint32_t OriginalBaudrate = BaudrateControl.get32();

    // DEBUG_V(String("CbBaudrateControl ID: ") + String(sender->id) + ", Value: " + sender->value);

    do  // once
    {
        if (false == SetBaudrate (sender->value))
        {
            // had an error
            Log.errorln ((String (F ("serialCallback: ")) + F (BAD_VALUE_STR)).c_str ());
            extern uint16_t diagLogMsgID;
            ESPUI.print (diagLogMsgID, ERROR_MSG_STR);  // Post warning message on Diagnostic Serial Log Panel.
            // DEBUG_V();
        }
    } while (false);
    // DEBUG_V();

    // DEBUG_END;
}
#endif // def OldWay

// ************************************************************************************************
void cSerialControl::serialCommands (void)
{
    // _ DEBUG_START;
#ifdef OldWay

    do  // once
    {
        if (!ControllerIsEnabled())
        {
            // DEBUG_V("Serial Controller disabled, nothing to do. Exit.");
            break;
        }

        while (serial_manager.onReceive ())
        {
            // Process any serial commands from user (CLI).
            cmdStr = serial_manager.getCmd ();
            // serial_manager.println((String(F("Raw CMD Parameter: '")) + cmdStr + "'").c_str());

            paramStr = serial_manager.getParam ();
            // serial_manager.println((String(F("Raw CLI Parameter: '")) + paramStr + "'").c_str());

            String Response;
            CommandProcessor.ProcessCommand (cmdStr, paramStr, Title, Response);
            serial_manager.print (Response);
            // DEBUG_V(String("Response.length: ") + String(Response.length()));
        }
    } while (false);
#endif // def OldWay

    // _ DEBUG_END;
}

// ************************************************************************************************
bool cSerialControl::SetBaudrate (String NewRate)
{
    // DEBUG_START;
    bool Response               = true;
    uint32_t OriginalBaudrate   = get32();
#ifdef OldWay
    do  // once
    {
        if (NewRate == SERIAL_096_STR)
        {
            // DEBUG_V(SERIAL_096_STR);
            BaudRateStr = SERIAL_096_STR;
            BaudRate    = 9600;
            break;
        }

        if (NewRate == SERIAL_192_STR)
        {
            // DEBUG_V(SERIAL_192_STR);
            BaudRateStr = SERIAL_192_STR;
            BaudRate    = 19200;
            break;
        }

        if (NewRate == SERIAL_576_STR)
        {
            // DEBUG_V(SERIAL_576_STR);
            BaudRateStr = SERIAL_576_STR;
            BaudRate    = 57600;
            break;
        }

        if (NewRate == SERIAL_115_STR)
        {
            // DEBUG_V(SERIAL_115_STR);
            BaudRateStr = SERIAL_115_STR;
            BaudRate    = 115200;
            break;
        }
        // DEBUG_V("Not a valid string");
        Response                = false;
        // ControllerIsEnabled()       = false;

        // DEBUG_V();
    } while (false);
    // DEBUG_V();

    if ((OriginalBaudrate != BaudRate) && (ControllerIsEnabled()))
    {
        // DEBUG_V("Change Baudrate");
        Serial.end ();  // Flush all characters in queue.
        Serial.begin (BaudRate);

        while (!Serial && !Serial.available ())
        {}                      // Wait for Serial Port to be available.
        Serial.println ();      // Push out any corrupted data due to baud change.
        // DEBUG_V("Change Baudrate - Done");
    }
#endif // def OldWay

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
// EOF
