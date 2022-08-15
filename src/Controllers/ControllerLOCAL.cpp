/*
   File: ControllerLOCAL.cpp
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
#include "ControllerLOCAL.h"
#include "language.h"

#if __has_include ("memdebug.h")
# include "memdebug.h"
#endif //  __has_include("memdebug.h")

// *********************************************************************************************
// class c_ControllerLOCAL : public c_ControllerCommon

// *********************************************************************************************
c_ControllerLOCAL::c_ControllerLOCAL () : c_ControllerCommon ("LOCAL", c_ControllerMgr::ControllerTypeId_t::LOCAL_CNTRL)
{
}       // c_ControllerLOCAL

// *********************************************************************************************
c_ControllerLOCAL::~c_ControllerLOCAL ()
{
}

// *********************************************************************************************
void c_ControllerLOCAL::AddControls (uint16_t ctrlTab)
{
    // DEBUG_START;

    c_ControllerCommon::AddControls (ctrlTab);

    // Messages.SetTitle(Name + " " + N_Messages);
    Messages.AddControls (EspuiParentElementId);
    Messages.ActivateMessageSet (Name);

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerLOCAL::CreateDefaultMsgSet ()
{
    // DEBUG_START;

    Messages.clear ();
    Messages.   AddMessage (    F ("LOCAL"),    F ("Welcome to Our Drive-by Holiday Light Show"));
    Messages.   AddMessage (    F ("LOCAL"),    F ("For Safety Keep Automobile Running Lights On"));
    Messages.   AddMessage (    F ("LOCAL"),    F ("Please Drive Slowly and Watch Out for Children and Pets"));

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerLOCAL::GetNextRdsMessage (c_ControllerMgr::RdsMsgInfo_t &Response)
{
    // DEBUG_START;

    if (ControllerEnabled)
    {
        Messages.GetNextRdsMessage (Response);
    }
    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerLOCAL::restoreConfiguration (ArduinoJson::JsonObject &config)
{
    // DEBUG_START;

    c_ControllerCommon::restoreConfiguration (config);
    Messages.RestoreConfig (config);

    // do we need to create a set of default messages?
    if (Messages.empty ())
    {
        CreateDefaultMsgSet ();
    }
    // DEBUG_END;
}       // restoreConfiguration

// *********************************************************************************************
void c_ControllerLOCAL::saveConfiguration (ArduinoJson::JsonObject &config)
{
    // DEBUG_START;

    c_ControllerCommon::saveConfiguration (config);
    Messages.SaveConfig (config);

    // DEBUG_END;
}       // saveConfiguration

// *********************************************************************************************
// EOF
