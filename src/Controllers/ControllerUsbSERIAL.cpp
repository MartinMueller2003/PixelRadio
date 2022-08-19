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
#include "ControllerUsbSERIAL.h"
#include "language.h"
#include <ArduinoLog.h>

#if __has_include ("memdebug.h")
 # include "memdebug.h"
#endif //  __has_include("memdebug.h")

// ================================================================================================
c_ControllerUsbSERIAL::c_ControllerUsbSERIAL () : c_ControllerCommon ("USB SERIAL", c_ControllerMgr::ControllerTypeId_t::USB_SERIAL_CNTRL)
{}      // c_ControllerUsbSERIAL

// ================================================================================================
c_ControllerUsbSERIAL::~c_ControllerUsbSERIAL ()
{}

// ************************************************************************************************
void c_ControllerUsbSERIAL::AddControls (uint16_t ctrlTab)
{
    // DEBUG_START;

    c_ControllerCommon::AddControls (ctrlTab);

    uint16_t LabelId;
    LabelId = ESPUI.addControl (ControlType::Label,
                                emptyString.c_str (),
                                String (N_BAUDRATE),
                                ControlColor::Turquoise,
                                ControlLabelElementId);
    ESPUI.setElementStyle (LabelId, CSS_LABEL_STYLE_BLACK);

    ControlerEnabledElementId = ESPUI.addControl (ControlType::Select,
                                                  emptyString.c_str (),
                                                  BaudRateStr,
                                                  ControlColor::Turquoise,
                                                  ControlLabelElementId,
                                                  [] (Control * sender, int type, void * param)
                                                  {
                                                      if (param)
                                                      {
                                                          reinterpret_cast <c_ControllerUsbSERIAL *> (param)->CbBaudrateControl (sender,
                                                                                                                                 type);
                                                      }
                                                  },
                                                  this);

    // DEBUG_V(String("ControlerEnabledElementId: ") + String(ControlerEnabledElementId));
    // ESPUI.addControl(ControlType::Option, SERIAL_OFF_STR, SERIAL_OFF_STR, ControlColor::None, ControlerEnabledElementId);
    ESPUI.      addControl (    ControlType::Option,    SERIAL_096_STR, SERIAL_096_STR, ControlColor::None,     ControlerEnabledElementId);
    ESPUI.      addControl (    ControlType::Option,    SERIAL_192_STR, SERIAL_192_STR, ControlColor::None,     ControlerEnabledElementId);
    ESPUI.      addControl (    ControlType::Option,    SERIAL_576_STR, SERIAL_576_STR, ControlColor::None,     ControlerEnabledElementId);
    ESPUI.      addControl (    ControlType::Option,    SERIAL_115_STR, SERIAL_115_STR, ControlColor::None,     ControlerEnabledElementId);
    // DEBUG_V();
#ifdef OldWay
        extern String logLevelStr;
        EspuiMsgId = ESPUI.addControl (ControlType::Label,
                                       "SERIAL_MSG",
                                       (logLevelStr.equals (F (DIAG_LOG_SILENT_STR))) ? F (CTLR_SERIAL_MSG_STR) : emptyString,
                                       ControlColor::Turquoise,
                                       ControlLabelElementId);
        ESPUI.setElementStyle (EspuiMsgId, CSS_LABEL_STYLE_BLACK);
#endif // def OldWay

    // DEBUG_END;
}       // AddControls

// ************************************************************************************************
void c_ControllerUsbSERIAL::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t & Response)
{
    if (ControllerEnabled)
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
    uint32_t OriginalBaudrate = BaudRate;

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

    displaySaveWarning ();
    Log.infoln ((Name + F (": Baudrate Set to: ") + BaudRateStr).c_str ());

    // DEBUG_END;
}       // CbBaudrateControl

// ************************************************************************************************
void c_ControllerUsbSERIAL::serialCommands (void)
{
    // _ DEBUG_START;

    do  // once
    {
        if (!ControllerEnabled)
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
            CommandProcessor.ProcessCommand (cmdStr, paramStr, Name, Response);
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
    uint32_t OriginalBaudrate   = BaudRate;

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
        ControllerEnabled       = false;

        // DEBUG_V();
    } while (false);
    // DEBUG_V();

    if ((OriginalBaudrate != BaudRate) && (ControllerEnabled))
    {
        // DEBUG_V("Change Baudrate");
        Serial.end ();  // Flush all characters in queue.
        Serial.begin (BaudRate);

        while (!Serial && !Serial.available ())
        {}                      // Wait for Serial Port to be available.
        Serial.println ();      // Push out any corrupted data due to baud change.
        // DEBUG_V("Change Baudrate - Done");
    }
    // DEBUG_END;
    return Response;
}       // SetBaudrate

// *********************************************************************************************
void c_ControllerUsbSERIAL::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    c_ControllerCommon::restoreConfiguration (config);

    if (config.containsKey (N_Baudrate))
    {
        BaudRateStr = (const char *)config[N_Baudrate];
        SetBaudrate (BaudRateStr);
    }
    // DEBUG_END;
}       // restoreConfiguration

// *********************************************************************************************
void c_ControllerUsbSERIAL::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    c_ControllerCommon::saveConfiguration (config);

    config[N_Baudrate] = BaudRateStr;

    // DEBUG_END;
}       // saveConfiguration

// *********************************************************************************************
// EOF
