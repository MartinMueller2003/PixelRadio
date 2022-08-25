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

// ************************************************************************************************
cSerialControl::~cSerialControl ()
{}

// ************************************************************************************************
void cSerialControl::AddControls (uint16_t TabId, ControlColor color)
{
 // DEBUG_START;

    cBaudrateControl::AddControls(TabId, color);

    String Temp = F("Serial Control<br>Last Command Processed");
    LastCmdProcessed.SetTitle(Temp);
    LastCmdProcessed.AddControls(TabId, color);
    LastCmdProcessed.setControlStyle(eCssStyle::CssStyleWhite);
    LastCmdProcessed.set(Temp);

    // DEBUG_END;
}

// ************************************************************************************************
void cSerialControl::initSerialControl(HardwareSerial * port)
{
    // DEBUG_START;

    SerialPort = port;

    SerialPort->flush ();                   // Purge pending serial chars.
    SerialPort->end ();

    serial_manager.start (*port);            // Start Command Line Processor.
    serial_manager.setFlag (CMD_EOL_TERM);  // EOL Termination character.
    serial_manager.setDelimiter ('=');      // Parameter delimiter character.

    SerialPort->flush ();                   // Purge pending serial chars.
    SerialPort->end ();

    SerialPort->begin (get32());            // set the baudrate
    SerialPort->flush ();                   // Repeat the flushing.

    // DEBUG_END;
}

// ************************************************************************************************
void cSerialControl::poll (void)
{
    // _ DEBUG_START;

    do  // once
    {
        if (!ControllerIsEnabled)
        {
            // DEBUG_V("Serial Controller disabled, nothing to do. Exit.");
            break;
        }

        while (serial_manager.onReceive ())
        {
         // DEBUG_V("Process any serial commands from user (CLI).");
            cmdStr = serial_manager.getCmd();
         // DEBUG_V((String(F("Raw CMD Parameter: '")) + cmdStr + "'").c_str());

            paramStr = serial_manager.getParam ();
         // DEBUG_V((String(F("Raw CLI Parameter: '")) + paramStr + "'").c_str());

            LastCmdProcessed.set(String(F("Command: '")) + cmdStr + F("' <br>Parameter: '") + paramStr + F("'") );

            String Response;
            CommandProcessor.ProcessCommand (cmdStr, paramStr, Response);
            Response += F("\n");
            serial_manager.print(Response);
            // DEBUG_V(String("Response.length: ") + String(Response.length()));
        }
    } while (false);

    // _ DEBUG_END;
}

// ************************************************************************************************
bool cSerialControl::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
 // DEBUG_START;
    uint32_t OriginalBaudrate = get32();
    bool Response = true;

    do // once
    {
        Response = cChoiceListControl::set(value, ResponseMessage, ForceUpdate);
        if(!ForceUpdate && !Response)
        {
         // DEBUG_V("Data not valid");
            Response = false;
            break;
        }

        if (OriginalBaudrate == get32())
        {
         // DEBUG_V("Dont interrupt the port");
            break;
        }

     // DEBUG_V("Change Baudrate");
        SerialPort->flush ();             // Flush all characters in queue.
        SerialPort->end ();             // Flush all characters in queue.
        SerialPort->begin (get32());    // set the new baudrate
        SerialPort->println ();         // Push out any corrupted data due to baud change.
     // DEBUG_V();

    } while (false);

 // DEBUG_END;
    return Response;
}

// *********************************************************************************************
// EOF
