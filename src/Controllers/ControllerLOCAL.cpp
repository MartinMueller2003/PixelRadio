/*
  *    File: ControllerLOCAL.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.0
  *    Creation: Dec-16-2021
  *    Revised:  Apr-06-2022
  *    Public Release:
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *    Revision History: See PixelRadio.cpp
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license
  *    absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  *
  *     Note 1: All Text uses defines instead of const String. This saves ~30K Ram and ~50K Flash Memory.
  */

// *********************************************************************************************
#include "ControllerLOCAL.h"
#include "language.h"

#if __has_include ("memdebug.h")
 #include "memdebug.h"
#endif //  __has_include("memdebug.h")

// *********************************************************************************************
static const PROGMEM char Name [] = "LOCAL";

// *********************************************************************************************
c_ControllerLOCAL::c_ControllerLOCAL () :   cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::LOCAL_CNTRL)
{}  // c_ControllerLOCAL

// *********************************************************************************************
c_ControllerLOCAL::~c_ControllerLOCAL ()
{}

// *********************************************************************************************
void c_ControllerLOCAL::AddControls (uint16_t TabId, ControlColor color)
{
    // DEBUG_START;

    cControllerCommon::AddControls (TabId, color);

    // Messages.SetTitle(Name + " " + N_Messages);
    Messages.AddControls (TabId);
    Messages.ActivateMessageSet (GetTitle ());

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerLOCAL::CreateDefaultMsgSet ()
{
    // DEBUG_START;

    Messages.clear ();
    Messages.   AddMessage (F ("LOCAL"),    F ("Welcome to Our Drive-by Holiday Light Show"));
    Messages.   AddMessage (F ("LOCAL"),    F ("For Safety Keep Automobile Running Lights On"));
    Messages.   AddMessage (F ("LOCAL"),    F ("Please Drive Slowly and Watch Out for Children and Pets"));

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerLOCAL::GetNextRdsMessage (const String & value, c_ControllerMgr::RdsMsgInfo_t & Response)
{
    // DEBUG_START;

    if (ControllerIsEnabled ())
    {
        // DEBUG_V(String("value: '") + String(value) + "'");
        Messages.GetNextRdsMessage (value, Response);
    }

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerLOCAL::restoreConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::restoreConfiguration (config);
    Messages.RestoreConfig (config);

    // do we need to create a set of default messages?
    if (Messages.empty ())
    {
        CreateDefaultMsgSet ();
    }

    // DEBUG_END;
}   // restoreConfiguration

// *********************************************************************************************
void c_ControllerLOCAL::saveConfiguration (ArduinoJson::JsonObject & config)
{
    // DEBUG_START;

    cControllerCommon::saveConfiguration (config);
    Messages.SaveConfig (config);

    // DEBUG_END;
}   // saveConfiguration

// *********************************************************************************************
bool c_ControllerLOCAL::SetRdsText (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = false;

    do  // once
    {
        if (payloadStr.length () > 63)
        {
            ResponseMessage = F ("Controller LOCAL: BAD VALUE: Text is too long");
            break;
        }

        response = true;
        Messages.AddMessage (F ("LOCAL"), payloadStr);
        ResponseMessage = F ("OK");
    } while (false);

    // DEBUG_END;
    return response;
}

// *********************************************************************************************
bool c_ControllerLOCAL::SetRdsTime (String & payloadStr, String & ResponseMessage)
{
    // DEBUG_START;

    bool response = false;

    do  // once
    {
        uint32_t NewTime = payloadStr.toInt ();
        if (NewTime < 5)
        {
            ResponseMessage = F ("Controller LOCAL: BAD VALUE: Too Small");
            break;
        }

        response = true;
        Messages.SetDurration (F ("LOCAL"), NewTime);
        ResponseMessage = F ("OK");
    } while (false);

    // DEBUG_END;
    return response;
}

// *********************************************************************************************
// EOF
