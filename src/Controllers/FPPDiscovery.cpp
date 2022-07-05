/*
   File: FPPDiscovery.cpp
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
*/

#include <Arduino.h>
#include "FPPDiscovery.h"
#include "fseq.h"
#include <WiFi.h>
#include <time.h>
#include "../language.h"

#if __has_include("../memdebug.h")
#include "../memdebug.h"
#endif //  __has_include("../memdebug.h")

#define FPP_TYPE_ID         0xC3
#define FPP_VARIANT_NAME    String(N_PixelRadio)

#define FPP_DISCOVERY_PORT  32320
#define ulrCommand          N_command
#define ulrPath             N_path

//-----------------------------------------------------------------------------
c_FPPDiscovery::c_FPPDiscovery()
{
    // DEBUG_START;
    memset((void *)&MultiSyncStats, 0x00, sizeof(MultiSyncStats));
    // DEBUG_END;
} // c_FPPDiscovery

//-----------------------------------------------------------------------------
void c_FPPDiscovery::begin(FileChangeCb _FppdCb, void* _UserParam)
{
    // DEBUG_START;

    FppdCb = _FppdCb;
    UserParam = _UserParam;

    hasBeenInitialized = true;
    NetworkStateChanged(WiFi.isConnected());

    // register for changes in the WiFi State
    WiFi.onEvent([](WiFiEvent_t)
                 { FPPDiscovery.NetworkStateChanged(WiFi.isConnected()); },
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

    WiFi.onEvent([](WiFiEvent_t)
                 { FPPDiscovery.NetworkStateChanged(WiFi.isConnected()); },
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    WiFi.onEvent([](WiFiEvent_t)
                 { FPPDiscovery.NetworkStateChanged(WiFi.isConnected()); },
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

    WiFi.onEvent([](WiFiEvent_t)
                 { FPPDiscovery.NetworkStateChanged(WiFi.isConnected()); },
                 WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    // DEBUG_END;
} // begin

//-----------------------------------------------------------------------------
// Configure and start the web server
void c_FPPDiscovery::NetworkStateChanged(bool NewNetworkState)
{
    // DEBUG_START;

    do // once
    {
        if (OldNetworkState == NewNetworkState)
        {
            break;
        }
        OldNetworkState = NewNetworkState;

        if (false == NewNetworkState)
        {
            break;
        }

        // DEBUG_V ();

        IPAddress address = IPAddress(239, 70, 80, 80);
        bool fail = false;

        // Try to listen to the broadcast port
        if (!udp.listen(FPP_DISCOVERY_PORT))
        {
            Log.infoln(String(F("FPP Discovery: FAILED to subscribed to broadcast messages")).c_str());
            fail = true;
            break;
        }
        // Log.infoln (String (F ("FPPDiscovery subscribed to broadcast")));

        if (!udp.listenMulticast(address, FPP_DISCOVERY_PORT))
        {
            Log.infoln(String(F("FPP Discovery: FAILED to subscribed to multicast messages")).c_str());
            fail = true;
            break;
        }
        // Log.infoln (String (F ("FPPDiscovery subscribed to multicast: ")) + address.toString ());

        if (!fail)
        {
            Log.infoln((String(F("FPP Discovery: Listening on port ")) + String(FPP_DISCOVERY_PORT)).c_str());
        }

        udp.onPacket(std::bind(&c_FPPDiscovery::ProcessReceivedUdpPacket, this, std::placeholders::_1));

        sendPingPacket();

    } while (false);

    // DEBUG_END;

} // NetworkStateChanged

//-----------------------------------------------------------------------------
void c_FPPDiscovery::ProcessReceivedUdpPacket(AsyncUDPPacket UDPpacket)
{
    // DEBUG_START;
    do // once
    {
        FPPPacket *fppPacket = reinterpret_cast<FPPPacket *>(UDPpacket.data());
        // DEBUG_V (String ("Received UDP packet from: ") + UDPpacket.remoteIP ().toString ());
        // DEBUG_V (String ("                 Sent to: ") + UDPpacket.localIP ().toString ());
        // DEBUG_V (String ("         FPP packet_type: ") + String(fppPacket->packet_type));

        if ((fppPacket->header[0] != 'F') ||
            (fppPacket->header[1] != 'P') ||
            (fppPacket->header[2] != 'P') ||
            (fppPacket->header[3] != 'D'))
        {
            // DEBUG_V ("Invalid FPP header");
            MultiSyncStats.pktError++;
            break;
        }
        // DEBUG_V ();

        struct timeval tv;
        gettimeofday(&tv, NULL);
        MultiSyncStats.lastReceiveTime = tv.tv_sec;

        switch (fppPacket->packet_type)
        {
            case CTRL_PKT_CMD: // deprecated in favor of FPP Commands
            {
                MultiSyncStats.pktCommand++;
                // DEBUG_V ("Unsupported PDU: CTRL_PKT_CMD");
                break;
            }

            case CTRL_PKT_SYNC:
            {
                FPPMultiSyncPacket *msPacket = reinterpret_cast<FPPMultiSyncPacket *>(UDPpacket.data());
                // DEBUG_V (String (F ("msPacket->sync_type: ")) + String(msPacket->sync_type));

                if (msPacket->sync_type == SYNC_FILE_SEQ)
                {
                    // FSEQ type, not media
                    // DEBUG_V (String (F ("Received FPP FSEQ sync packet")));
                    FppRemoteIp = UDPpacket.remoteIP();
                    ProcessSyncPacket(msPacket->sync_action, String(msPacket->filename), msPacket->seconds_elapsed);
                }
                else if (msPacket->sync_type == SYNC_FILE_MEDIA)
                {
                    // DEBUG_V (String (F ("Unsupported SYNC_FILE_MEDIA message.")));
                }
                else
                {
                    // DEBUG_V (String (F ("Unexpected Multisync msPacket->sync_type: ")) + String (msPacket->sync_type));
                }

                break;
            }

            case CTRL_PKT_EVENT: // deprecated in favor of FPP Commands
            {
                // DEBUG_V ("Unsupported PDU: CTRL_PKT_EVENT");
                break;
            }

            case CTRL_PKT_BLANK:
            {
                // DEBUG_V (String (F ("FPP Blank packet")));
                MultiSyncStats.pktBlank++;
                CurrentFileName = emptyString;
                break;
            }

            case CTRL_PKT_PING:
            {
                // DEBUG_V (String (F ("Ping Packet")));

                MultiSyncStats.pktPing++;
                FPPPingPacket *pingPacket = reinterpret_cast<FPPPingPacket *>(UDPpacket.data());

                // DEBUG_V (String (F ("Ping Packet subtype: ")) + String (pingPacket->ping_subtype));
                // DEBUG_V (String (F ("Ping Packet packet.versionMajor: ")) + String (pingPacket->versionMajor));
                // DEBUG_V (String (F ("Ping Packet packet.versionMinor: ")) + String (pingPacket->versionMinor));
                // DEBUG_V (String (F ("Ping Packet packet.hostName:     ")) + String (pingPacket->hostName));
                // DEBUG_V (String (F ("Ping Packet packet.hardwareType: ")) + String (pingPacket->hardwareType));

                if (pingPacket->ping_subtype == 0x01)
                {
                    // DEBUG_V (String (F ("FPP Ping discovery packet")));
                    // received a discover ping packet, need to send a ping out
                    if (UDPpacket.isBroadcast() || UDPpacket.isMulticast())
                    {
                        // DEBUG_V ("Broadcast Ping Response");
                        sendPingPacket();
                    }
                    else
                    {
                        // DEBUG_V ("Unicast Ping Response");
                        sendPingPacket(UDPpacket.remoteIP());
                    }
                }
                else
                {
                    // DEBUG_V (String (F ("Unexpected Ping sub type: ")) + String (pingPacket->ping_subtype));
                }
                break;
            }

            case CTRL_PKT_PLUGIN:
            {
                // DEBUG_V ("Unsupported PDU: CTRL_PKT_PLUGIN");
                MultiSyncStats.pktPlugin++;
                break;
            }

            case CTRL_PKT_FPPCOMMAND:
            {
                // DEBUG_V ("Unsupported PDU: CTRL_PKT_FPPCOMMAND");
                MultiSyncStats.pktFPPCommand++;
                break;
            }

            default:
            {
                // DEBUG_V (String ("UnHandled PDU: packet_type:  ") + String (fppPacket->packet_type));
                break;
            }
        } // switch (fppPacket->packet_type)
    } while (false);

    // DEBUG_END;
} // ProcessReceivedUdpPacket

//-----------------------------------------------------------------------------
void c_FPPDiscovery::ProcessSyncPacket(uint8_t action, String FileName, float SecondsElapsed)
{
    // DEBUG_START;
    do // once
    {
        // DEBUG_V (String("  action: ") + String(action));
        // DEBUG_V (String("FileName: '") + FileName + "'");
        CurrentFileName = FileName;

        switch (action)
        {
            case SYNC_PKT_START:
            {
                // DEBUG_V ("Sync::Start");
                // DEBUG_V (String ("      FileName: ") + FileName);
                // DEBUG_V (String ("SecondsElapsed: ") + SecondsElapsed);
                MultiSyncStats.pktSyncSeqStart++;
                break;
            }

            case SYNC_PKT_STOP:
            {
                // DEBUG_V ("Sync::Stop");
                // DEBUG_V (String ("      FileName: ") + FileName);
                // DEBUG_V (String ("SecondsElapsed: ") + SecondsElapsed);
                CurrentFileName = emptyString;
                MultiSyncStats.pktSyncSeqStop++;
                break;
            }

            case SYNC_PKT_SYNC:
            {
                // DEBUG_V ("Sync");
                // DEBUG_V (String ("      FileName: ") + FileName);
                // DEBUG_V (String ("SecondsElapsed: ") + SecondsElapsed);

                /*
                Log.infoln (String(float(millis()/1000.0)) + "," +
                                String(InputFPPRemotePlayFile.GetLastFrameId()) + "," +
                                String (seconds_elapsed) + "," +
                                String (FrameId) + "," +
                                String(InputFPPRemotePlayFile.GetTimeOffset(),5));
                */

                MultiSyncStats.pktSyncSeqSync++;
                // DEBUG_V (String ("SecondsElapsed: ") + String (SecondsElapsed));
                break;
            }

            case SYNC_PKT_OPEN:
            {
                // DEBUG_V ("Sync::Open");
                // DEBUG_V (String ("      FileName: ") + FileName);
                // DEBUG_V (String ("SecondsElapsed: ") + SecondsElapsed);
                // StartPlaying (FileName, FrameId);
                MultiSyncStats.pktSyncSeqOpen++;
                break;
            }

            default:
            {
                // DEBUG_V (String (F ("Sync: ERROR: Unknown Action: ")) + String (action));
                break;
            }

        } // switch
    } while (false);

    FppdCb(CurrentFileName, UserParam);

    // DEBUG_END;
} // ProcessSyncPacket

//-----------------------------------------------------------------------------
void c_FPPDiscovery::sendPingPacket(IPAddress destination)
{
    // DEBUG_START;

    FPPPingPacket packet;
    memset(packet.raw, 0, sizeof(packet));
    packet.raw[0] = 'F';
    packet.raw[1] = 'P';
    packet.raw[2] = 'P';
    packet.raw[3] = 'D';
    packet.packet_type = 0x04;
    packet.data_len = 294;
    packet.ping_version = 0x3;
    packet.ping_subtype = 0x0;
    packet.ping_hardware = FPP_TYPE_ID;

    uint16_t v = (uint16_t)atoi(VERSION_STR);
    packet.versionMajor = (v >> 8) + ((v & 0xFF) << 8);
    v = (uint16_t)atoi(&VERSION_STR[2]);
    packet.versionMinor = (v >> 8) + ((v & 0xFF) << 8);

    packet.operatingMode = 0x08; // Support remote mode : Bridge Mode

    uint32_t ip = static_cast<uint32_t>(WiFi.localIP());
    memcpy(packet.ipAddress, &ip, 4);

    strcpy(packet.hostName, WiFi.getHostname());
    strcpy(packet.version, VERSION_STR);
    strcpy(packet.hardwareType, FPP_VARIANT_NAME.c_str());
    packet.ranges[0] = 0;

    // DEBUG_V ("Send Ping to " + destination.toString());
    udp.writeTo(packet.raw, sizeof(packet), destination, FPP_DISCOVERY_PORT);

    // DEBUG_END;
} // sendPingPacket

// #define PRINT_DEBUG
#ifdef PRINT_DEBUG
//-----------------------------------------------------------------------------
static void printReq(AsyncWebServerRequest *request, bool post)
{
    // DEBUG_START;

    int params = request->params();
    // DEBUG_V (String ("   Num Params: ") + String (params));

    for (int i = 0; i < params; i++)
    {
        // DEBUG_V (String ("current Param: ") + String (i));
        AsyncWebParameter *p = request->getParam(i);
        // DEBUG_V (String ("      p->name: ") + String (p->name()));
        // DEBUG_V (String ("     p->value: ") + String (p->value()));

        if (p->isFile())
        { // p->isPost() is also true
            LOG_PORT.printf_P(PSTR("FILE[%s]: %s, size: %u\n"), p->name().c_str(), p->value().c_str(), p->size());
        }
        else if (p->isPost())
        {
            LOG_PORT.printf_P(PSTR("POST[%s]: %s\n"), p->name().c_str(), p->value().c_str());
        }
        else
        {
            LOG_PORT.printf_P(PSTR("GET[%s]: %s\n"), p->name().c_str(), p->value().c_str());
        }
    }
    // DEBUG_END;
} // printReq
#else
#define printReq(a, b)
#endif // !def PRINT_DEBUG

//-----------------------------------------------------------------------------
void c_FPPDiscovery::BuildFseqResponse(String fname, String &resp)
{
    // DEBUG_START;

    DynamicJsonDocument JsonDoc(4 * 1024);
    JsonObject JsonData = JsonDoc.to<JsonObject>();

    JsonData[F("Name")] = fname;
    JsonData[F("Version")] = String(0) + "." + String(0);
    JsonData[F("ID")] = String(0);
    JsonData[F("StepTime")] = String(0);
    JsonData[F("NumFrames")] = String(0);
    JsonData[F("CompressionType")] = 0;

    static const int TIME_STR_CHAR_COUNT = 32;
    char timeStr[TIME_STR_CHAR_COUNT];
    struct tm tm = *gmtime(&MultiSyncStats.lastReceiveTime);
    // BUGBUG -- trusting the provided `tm` structure values contain valid data ... use `snprintf` to mitigate.
    int actuallyWritten = snprintf(timeStr, TIME_STR_CHAR_COUNT,
                                   "%4d-%.2d-%.2d %.2d:%.2d:%.2d",
                                   1900 + tm.tm_year, tm.tm_mon + 1, tm.tm_mday,
                                   tm.tm_hour, tm.tm_min, tm.tm_sec);

    // TODO: assert ((actuallyWritten > 0) && (actuallyWritten < TIME_STR_CHAR_COUNT))
    if ((actuallyWritten > 0) && (actuallyWritten < TIME_STR_CHAR_COUNT))
    {
        JsonData[F("lastReceiveTime")] = timeStr;
    }

    JsonData[F("pktCommand")] = MultiSyncStats.pktCommand;
    JsonData[F("pktSyncSeqOpen")] = MultiSyncStats.pktSyncSeqOpen;
    JsonData[F("pktSyncSeqStart")] = MultiSyncStats.pktSyncSeqStart;
    JsonData[F("pktSyncSeqStop")] = MultiSyncStats.pktSyncSeqStop;
    JsonData[F("pktSyncSeqSync")] = MultiSyncStats.pktSyncSeqSync;
    JsonData[F("pktSyncMedOpen")] = MultiSyncStats.pktSyncMedOpen;
    JsonData[F("pktSyncMedStart")] = MultiSyncStats.pktSyncMedStart;
    JsonData[F("pktSyncMedStop")] = MultiSyncStats.pktSyncMedStop;
    JsonData[F("pktSyncMedSync")] = MultiSyncStats.pktSyncMedSync;
    JsonData[F("pktBlank")] = MultiSyncStats.pktBlank;
    JsonData[F("pktPing")] = MultiSyncStats.pktPing;
    JsonData[F("pktPlugin")] = MultiSyncStats.pktPlugin;
    JsonData[F("pktFPPCommand")] = MultiSyncStats.pktFPPCommand;
    JsonData[F("pktError")] = MultiSyncStats.pktError;
    JsonData[F("MaxChannel")] = String(0);
    JsonData[F("ChannelCount")] = String(0);
#ifdef DOWENEEDTHIS
    uint32_t FileOffsetToCurrentHeaderRecord = 0;
    uint32_t FileOffsetToStartOfSequenceData = 0; // DataOffset

    // DEBUG_V (String ("FileOffsetToCurrentHeaderRecord: ") + String (FileOffsetToCurrentHeaderRecord));
    // DEBUG_V (String ("FileOffsetToStartOfSequenceData: ") + String (FileOffsetToStartOfSequenceData));

    if (FileOffsetToCurrentHeaderRecord < FileOffsetToStartOfSequenceData)
    {
        JsonArray JsonDataHeaders = JsonData.createNestedArray(F("variableHeaders"));

        char FSEQVariableDataHeaderBuffer[sizeof(FSEQRawVariableDataHeader) + 1];
        memset((uint8_t *)FSEQVariableDataHeaderBuffer, 0x00, sizeof(FSEQVariableDataHeaderBuffer));
        FSEQRawVariableDataHeader *pCurrentVariableHeader = (FSEQRawVariableDataHeader *)FSEQVariableDataHeaderBuffer;

        while (FileOffsetToCurrentHeaderRecord < FileOffsetToStartOfSequenceData)
        {
            FileMgr.ReadSdFile(fseq, (byte *)FSEQVariableDataHeaderBuffer, sizeof(FSEQRawVariableDataHeader), FileOffsetToCurrentHeaderRecord);

            int VariableDataHeaderTotalLength = read16((uint8_t *)&(pCurrentVariableHeader->length));
            int VariableDataHeaderDataLength = VariableDataHeaderTotalLength - sizeof(FSEQRawVariableDataHeader);

            String HeaderTypeCode(pCurrentVariableHeader->type);

            if ((HeaderTypeCode == "mf") || (HeaderTypeCode == "sp"))
            {
                char *VariableDataHeaderDataBuffer = (char *)malloc(VariableDataHeaderDataLength + 1);
                memset(VariableDataHeaderDataBuffer, 0x00, VariableDataHeaderDataLength + 1);

                FileMgr.ReadSdFile(fseq, (byte *)VariableDataHeaderDataBuffer, VariableDataHeaderDataLength, FileOffsetToCurrentHeaderRecord);

                JsonObject JsonDataHeader = JsonDataHeaders.createNestedObject();
                JsonDataHeader[HeaderTypeCode] = String(VariableDataHeaderDataBuffer);

                free(VariableDataHeaderDataBuffer);
            }

            FileOffsetToCurrentHeaderRecord += VariableDataHeaderTotalLength + sizeof(FSEQRawVariableDataHeader);
        } // while there are headers to process
    }     // there are headers to process
#endif    // def DOWENEEDTHIS

    serializeJson(JsonData, resp);
    // DEBUG_V (String ("resp: ") + resp);

    // DEBUG_END;

} // BuildFseqResponse

//-----------------------------------------------------------------------------
void c_FPPDiscovery::ProcessGET(AsyncWebServerRequest *request)
{
    // DEBUG_START;
    printReq(request, false);

    do // once
    {
        if (!request->hasParam(ulrPath))
        {
            request->send(404);
            // DEBUG_V ();
            break;
        }

        // DEBUG_V ();

        String path = request->getParam(ulrPath)->value();

        // DEBUG_V (String ("Path: ") + path);

        if (path.startsWith(F("/api/sequence/")))
        {
            // DEBUG_V ();

            String seq = path.substring(14);
            if (seq.endsWith(F("/meta")))
            {
                // DEBUG_V ();

                seq = seq.substring(0, seq.length() - 5);

                String resp = emptyString;
                BuildFseqResponse(seq, resp);
                request->send(200, F("application/json"), resp);
                break;
            }
        }
        request->send(404);

    } while (false); // do once

    // DEBUG_END;

} // ProcessGET

//-----------------------------------------------------------------------------
void c_FPPDiscovery::GetSysInfoJSON(JsonObject &jsonResponse)
{
    // DEBUG_START;

    jsonResponse[F("HostName")] = WiFi.getHostname();
    jsonResponse[F("HostDescription")] = emptyString;
    jsonResponse[F("Platform")] = F("PixelRadio");
    jsonResponse[F("Variant")] = FPP_VARIANT_NAME;
    jsonResponse[F("Mode")] = F("remote");
    jsonResponse[F("Version")] = VERSION_STR;

    jsonResponse[F("majorVersion")] = (uint16_t)atoi(VERSION_STR);
    jsonResponse[F("minorVersion")] = (uint16_t)atoi(&VERSION_STR[2]);
    jsonResponse[F("typeId")] = FPP_TYPE_ID;

    JsonObject jsonResponseUtilization = jsonResponse.createNestedObject(F("Utilization"));
    jsonResponseUtilization[F("MemoryFree")] = ESP.getFreeHeap();
    jsonResponseUtilization[F("Uptime")] = millis();

    jsonResponse[F("rssi")] = WiFi.RSSI();
    JsonArray jsonResponseIpAddresses = jsonResponse.createNestedArray(F("IPS"));
    jsonResponseIpAddresses.add(WiFi.localIP().toString());

    // DEBUG_END;

} // GetSysInfoJSON

//-----------------------------------------------------------------------------
void c_FPPDiscovery::ProcessFPPJson(AsyncWebServerRequest *request)
{
    // DEBUG_START;
    printReq(request, false);

    do // once
    {
        if (!request->hasParam(ulrCommand))
        {
            request->send(404);
            // DEBUG_V (String ("Missing Param: 'command' "));

            break;
        }

        DynamicJsonDocument JsonDoc(2048);
        JsonObject JsonData = JsonDoc.to<JsonObject>();

        String command = request->getParam(ulrCommand)->value();
        // DEBUG_V (String ("command: ") + command);

        if (command == F("getFPPstatus"))
        {
            String adv = F("false");
            if (request->hasParam(F("advancedView")))
            {
                adv = request->getParam(F("advancedView"))->value();
            }

            JsonObject JsonDataMqtt = JsonData.createNestedObject(F("MQTT"));

            JsonDataMqtt[F("configured")] = false;
            JsonDataMqtt[F("connected")] = false;

            JsonObject JsonDataCurrentPlaylist = JsonData.createNestedObject(F("current_playlist"));

            JsonDataCurrentPlaylist[F("count")] = F("0");
            JsonDataCurrentPlaylist[F("description")] = emptyString;
            JsonDataCurrentPlaylist[F("index")] = F("0");
            JsonDataCurrentPlaylist[F("playlist")] = emptyString;
            JsonDataCurrentPlaylist[F("type")] = emptyString;

            JsonData[F("volume")] = 70;
            JsonData[F("media_filename")] = emptyString;
            JsonData[F("fppd")] = F("running");
            JsonData[F("current_song")] = emptyString;

            JsonData[F("current_sequence")] = emptyString;
            JsonData[F("playlist")] = emptyString;
            JsonData[F("seconds_elapsed")] = String(0);
            JsonData[F("seconds_played")] = String(0);
            JsonData[F("seconds_remaining")] = String(0);
            JsonData[F("sequence_filename")] = emptyString;
            JsonData[F("time_elapsed")] = String(F("00:00"));
            JsonData[F("time_remaining")] = String(F("00:00"));

            JsonData[F("status")] = 0;
            JsonData[F("status_name")] = F("idle");

            JsonData[F("mode")] = 8;
            JsonData[F("mode_name")] = F("remote");

            String Response;
            serializeJson(JsonDoc, Response);
            // DEBUG_V (String ("JsonDoc: ") + Response);
            request->send(200, F("application/json"), Response);

            break;
        }

        if (command == F("getSysInfo"))
        {
            GetSysInfoJSON(JsonData);

            String resp = emptyString;
            serializeJson(JsonData, resp);
            // DEBUG_V (String ("JsonDoc: ") + resp);
            request->send(200, F("application/json"), resp);

            break;
        }

        if (command == F("getHostNameInfo"))
        {
            JsonData[F("HostName")] = WiFi.getHostname();
            JsonData[F("HostDescription")] = F("Pixel Radio");

            String resp;
            serializeJson(JsonData, resp);
            // DEBUG_V (String ("resp: ") + resp);
            request->send(200, F("application/json"), resp);

            break;
        }

        if (command == F("getChannelOutputs"))
        {
            if (request->hasParam(F("file")))
            {
                String filename = request->getParam(F("file"))->value();
                if (String(F("co-other")) == filename)
                {
                    String resp;
                    // DEBUG_V (String ("resp: ") + resp);
                    request->send(200, F("application/json"), resp);

                    break;
                }
            }
        }

        // DEBUG_V (String ("Unknown command: ") + command);
        request->send(404);

    } while (false);

    // DEBUG_END;

} // ProcessFPPJson

c_FPPDiscovery FPPDiscovery;
