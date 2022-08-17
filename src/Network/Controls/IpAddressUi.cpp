/*
   File: IpAddressUi.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include "IpAddressUi.hpp"
#include "memdebug.h"
#include "PtyCode.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>

static const PROGMEM uint32_t IpAddress_MAX_SZ = 16;

// *********************************************************************************************
cIpAddressUi::cIpAddressUi () : cControlCommon (emptyString)
{
    // _ DEBUG_START;

    DataValueStr.reserve (IpAddress_MAX_SZ + 2);
    DataValueStr        = F ("0.0.0.0");
    Title               = F ("Not Set");

    ActiveLabelStyle    = CSS_LABEL_STYLE_BLACK;
    InactiveLabelStyle  = CSS_LABEL_STYLE_BLACK;

    // _ DEBUG_END;
}

// *********************************************************************************************
cIpAddressUi::~cIpAddressUi ()
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cIpAddressUi::AddControls (uint16_t value, ControlColor color, String &_Title)
{
    // DEBUG_START;

    Title = _Title;
    AddControls (value, color);

    // DEBUG_END;
}

// *********************************************************************************************
void cIpAddressUi::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    uint16_t TempId;

    TempId = ESPUI.addControl (ControlType::Label,
            emptyString.c_str (),
            Title, ControlColor::Carrot,
            value);
    ESPUI.setElementStyle (TempId, CSS_LABEL_STYLE_BLACK);

    cControlCommon::AddControls (value, ControlType::Text, color);
    ESPUI.updateControlLabel (ControlId, Title.c_str ());
    ESPUI.addControl (ControlType::Max,
        emptyString.c_str (),
        String (IpAddress_MAX_SZ),
        ControlColor::None,
        ControlId);

    Booting = false;
    // DEBUG_END;
}

// *********************************************************************************************
IPAddress       cIpAddressUi::GetIpAddress () {return IpAddress;}

// *********************************************************************************************
void            cIpAddressUi::ResetToDefaults ()
{
    // DEBUG_START;

    String      value = F ("0.0.0.0");
    String      dummy;

    set (value, dummy);

    // DEBUG_END;
}

// *********************************************************************************************
bool cIpAddressUi::set (String &value, String &ResponseMessage)
{
    // DEBUG_START;

    // DEBUG_V(String("       value: ") + value);
    // DEBUG_V(String("DataValueStr: ") + DataValueStr);

    bool Response = true;

    ResponseMessage.reserve (128);
    ResponseMessage.clear ();

    IPAddress TempIp;

    do  // once
    {
        if (!TempIp.fromString (value))
        {
            ResponseMessage = Title + (F (" value: '")) + value + String (F ("' is not valid"));
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        if (TempIp == IPAddress (uint32_t (0)))
        {
            ResponseMessage = Title + (F (" value: '")) + value + String (F ("' is not valid"));
            Log.infoln (ResponseMessage.c_str ());
            Response = false;
            break;
        }

        if (TempIp == IpAddress)
        {
            // DEBUG_V("Address did not change");
            Log.infoln ((Title + F (" Unchanged")).c_str ());
            break;
        }
        IpAddress       = TempIp;
        DataValueStr    = IpAddress.toString ();
        DataValue       = IpAddress;

        ESPUI.updateControlValue (ControlId, DataValueStr);

        if (!Booting)
        {
            DHCP.TestIpSettings ();
        }
        Log.infoln ((Title + F (" Set to: ") + DataValueStr).c_str ());

        displaySaveWarning ();
    } while (false);

    // DEBUG_END;
    return Response;
}

// *********************************************************************************************
// OEF
