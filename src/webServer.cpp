/*
   File: webserver.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Notes:
   1. The HTTP GET server uses port 8080 (default).
   2. If host name is changed then FLASH must be fully erased before loading new code. Platformio command: pio run -t erase
   3. mDNS can be excluded from build, see config.h. Default mDNS access is 'PixelRadio.local", but can be changed in the WebUI.
   4. Use Android "Service Browser" app for mDNS Host name debugging. PixelRadio will be found in the android.tcp section.

   gif to base64 conversion tool (select Plain text -- just the Base64 Value):
   https://base64.guru/converter/encode/image/gif

 */

// *********************************************************************************************

#include "config.h"
#include "credentials.h"
#include "ESPUI.h"
#include "globals.h"
#include "language.h"
#include "PixelRadio.h"
#include <ArduinoLog.h>
#include <WiFi.h>

// ************************************************************************************************

WiFiServer server (HTTP_PORT);  // WiFi WebServer object.

#ifdef oldWay

    // ************************************************************************************************
    // getCommandArg(): Get the Webserver's Command Argument. Result is returned as String passed by reference.
    // Also returns String Length. If argument missing then returns -1;
 # ifdef HTTP_ENB
        int16_t getCommandArg (String & requestStr, uint8_t maxSize)
        {
            int16_t     argStart        = 0;
            int16_t     argStop         = 0;
            String      argStr          = "";   // Copy of Argument String.
            String      argLcStr        = "";   // Lowercase version.

            argLcStr = requestStr;
            argLcStr.toLowerCase ();
            argStart = requestStr.indexOf ("=");

            if (argStart >= 0)
            {
                argStop = argLcStr.indexOf (HTTP_CMD_END_STR);  // Search for end of command data arg.

                if (argStop < 0)
                {
                    argStop = requestStr.length ();
                }
                argStr  = requestStr.substring (argStart + 1, argStop - 1);
                argStr  = urlDecode (argStr);   // Convert any URL encoded text to ASCII.
                //      argStr.replace("%20", " ");  // Replace the html space with ASCII text.
                argStr. trim ();

                if (argStr.length () > maxSize)
                {
                    argStr = argStr.substring (0, maxSize);
                }
                else if (argStr.length () == 0)
                {
                    return -1;          // Fail, Argument Missing.
                }
                argStr. trim ();        // Trim one more time.
                requestStr = argStr;
            }
            else                        // Fail, Improper Argument Provided.
            {
                requestStr = "";

                return -1;
            }
            return requestStr.length ();
        }

 # endif // ifdef HTTP_ENB

#endif // def OldWay

// *************************************************************************************************************************
// gpioHttpControl(): HTTP handler for GPIO Commands. This is a companion to processWebClient().
#ifdef HTTP_ENB
    void gpioHttpControl (WiFiClient client, String requestStr, uint8_t pin)
    {
 # ifdef OldWay
            bool        successFlg = true;
            char        charBuff[60];

            sprintf (charBuff, "-> HTTP Controller: Received GPIO Pin-%d Command", pin);
            Log.infoln (charBuff);

            successFlg = gpioCmd (requestStr, HttpControllerId, pin);

            client.     print ( HTML_HEADER_STR);
            client.     print ( HTML_DOCTYPE_STR);

            if (!successFlg)
            {
                sprintf (charBuff, "{\"%s%d\": \"fail\"}", CMD_GPIO_STR, pin);
            }
            else if (requestStr == CMD_GPIO_READ_STR)
            {
                sprintf (charBuff, "{\"%s%d\": \"%d\"}", CMD_GPIO_STR, pin, digitalRead (pin));
            }
            else
            {
                sprintf (charBuff, "{\"%s%d\": \"ok\"}", CMD_GPIO_STR, pin);
            }
            client.print (charBuff);
            client.println (HTML_CLOSE_STR);
 # endif // def OldWay
    }

#endif // ifdef HTTP_ENB

// ************************************************************************************************
// makeHttpCmdStr(): Return the HTTP command string. On entry cmdStr has command keyword.
//                   This is a companion to processWebClient().
#ifdef HTTP_ENB
    String makeHttpCmdStr (String cmdStr)
    {
        cmdStr = String (HTTP_CMD_STR) + cmdStr;

        return cmdStr;
    }

#endif // ifdef HTTP_ENB

// ************************************************************************************************
// processWebClient(): Web Client Handler. Must be installed in main loop.
// URL Example: http://pixelradio.local:8080/cmd?aud=mono
#ifdef HTTP_ENB
    void processWebClient (void)
    {
 # ifdef OldWay
            static bool connectFlg      = false;
            bool successFlg             = true;
            uint16_t    charCnt         = 0;
            uint32_t    previousTime    = millis ();

            String      argStr          = "";
            String      currentLine     = "";
            String      requestStr      = "";           // Var to capture the HTTP request reply.
            String      requestLcStr    = "";           // Var to store the lower case HTTP request reply.

            WiFiClient client = server.available ();    // Listen for incoming web clients.

            if (client)                                 // New client connection (web page access).
            {
                requestStr.reserve (HTTP_RESPONSE_MAX_SZ + 10);
                requestLcStr.reserve (HTTP_RESPONSE_MAX_SZ + 10);

                if (!connectFlg)
                {
                    Log.infoln ("HTTP Controller: New Client");
                    previousTime = millis ();
                }

                if (!client.available ())       // If no bytes available then this is a Browser Prefetch.
                {
                    Log.infoln ("-> HTTP Controller: Empty Prefetch, Close Connection.");
                    client.stop ();

                    return;
                }
                connectFlg      = true;
                currentLine     = "";   // Init string to hold incoming data from the client,

                // while (client.connected()) { // loop while the client is connected.
                while (client.connected () && millis () - previousTime <= CLIENT_TIMEOUT)
                {
                    if (client.available ())
                    {
                        char c = client.read ();

                        if (charCnt < HTTP_RESPONSE_MAX_SZ)
                        {
                            // Serial.write(c); // DEBUG ONLY: Log saved URI chars to serial monitor.
                            requestStr += c;    // Build URI string.
                            charCnt++;
                        }

                        if (c == '\n')
                        {
                            // if the current line is zero, you got two newline characters in a row.
                            // that's the end of the client HTTP request body, so check for the
                            // optional post data. Then process the command.
                            if (currentLine.length () == 0)
                            {
                                // Log.verboseln(requestStr.c_str());
                                requestLcStr = requestStr;
                                requestLcStr.toLowerCase ();

                                // ************ PROCESS POST DATA (if any) ***************
                                uint16_t contentLenIndex = requestLcStr.indexOf (HTTP_POST_STR);

                                if (contentLenIndex > 0)
                                {
                                    String lenStr = requestLcStr.substring (contentLenIndex + sizeof (HTTP_POST_STR));

                                    if (lenStr.toInt () > 0)
                                    {
                                        Log.verboseln ("-> HTTP CMD: Found Post Data");

                                        uint16_t endCmdIndex = requestLcStr.indexOf (HTTP_CMD_END_STR);

                                        if (endCmdIndex > 0)
                                        {
                                            requestStr =  requestStr.substring (0, endCmdIndex);
                                        }

                                        while (client.available () && charCnt < HTTP_RESPONSE_MAX_SZ)
                                        {
                                            char c = client.read ();

                                            if ((c == '\r') || (c == '\n'))
                                            {
                                                break;
                                            }
                                            // Serial.write(c); // DEBUG ONLY: Log saved URI chars to serial monitor.
                                            requestStr += c;    // Build URI string.
                                            charCnt++;
                                        }
                                        requestStr      += ' '; // Must pad end of Post data with Space (needed by cmd processor).
                                        requestLcStr    = requestStr;
                                        requestLcStr.toLowerCase ();
                                    }
                                }

                                // ************ NO COMMAND, EMPTY PAYLOAD ***************
                                if (requestLcStr.indexOf (HTTP_EMPTY_RESP_STR) == 0)
                                {
                                    Log.verboseln ("-> HTTP CMD: Empty Payload, Ignored");
                                    break;
                                }

                                // ************ AUDIO MODE COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_AUDMODE_STR)) > 0)
                                {
                                    Log.infoln ("-> HTTP CMD: Audio Mode");

                                    if (getCommandArg (requestStr, CMD_AUD_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: Audio Mode Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (audioModeCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_AUDMODE_STR);
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_AUDMODE_STR);
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************ GPIO19 COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_GPIO19_STR)) > 0)
                                {
                                    Log.infoln ("-> HTTP CMD: GPIO19");

                                    if (getCommandArg (requestStr, CMD_GPIO_MAX_SZ) == -1)
                                    {
                                        Log.errorln ("-> HTTP CMD: GPIO Control Missing Value (abort).");
                                    }
                                    else
                                    {
                                        gpioHttpControl (client, requestStr, GPIO19_PIN);
                                    }
                                }

                                // ************ GPIO23 COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_GPIO23_STR)) > 0)
                                {
                                    Log.infoln ("-> HTTP CMD: GPIO23");

                                    if (getCommandArg (requestStr, CMD_GPIO_MAX_SZ) == -1)
                                    {
                                        Log.errorln ("-> HTTP CMD: GPIO Control Missing Value (abort).");
                                    }
                                    else
                                    {
                                        gpioHttpControl (client, requestStr, GPIO23_PIN);
                                    }
                                }

                                // ************ GPIO33 COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_GPIO33_STR)) > 0)
                                {
                                    Log.infoln ("-> HTTP CMD: GPIO33");

                                    if (getCommandArg (requestStr, CMD_GPIO_MAX_SZ) == -1)
                                    {
                                        Log.errorln ("-> HTTP CMD: GPIO Control Missing Value (abort).");
                                    }
                                    else
                                    {
                                        gpioHttpControl (client, requestStr, GPIO33_PIN);
                                    }
                                }

                                // ************ INFO COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_INFO_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: INFO");

                                    if (getCommandArg (requestStr, CMD_SYS_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: Info Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (infoCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        String HostName;
                                        WiFiDriver.GetHostName (HostName);
                                        client.printf (
                                            "{\"%s\": \"ok\", \"version\": \"%s\", \"hostName\": \"%s\", \"ip\": \"%s\", \"rssi\": %d, \"status\": \"0x%04X\"}\r\n",
                                            CMD_INFO_STR,
                                            VERSION_STR,
                                            HostName.c_str (),
                                            WiFi.localIP ().toString ().c_str (),
                                            WiFi.RSSI (),
                                            ControllerMgr.getControllerStatusSummary ());
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_INFO_STR);
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************ FREQUENCY COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_FREQ_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: FREQUENCY");

                                    if (getCommandArg (requestStr, CMD_FREQ_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: Frequency Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (frequencyCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_FREQ_STR);
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_FREQ_STR);
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************ MUTE COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_MUTE_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: MUTE");

                                    if (getCommandArg (requestStr, CMD_MUTE_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: Mute Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (muteCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_MUTE_STR);
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_MUTE_STR);
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************* PI CODE COMMAND ****************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_PICODE_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: PI (Program ID) CODE");

                                    if (getCommandArg (requestStr, CMD_PI_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: PI Code Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (piCodeCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_PICODE_STR);
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_PICODE_STR);
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }
  #  ifdef OldWay
                                    // ************* PTY CODE COMMAND ****************
                                    else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_PTYCODE_STR)) >= 0)
                                    {
                                        Log.infoln ("-> HTTP CMD: PTY CODE");

                                        if (getCommandArg (requestStr, CMD_PTY_MAX_SZ) == -1)
                                        {
                                            successFlg = false;
                                            Log.errorln ("-> HTTP CMD: PTY Code Missing Value (abort).");
                                        }
                                        else
                                        {
                                            if (ptyCodeCmd (requestStr, HttpControllerId) == false)
                                            {
                                                successFlg = false;
                                            }
                                        }
                                        client. print ( HTML_HEADER_STR);
                                        client. print ( HTML_DOCTYPE_STR);

                                        if (successFlg)
                                        {
                                            client.printf ("{\"%s\": \"ok\"}\r\n", CMD_PTYCODE_STR);
                                        }
                                        else
                                        {
                                            client.printf ("{\"%s\": \"fail\"}\r\n", CMD_PTYCODE_STR);
                                        }
                                        client.println (HTML_CLOSE_STR);
                                    }

                                    // ********PROGRAM SERVICE NAME COMMAND *********
                                    else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_PSN_STR)) >= 0)
                                    {
                                        Log.infoln ("-> HTTP CMD: PSN (Program Service Name)");

                                        if (getCommandArg (requestStr, RDS_PSN_MAX_SZ) == -1)
                                        {
                                            successFlg = false;
                                            Log.errorln ("-> HTTP CMD: PSN Missing Value, (abort).");
                                        }
                                        else
                                        {
                                            if (programServiceNameCmd (requestStr, HttpControllerId) == false)
                                            {
                                                successFlg = false;
                                            }
                                        }
                                        client. print ( HTML_HEADER_STR);
                                        client. print ( HTML_DOCTYPE_STR);

                                        if (successFlg)
                                        {
                                            client.printf ("{\"%s\": \"ok\"}\r\n", CMD_PSN_STR);
                                        }
                                        else
                                        {
                                            client.printf ("{\"%s\": \"fail\"}\r\n", CMD_PSN_STR);
                                        }
                                        client.println (HTML_CLOSE_STR);
                                    }
  #  endif // def OldWay

                                // ************ RT COMMAND ***************
                                // To clear RadioText display send %20 as payload.
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_RADIOTEXT_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: RTM (RadioText Message)");

                                    if (getCommandArg (requestStr, RDS_TEXT_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: RTM, Missing RadioText Message, Ignored.");
                                    }
                                    else
                                    {
                                        if (radioTextCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_RADIOTEXT_STR);      // JSON Fmt.
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_RADIOTEXT_STR);    // JSON Fmt.
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************ RFC COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_RF_CARRIER_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: RFC (RF Carrier Control)");

                                    if (getCommandArg (requestStr, CMD_RF_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: RFC, Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (rfCarrierCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_RF_CARRIER_STR);     // JSON Fmt.
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_RF_CARRIER_STR);   // JSON Fmt.
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************ REBOOT COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_REBOOT_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: REBOOT");

                                    if (getCommandArg (requestStr, CMD_SYS_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: REBOOT, Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (rebootCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_REBOOT_STR); // JSON Fmt.
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_REBOOT_STR);       // JSON Fmt.
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************** START COMMAND ***************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_START_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: START");

                                    if (getCommandArg (requestStr, CMD_RDS_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: START, Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (startCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_START_STR);  // JSON Fmt.
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_START_STR);        // JSON Fmt.
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // **************** STOP COMMAND ****************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_STOP_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: STOP");

                                    if (getCommandArg (requestStr, CMD_RDS_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: STOP, Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (stopCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_STOP_STR);   // JSON Fmt.
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_STOP_STR); // JSON Fmt.
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************* TIME PERIOID COMMAND ************
                                else if (requestLcStr.indexOf (makeHttpCmdStr (CMD_PERIOD_STR)) >= 0)
                                {
                                    Log.infoln ("-> HTTP CMD: RTPER (RadioText Time Period)");

                                    if (getCommandArg (requestStr, CMD_TIME_MAX_SZ) == -1)
                                    {
                                        successFlg = false;
                                        Log.errorln ("-> HTTP CMD: RTPER, Missing Value (abort).");
                                    }
                                    else
                                    {
                                        if (rdsTimePeriodCmd (requestStr, HttpControllerId) == false)
                                        {
                                            successFlg = false;
                                        }
                                    }
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);

                                    if (successFlg)
                                    {
                                        client.printf ("{\"%s\": \"ok\"}\r\n", CMD_PERIOD_STR); // JSON Fmt.
                                    }
                                    else
                                    {
                                        client.printf ("{\"%s\": \"fail\"}\r\n", CMD_PERIOD_STR);       // JSON Fmt.
                                    }
                                    client.println (HTML_CLOSE_STR);
                                }

                                // ************ UNKNOWN COMMAND ***************
                                else
                                {
                                    Log.errorln ("-> HTTP CMD: COMMAND IS UNDEFINED");
                                    client.     print ( HTML_HEADER_STR);
                                    client.     print ( HTML_DOCTYPE_STR);
                                    client.     println (       "{\"cmd\": \"undefined\"}\r\n");        // JSON Fmt.
                                    client.     println (       HTML_CLOSE_STR);
                                }

                                // Break out of the while loop
                                break;
                            }
                            else        // if you got a newline, then clear currentLine
                            {
                                currentLine = "";
                            }
                        }
                        else if (c != '\r')     // if you got anything else but a carriage return character,
                        {
                            currentLine += c;   // add it to the end of the currentLine
                        }
                    }
                }
                requestStr      = "";   // Clear the reply request variable.
                requestLcStr    = "";

                connectFlg = false;
                client.stop ();         // Close the GET HTTP connection.
                Log.infoln ("-> HTTP Controller: Client Disconnected.");
            }
            else if (connectFlg)        // Client was connected, but now nothing to do.
            {
                connectFlg = false;
                client.stop ();         // Close the GET HTTP connection.
                Log.infoln ("-> HTTP Controller: Connected Client Now Idle, Disconnected).");
            }
 # endif // def OldWay
    }

#endif // ifdef HTTP_ENB

#ifdef OldWay
    // *********************************************************************************************
    // urlDecode(): Convert URL encoding into ASII.
    String urlDecode (String urlStr)
    {
        String  encodeStr = "";
        char    c;
        char    code0;
        char    code1;

        for (int i = 0; i < urlStr.length (); i++)
        {
            c = urlStr.charAt (i);

            if (c == '%')
            {
                code0           = urlStr.charAt (++i);
                code1           = urlStr.charAt (++i);
                c               = (urlDecodeHex (code0) << 4) | urlDecodeHex (code1);
                encodeStr       += c;
            }
            else if (c == '+')
            {
                encodeStr += ' ';
            }
            else
            {
                encodeStr += c;
            }
        }

        return encodeStr;
    }

    // *********************************************************************************************
    // urlDecodeHex(): convert hex to integer base. This is companion function for urlDecode().
    unsigned char urlDecodeHex (char c)
    {
        if ((c >= '0') && (c <= '9'))
        {
            return (unsigned char)c - '0';
        }

        if ((c >= 'a') && (c <= 'f'))
        {
            return (unsigned char)c - 'a' + 10;
        }

        if ((c >= 'A') && (c <= 'F'))
        {
            return (unsigned char)c - 'A' + 10;
        }
        return 0;
    }

#endif // def OldWay
