/*
   File: ControllerNONE.cpp
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
#include "ControllerNONE.h"

#if __has_include("../memdebug.h")
#  include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

// *********************************************************************************************
// class c_ControllerNONE : public c_ControllerCommon

// *********************************************************************************************
c_ControllerNONE::c_ControllerNONE() : c_ControllerCommon("NONE", c_ControllerMgr::ControllerTypeId_t::NO_CNTRL)
{

} // c_ControllerNONE

// *********************************************************************************************
c_ControllerNONE::~c_ControllerNONE() {}

// *********************************************************************************************
void c_ControllerNONE::restoreConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::restoreConfiguration(config);

   // DEBUG_END;
} // restoreConfiguration

// *********************************************************************************************
void c_ControllerNONE::saveConfiguration(ArduinoJson::JsonObject &config)
{
   // DEBUG_START;

   c_ControllerCommon::saveConfiguration(config);

   // DEBUG_END;
} // saveConfiguration

// *********************************************************************************************
// EOF
