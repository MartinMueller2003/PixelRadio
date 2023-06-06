/*
  *    File: ControllerHTTP.cpp
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
#include <ArduinoLog.h>
#include "ControllerHTTP.h"
#include "Language.h"

#include "memdebug.h"

// *********************************************************************************************

static const PROGMEM char Name []   = "HTTP";
static const uint16_t   HTTP_PORT   = 8080;     // Port for HTTP commands
static AsyncWebServer   webServer (HTTP_PORT);  // Web Server

// *********************************************************************************************
void PrettyPrint (DynamicJsonDocument & jsonStuff, String Name)
{
    // DEBUG_V ("---------------------------------------------");
    LOG_PORT.println (String (F ("---- Pretty Print: '")) + Name + "'");
    serializeJson (jsonStuff, LOG_PORT);
    LOG_PORT.println ("");
    // DEBUG_V ("---------------------------------------------");
}  // PrettyPrint

// *********************************************************************************************
void PrettyPrint (JsonArray & jsonStuff, String Name)
{
    // DEBUG_V ("---------------------------------------------");
    LOG_PORT.println (String (F ("---- Pretty Print: '")) + Name + "'");
    serializeJson (jsonStuff, LOG_PORT);
    LOG_PORT.println ("");
    // DEBUG_V ("---------------------------------------------");
}  // PrettyPrint

// *********************************************************************************************
void PrettyPrint (JsonObject & jsonStuff, String Name)
{
    // DEBUG_V ("---------------------------------------------");
    LOG_PORT.println (String (F ("---- Pretty Print: '")) + Name + "'");
    serializeJson (jsonStuff, LOG_PORT);
    LOG_PORT.println ("");
    // DEBUG_V ("---------------------------------------------");
}  // PrettyPrint

// *********************************************************************************************
c_ControllerHTTP::c_ControllerHTTP () :   cControllerCommon (Name, c_ControllerMgr::ControllerTypeId_t::HTTP_CNTRL)
{}  // c_ControllerHTTP

// *********************************************************************************************
c_ControllerHTTP::~c_ControllerHTTP ()
{}

// *********************************************************************************************
bool c_ControllerHTTP::GetNextRdsMessage (const String & value, c_ControllerMgr::RdsMsgInfo_t & Response)
{
    // DEBUG_START;

    if (!AllMessagesPlayed || ControllerIsEnabled ())
    {
        AllMessagesPlayed = Messages.GetNextRdsMessage (value, Response);
    }

    // DEBUG_END;
    return AllMessagesPlayed;
}

// *********************************************************************************************
// Configure and start the web server
void c_ControllerHTTP::init ()
{
    // DEBUG_START;
    // Add header for SVG plot support?
    DefaultHeaders::Instance ().addHeader (F ("Access-Control-Allow-Origin"),  "*");
    DefaultHeaders::Instance ().addHeader (
        F ("Access-Control-Allow-Headers"),
        "append, delete, entries, foreach, get, has, keys, set, values, Authorization, Content-Type, Content-Range, Content-Disposition, Content-Description, cache-control, x-requested-with");
    DefaultHeaders::Instance ().addHeader (F ("Access-Control-Allow-Methods"), "GET, HEAD, POST, PUT, DELETE, CONNECT, OPTIONS, TRACE, PATCH");

    // Heap status handler
    webServer.on (
        "/heap",
        HTTP_GET,
        [] (AsyncWebServerRequest * request)
        {
            // DEBUG_V("Got a heap request")
            request->send (200, String (F ("text/plain")), String (ESP.getFreeHeap ()).c_str ());
        });

    // webServer.serveStatic ("/", LittleFS, "/www/").setDefaultFile ("index.html");
    // Heap status handler
    webServer.on (
        "/cmd",
        HTTP_ANY,
        [this] (AsyncWebServerRequest * request)
        {
            // DEBUG_V(String("              url: ") + request->url());
            uint32_t numParams = request->params ();
            // DEBUG_V(String("        numParams: ") + String(numParams));
            if (numParams)
            {
                for (uint32_t CurrentParamIndex = 0;CurrentParamIndex < numParams;++CurrentParamIndex)
                {
                    String Response;
                    String CommandName  = request->getParam (size_t(CurrentParamIndex))->name ();
                    String Parameter    = request->getParam (size_t(CurrentParamIndex))->value ();

                    // DEBUG_V(String("      getParam[" + String(CurrentParamIndex) + "]: ") + CommandName);
                    // DEBUG_V(String("      getParam[" + String(CurrentParamIndex) + "]: ") + Parameter);

                    CommandProcessor.ProcessCommand (CommandName, Parameter, Response);
                    request->send (200, String (F ("text/plain")), Response);
                }
            }
            else
            {
                request->send (200, String (F ("text/plain")), "No parameters");
            }
        });

    webServer.onNotFound (
        [this] (AsyncWebServerRequest * request)
        {
            request->send (404, String (F ("text/plain")), "Page Not found");
        });

    webServer.begin ();

    Log.infoln ((String (F ("Web server listening on port ")) + HTTP_PORT).c_str ());

    // DEBUG_END;
}

// *********************************************************************************************
void c_ControllerHTTP::poll ()
{
    // _ DEBUG_START;

    if (!HasBeenInitialized)
    {
        if (WiFiDriver.IsWiFiConnected () && ControllerIsEnabled ())
        {
            init ();
            HasBeenInitialized = true;
        }
    }

    // _ DEBUG_END;
}

// *********************************************************************************************
// EOF
