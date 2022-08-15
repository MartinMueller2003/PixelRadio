#pragma once
/*
   File: FPPDiscovery.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.0
   Creation: Dec-16-2021
   Revised:  Mar-30-2022
   Public Release:
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, Martin Mueller
   Revision History: See PixelRadio.cpp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 *
 */

#include <Arduino.h>
#include "PixelRadio.h"
#include <ESPAsyncWebServer.h>
#include <AsyncUDP.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

class c_FPPDiscovery
{
public:

    typedef std::function <void (String &FileName, void * UserParam)> FileChangeCb;

private:

    AsyncUDP udp;
    void        ProcessReceivedUdpPacket (AsyncUDPPacket _packet);
    void        ProcessSyncPacket (uint8_t action, String filename, float seconds_elapsed);
    void        sendPingPacket (IPAddress destination = IPAddress(255, 255, 255, 255));

    bool hasBeenInitialized     = false;
    bool OldNetworkState        = false;
    IPAddress FppRemoteIp       = IPAddress (uint32_t (0));
    String CurrentFileName;
    FileChangeCb FppdCb;
    void * UserParam = nullptr;

    void        GetSysInfoJSON    (ArduinoJson::JsonObject &jsonResponse);
    void        BuildFseqResponse (String fname, String &resp);

    struct MultiSyncStats_t
    {
        time_t          lastReceiveTime = 0;
        uint32_t        pktCommand      = 0;
        uint32_t        pktSyncSeqOpen  = 0;
        uint32_t        pktSyncSeqStart = 0;
        uint32_t        pktSyncSeqStop  = 0;
        uint32_t        pktSyncSeqSync  = 0;
        uint32_t        pktSyncMedOpen  = 0;
        uint32_t        pktSyncMedStart = 0;
        uint32_t        pktSyncMedStop  = 0;
        uint32_t        pktSyncMedSync  = 0;
        uint32_t        pktBlank        = 0;
        uint32_t        pktPing         = 0;
        uint32_t        pktPlugin       = 0;
        uint32_t        pktFPPCommand   = 0;
        uint32_t        pktError        = 0;
    };
    MultiSyncStats_t MultiSyncStats;

#define SYNC_PKT_START       0
#define SYNC_PKT_STOP        1
#define SYNC_PKT_SYNC        2
#define SYNC_PKT_OPEN        3

#define SYNC_FILE_SEQ        0
#define SYNC_FILE_MEDIA      1

#define CTRL_PKT_CMD         0  // deprecated in favor of FPP Commands
#define CTRL_PKT_SYNC        1
#define CTRL_PKT_EVENT       2  // deprecated in favor of FPP Commands
#define CTRL_PKT_BLANK       3
#define CTRL_PKT_PING        4
#define CTRL_PKT_PLUGIN      5
#define CTRL_PKT_FPPCOMMAND  6

public:

    c_FPPDiscovery ();
    virtual ~c_FPPDiscovery ()
    {
    }

    void        begin (FileChangeCb _FppdCb, void * _UserParam);

    void        ProcessFPPJson      (AsyncWebServerRequest * request);
    void        ProcessGET          (AsyncWebServerRequest * request);
    void        NetworkStateChanged (bool NewNetworkState);
    String&     GetCurrentFileName  ()
    {
        return CurrentFileName;
    }
};

extern c_FPPDiscovery  FPPDiscovery;
