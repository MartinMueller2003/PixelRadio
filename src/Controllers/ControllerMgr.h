/*
  *    File: ControllerMgr.h
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
  *
  */

// *********************************************************************************************
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPUI.h>

class cControllerCommon;    // forward declaration

#define ControllerTypeId c_ControllerMgr::ControllerTypeId_t

#define LOCAL_CONTROLLER_ACTIVE_FLAG    0x0010
#define HTTP_CONTROLLER_ACTIVE_FLAG     0x0020
#define MQTT_CONTROLLER_ACTIVE_FLAG     0x0040
#define SERIAL_CONTROLLER_ACTIVE_FLAG   0x0080
#define FPPD_CONTROLLER_ACTIVE_FLAG     0x1000
#define SERIAL1_CONTROLLER_ACTIVE_FLAG  0x2000


class c_ControllerMgr
{
public:

    enum ControllerTypeId_t
    {
        USB_SERIAL_CNTRL = 0,
        GPIO_SERIAL_CNTRL,
        MQTT_CNTRL,
        FPPD_CNTRL,
        HTTP_CNTRL,
        LOCAL_CNTRL,
        NO_CNTRL,
        NumControllerTypes,
        ControllerIdStart = 0
    };

    struct RdsMsgInfo_t
    {
        String      ControllerName;
        String      Text;
        uint32_t    DurationMilliSec = 0;
    };

protected:

    struct ControllerInfo_t
    {
        ControllerTypeId_t  ControllerId    = ControllerIdStart;
        uint16_t            ActiveBit       = 0;
        uint16_t            SendingBit      = 0;
        cControllerCommon   * pController   = nullptr;
    };

    ControllerInfo_t    ListOfControllers[ControllerTypeId_t::NumControllerTypes];
    uint32_t            currentControllerIndex = ControllerTypeId_t::ControllerIdStart;
private:

    ControllerTypeId_t  CurrentSendingControllerId  = ControllerTypeId_t::NO_CNTRL;
    bool                RdsOutputEnabled            = true;
    void ClearAllMessagesPlayedConditions ();

public:

    c_ControllerMgr ();
    virtual~c_ControllerMgr ();
    void                poll ();
    void                begin ();
    void                AddControls (uint16_t TabId, ControlColor color);
    void                Display (ControllerTypeId_t Id);
    cControllerCommon   * GetControllerById (ControllerTypeId_t Id);
    bool                GetControllerEnabledFlag (ControllerTypeId_t Id);
    uint16_t            getControllerStatusSummary ();
    String              GetName (ControllerTypeId_t Id);
    bool                GetNextRdsMessage (RdsMsgInfo_t & Response);
    void                restoreConfiguration (ArduinoJson::JsonObject & config);
    void                saveConfiguration (ArduinoJson::JsonObject & config);
    void                SetRdsOutputEnabled (bool value) {RdsOutputEnabled = value;}
};  // c_ControllerMgr

#define CtypeId                 c_ControllerMgr::ControllerTypeId_t
#define LocalControllerId       CtypeId::LOCAL_CNTRL
#define HttpControllerId        CtypeId::HTTP_CNTRL
#define MqttControllerId        CtypeId::MQTT_CNTRL
#define FppdControllerId        CtypeId::FPPD_CNTRL
#define UsbSerialControllerId   CtypeId::USB_SERIAL_CNTRL
#define NullControllerId        CtypeId::NO_CNTRL

extern c_ControllerMgr ControllerMgr;

// *********************************************************************************************
// EOF
