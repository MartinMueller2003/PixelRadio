/*
   File: ControllerUsbSERIAL.cpp
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
#include <ArduinoLog.h>
#include "ControllerUsbSERIAL.h"
#include "language.h"
# include "memdebug.h"

static const PROGMEM String Name = F("USB SERIAL");
// ================================================================================================
c_ControllerUsbSERIAL::c_ControllerUsbSERIAL () : cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::USB_SERIAL_CNTRL)
{}      // c_ControllerUsbSERIAL

// ================================================================================================
c_ControllerUsbSERIAL::~c_ControllerUsbSERIAL ()
{}

// ************************************************************************************************
void c_ControllerUsbSERIAL::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cControllerCommon::AddControls (TabId, color);
    BaudrateControl.AddControls (ControlId, color);
    /*
    #ifdef OldWay
            extern String logLevelStr;
            EspuiMsgId = ESPUI.addControl (ControlType::Label,
                                           "SERIAL_MSG",
                                           (logLevelStr.equals (F (DIAG_LOG_SILENT_STR))) ? F (CTLR_SERIAL_MSG_STR) : emptyString,
                                           ControlColor::Turquoise,
                                           ControlLabelElementId);
            ESPUI.setElementStyle (EspuiMsgId, CSS_LABEL_STYLE_BLACK);
    #endif // def OldWay
    */
    // DEBUG_END;
}       // AddControls

// ************************************************************************************************
void c_ControllerUsbSERIAL::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response)
{
    if (ControllerIsEnabled())
    {
        Messages.GetNextRdsMessage (Response);
    }
}

// ************************************************************************************************
void c_ControllerUsbSERIAL::initSerialControl (void)
{
    // DEBUG_START;

    cmdStr.reserve (40);                        // Minimize memory re-allocations.
    paramStr.reserve (80);                      // Minimize memory re-allocations.

    Serial.flush ();                            // Purge pending serial chars.
    Serial.end ();
    serial_manager.start ();                    // Start Command Line Processor.
    serial_manager.setFlag (CMD_EOL_TERM);      // EOL Termination character.
    serial_manager.setDelimiter ('=');          // Parameter delimiter character.
    Serial.flush ();                            // Repeat the flushing.
    // Log.infoln(String(F("Serial Controller CLI is Enabled.")).c_str());

    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerUsbSERIAL::gpioSerialControl (String paramStr, uint8_t pin)
{
    // DEBUG_START;

    bool successFlg = false;
    String Response;

    // DEBUG_V(String("Serial Controller: Received GPIO Pin-") + String(pin) + " Command");
    // Log.infoln(charBuff);

#ifdef OldWay
        successFlg = gpioCmd (paramStr, TypeId, pin);
#endif // def OldWay

    Response = String (F ("{\"")) + CMD_GPIO_STR + String (pin) + F ("\": \"");

    if (!successFlg)
    {
        Response += F ("fail\"}");
    }
    else if (paramStr == CMD_GPIO_READ_STR)
    {
        Response += String (digitalRead (pin)) + F ("\"}");
    }
    else
    {
        Response += F ("ok\"}");
    }
    serial_manager.println (Response.c_str ());
    // DEBUG_END;
}

// ************************************************************************************************
void c_ControllerUsbSERIAL::CbBaudrateControl (Control * sender, int type)
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
}       // CbBaudrateControl

// ************************************************************************************************
void c_ControllerUsbSERIAL::serialCommands (void)
{
    // _ DEBUG_START;

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

    // _ DEBUG_END;
}

// ************************************************************************************************
bool c_ControllerUsbSERIAL::SetBaudrate (String NewRate)
{
    // DEBUG_START;
    bool Response               = true;
    uint32_t OriginalBaudrate   = BaudrateControl.get32();
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
}       // SetBaudrate

// *********************************************************************************************
void c_ControllerUsbSERIAL::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::restoreConfiguration (config);
    BaudrateControl.restoreConfiguration(config);

    // DEBUG_END;
}       // restoreConfiguration

// *********************************************************************************************
void c_ControllerUsbSERIAL::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::saveConfiguration (config);
    BaudrateControl.saveConfiguration(config);

    // DEBUG_END;
}       // saveConfiguration

// *********************************************************************************************
// EOF
