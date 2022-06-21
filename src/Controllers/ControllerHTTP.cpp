/*
   File: ControllerHTTP.cpp
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
#include "ControllerHTTP.h"
#include "../Language.h"
#include <ArduinoLog.h>

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

static String EmptyString = "";

// *********************************************************************************************
// class c_ControllerHTTP : public c_ControllerCommon

// *********************************************************************************************
c_ControllerHTTP::c_ControllerHTTP() : c_ControllerCommon("HTTP", c_ControllerMgr::ControllerTypeId_t::HTTP_CNTRL)
{

} // c_ControllerHTTP
// *********************************************************************************************
c_ControllerHTTP::~c_ControllerHTTP() {}

// *********************************************************************************************
void c_ControllerHTTP::ControllerEnabledCb(Control *sender, int type)
{
   // DEBUG_START;

   String OnOffStr;
   if (sender->id == EspuiControlID)
   {
      // DEBUG_V();
      if (type == S_ACTIVE)
      {
         // DEBUG_V();
         ControllerEnabled = true;
         OnOffStr = F("On");
      }
      else if (type == S_INACTIVE)
      {
         // DEBUG_V();
         ControllerEnabled = false;
         OnOffStr = F("Off");
      }
      // DEBUG_V();
      displaySaveWarning();
      Log.infoln((String(F("TTP Controller Set to: ")) + OnOffStr).c_str());
   }

   // DEBUG_END;

} // httpCallback

// ************************************************************************************************
void c_ControllerHTTP::AddControls(uint16_t ctrlTab)
{
   // DEBUG_START;
   
   ESPUI.addControl(
      ControlType::Separator, 
      CTRL_HTPP_SET_STR,
      EmptyString, 
      ControlColor::None, 
      ctrlTab);
   
   EspuiControlID = ESPUI.addControl(
      ControlType::Switcher,
      CTRL_HTTP_STR,
      ControllerEnabled ? "1" : "0", 
      ControlColor::Turquoise,
      ctrlTab,
      [](Control *sender, int type, void* param)
      {
         reinterpret_cast<c_ControllerHTTP*>(param)->ControllerEnabledCb(sender, type);
      },
      this);
   // DEBUG_END;
} // AddControls

// *********************************************************************************************
void c_ControllerHTTP::RestoreControllerConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   // DEBUG_END;
} // RestoreControllerConfiguration

// *********************************************************************************************
void c_ControllerHTTP::SaveControllerConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   // DEBUG_END;
} // SaveControllerConfiguration

// *********************************************************************************************
// EOF
