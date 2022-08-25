/*
    WiFiDriver.cpp - Output Management class
    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
    Version: 1.1.0
    Creation: Dec-16-2021
    Revised:  Jun-13-2022
    Revision History: See PixelRadio.cpp
    Project Leader: T. Black (thomastech)
    Contributors: thomastech, MartinMueller2003

    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
 */

#include <ArduinoLog.h>
#include <esp_wifi.h>
#include <Int64String.h>

#include "WiFiDriver.hpp"

#include "HotspotName.hpp"
#include "HostnameCtrl.hpp"
#include "MdnsName.hpp"
#include "SSID.hpp"
#include "DHCP.hpp"
#include "WpaKey.hpp"
#include "ApIpAddress.hpp"
#include "ApFallback.hpp"
#include "ApReboot.hpp"
#include "PixelRadio.h"
#include "StaticIpAddress.hpp"
#include "StaticGatewayAddress.hpp"
#include "StaticNetmask.hpp"
#include "StaticDnsAddress.hpp"

#include "memdebug.h"

// -----------------------------------------------------------------------------
/*
    There are four ways to define the default Network Name and PassPhrase

    1) Create a secrets.h file and place it in the network directory with the WiFiDriver.cpp file
    2) Use platformio_user.ini to define the WiFi Credentials for your platform
    3) Edit the strings below directly
    4) Place the credentials in a file on the SD card.

       SD Card Emergency Credential Restoration Feature:
   -------------------------------------------------
   Credential Restoration allows the User to change WiFI credentials by SD Card. Reverts to DHCP WiFi.
   (1) File Creation:
    Create file named credentials.txt and add the following JSON formatted text to it (fill in your SSID and WPA_KEY):
    {""WIFI_SSID_STR"":"SSID",
    ""WIFI_WPA_KEY_STR"":"WPA_KEY"}
   (2) Instructions:
       Install your prepared SD Card in PixelRadio. Reboot. Wait 30 secs, Remove card.
       Note: For Security the File is automatically deleted from card.

 */

#if __has_include ("credentials.h")
 # include "credentials.h"
#endif //  __has_include("credentials.h")

#if !defined (SSID_NM_STR)
 # define SSID_NM_STR "DEFAULT_SSID_NOT_SET"
#endif // SSID_NM_STR

#if !defined (WPA_KEY_STR)
 # define WPA_KEY_STR "DEFAULT_PASSPHRASE_NOT_SET"
#endif // WPA_KEY_STR

#if !defined (AP_PSK_STR)
 # define AP_PSK_STR "DEFAULT_AP_PASSPHRASE_NOT_SET"
#endif // AP_PSK_STR

/* Fallback configuration if config is empty or fails */
const PROGMEM char  default_ssid             [] = SSID_NM_STR;
const PROGMEM char  default_passphrase       [] = WPA_KEY_STR;
const PROGMEM char  default_AP_passphrase    [] = AP_PSK_STR;

#define DNS_PORT 53 // Webserver DNS port.

/*****************************************************************************/
/* FSM                                                                       */
/*****************************************************************************/
fsm_WiFi_state_Boot fsm_WiFi_state_Boot_imp;
fsm_WiFi_state_ConnectingUsingConfig fsm_WiFi_state_ConnectingUsingConfig_imp;
fsm_WiFi_state_ConnectingUsingDefaults fsm_WiFi_state_ConnectingUsingDefaults_imp;
fsm_WiFi_state_ConnectedToAP fsm_WiFi_state_ConnectedToAP_imp;
fsm_WiFi_state_ConnectingAsAP   fsm_WiFi_state_ConnectingAsAP_imp;
fsm_WiFi_state_ConnectedToSta   fsm_WiFi_state_ConnectedToSta_imp;
fsm_WiFi_state_ConnectionFailed fsm_WiFi_state_ConnectionFailed_imp;
fsm_WiFi_state_Disabled fsm_WiFi_state_Disabled_imp;

// -----------------------------------------------------------------------------
///< Start up the driver and put it into a safe mode
c_WiFiDriver::c_WiFiDriver () :   c_WiFiDriverUi ()
{}  // c_WiFiDriver

// -----------------------------------------------------------------------------
///< deallocate any resources and put the output channels into a safe state
c_WiFiDriver::~c_WiFiDriver ()
{
    // DEBUG_START;

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::AnnounceState ()
{
    // DEBUG_START;

    String StateName;

    pCurrentFsmState->GetStateName (StateName);
    Log.verboseln ((String (F ("WiFi Entering State: ")) + StateName).c_str ());

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::Begin ()
{
    // DEBUG_START;

    fsm_WiFi_state_Boot_imp.SetParent (this);
    fsm_WiFi_state_ConnectingUsingConfig_imp.SetParent (this);
    fsm_WiFi_state_ConnectingUsingDefaults_imp.SetParent (this);
    fsm_WiFi_state_ConnectedToAP_imp.SetParent (this);
    fsm_WiFi_state_ConnectingAsAP_imp.SetParent (this);
    fsm_WiFi_state_ConnectedToSta_imp.SetParent (this);
    fsm_WiFi_state_ConnectionFailed_imp.SetParent (this);
    fsm_WiFi_state_Disabled_imp.SetParent (this);

    // DEBUG_V();
    fsm_WiFi_state_Boot_imp.Init ();
    // DEBUG_V();

    // Check for Emergency WiFi Credential File on SD Card.
    if (checkEmergencyCredentials (CRED_FILE_NAME))
    {
        // saveConfiguration(LITTLEFS_MODE, BACKUP_FILE_NAME); // Save restored credentials to file system.
    }
#ifdef FixMe

        if (FileMgr.SdCardIsInstalled ())
        {
            DynamicJsonDocument jsonConfigDoc (1024);

            // DEBUG_V ("read the sdcard config");
            if (FileMgr.ReadSdFile (F ("wificonfig.json"), jsonConfigDoc))
            {
                // DEBUG_V ("Process the sdcard config");
                JsonObject jsonConfig = jsonConfigDoc.as <JsonObject>();

                // copy the fields of interest into the local structure
                setFromJSON (   ssid,       jsonConfig, "WIFI_SSID_STR");
                setFromJSON (   passphrase, jsonConfig, "WIFI_WPA_KEY_STR");

                ConfigSaveNeeded = true;

                FileMgr.DeleteSdFile (F ("wificonfig.json"));
            }
            else
            {
                // DEBUG_V ("ERROR: Could not read SD card config");
            }
        }
#endif // def FixMe

    // Disable persistant credential storage and configure SDK params
    WiFi.persistent (false);
    esp_wifi_set_ps (WIFI_PS_NONE);

    // DEBUG_V ("");

    // Setup WiFi Handlers
    WiFi.   onEvent ([this] (WiFiEvent_t event, arduino_event_info_t info)
                         {
                             this->onWiFiStaConn (event, info);
                         }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

    WiFi.   onEvent ([this] (WiFiEvent_t event, arduino_event_info_t info)
                         {
                             this->onWiFiStaDisc (event, info);
                         }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    WiFi.   onEvent ([this] (WiFiEvent_t event, arduino_event_info_t info)
                         {
                             this->onWiFiConnect    (event, info);
                         }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);

    WiFi.   onEvent ([this] (WiFiEvent_t event, arduino_event_info_t info)
                         {
                             this->onWiFiDisconnect (event, info);
                         }, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    // set up the poll interval
    NextPollTime = millis () + PollInterval;

    // Main loop should start polling for us

    // DEBUG_END;
}   // begin

// -----------------------------------------------------------------------------
void c_WiFiDriver::connectWifi (const String & current_ssid, const String & current_passphrase)
{
    // DEBUG_START;

    do  // once
    {
        // WiFi reset flag is set which will be handled in the next iteration of the main loop.
        // Ignore connect request to lessen boot spam.
        if (ResetWiFi)
        {
            // DEBUG_V ("WiFi Reset Requested");
            break;
        }
        SetUpIp ();

        // DEBUG_V(String("Hostname: ") + Hostname);

        // Hostname must be set before on ESP32
        WiFi.persistent (false);
        // DEBUG_V("");
        WiFi.disconnect (true);
        // DEBUG_V("");

        if (!HostnameCtrl.get ().isEmpty ())
        {
            // DEBUG_V(String("Setting WiFi Hostname: ") + Hostname.getStr());
            WiFi.hostname (HostnameCtrl.get ());
        }
        // DEBUG_V("Setting WiFi Mode to STA");
        WiFi.enableAP (false);
        // DEBUG_V();
        WiFi.enableSTA (true);

        // DEBUG_V (String ("      ssid: ") + current_ssid);
        // DEBUG_V (String ("passphrase: ") + current_passphrase);
        // DEBUG_V (String ("  hostname: ") + Hostname);

        Log.infoln ((String (F ("Connecting to '")) +
                     current_ssid +
                     String (F ("' as ")) +
                     HostnameCtrl.get ()).c_str ());

        WiFi.setSleep (false);
        // DEBUG_V("");
        WiFi.begin (current_ssid.c_str (), current_passphrase.c_str ());
    } while (false);

    // DEBUG_END;
}   // connectWifi

// -----------------------------------------------------------------------------
void c_WiFiDriver::Disable ()
{
    // DEBUG_START;

    if (pCurrentFsmState != & fsm_WiFi_state_Disabled_imp)
    {
        // DEBUG_V ();
        WiFi.enableSTA (false);
        WiFi.enableAP (false);
        fsm_WiFi_state_Disabled_imp.Init ();
    }
    // DEBUG_END;
}   // Disable

// -----------------------------------------------------------------------------
void c_WiFiDriver::Enable ()
{
    // DEBUG_START;

    if (pCurrentFsmState == & fsm_WiFi_state_Disabled_imp)
    {
        // DEBUG_V ();
        WiFi.enableSTA (true);
        WiFi.enableAP (false);
        fsm_WiFi_state_ConnectionFailed_imp.Init ();
    }
    else
    {
        // DEBUG_V (String ("WiFi is not disabled"));
    }
    // DEBUG_END;
}   // Enable

// -----------------------------------------------------------------------------
String  c_WiFiDriver::GetDefaultWpaKey ()           {return default_passphrase;}

// -----------------------------------------------------------------------------
String  c_WiFiDriver::GetDefaultSsid ()             {return default_ssid;}

// -----------------------------------------------------------------------------
void    c_WiFiDriver::GetHostname (String & name)   {name = WiFi.getHostname ();}   // GetWiFiHostName

// -----------------------------------------------------------------------------
void    c_WiFiDriver::GetStatus (JsonObject & jsonStatus)
{
#ifdef OldWay
        // DEBUG_START;

        jsonStatus[F ("WIFI_RSSI")]         = WiFiRssi.get ();
        jsonStatus[F ("WIFI_IP_ADDR_STR")]  = getIpAddress ().toString ();
        jsonStatus[F ("WIFI_SUBNET_STR")]   = getIpSubNetMask ().toString ();
        jsonStatus[F ("WIFI_MAC_STR")]      = WiFi.macAddress ();
        jsonStatus[F ("WIFI_SSID_STR")]     = WiFi.SSID ();
        jsonStatus[F ("WIFI_CONNECTED")]    = IsWiFiConnected ();

        // DEBUG_END;
#endif // def OldWay
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::onWiFiStaConn (const WiFiEvent_t event, const WiFiEventInfo_t info)
{
    // DEBUG_V ("ESP has associated with the AP");
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::onWiFiStaDisc (const WiFiEvent_t event, const WiFiEventInfo_t info)
{
    // DEBUG_V ("ESP has disconnected from the AP");
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::onWiFiConnect (const WiFiEvent_t event, const WiFiEventInfo_t info)
{
    // DEBUG_START;

    pCurrentFsmState->OnConnect ();

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::onWiFiDisconnect (const WiFiEvent_t event, const WiFiEventInfo_t info)
{
    // DEBUG_START;

    pCurrentFsmState->OnDisconnect ();

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::Poll ()
{
    // _ DEBUG_START;

    if (millis () > NextPollTime)
    {
        // _ DEBUG_V ("Start Poll");
        NextPollTime += PollInterval;
        pCurrentFsmState->Poll ();
        // _ DEBUG_V ("End Poll");
    }

    if (ResetWiFi)
    {
        // DEBUG_V("Reset Triggered");
        ResetWiFi = false;
        reset ();
    }
    // _ DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::reset ()
{
    // DEBUG_START;

    fsm_WiFi_state_Boot_imp.Init ();

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
bool c_WiFiDriver::restoreConfiguration (JsonObject & json)
{
    // DEBUG_START;

    bool ConfigChanged = false;

    SSID.restoreConfiguration (json);
    WpaKey.restoreConfiguration (json);
    DHCP.restoreConfiguration (json);
    HostnameCtrl.restoreConfiguration (json);
    HotspotName.restoreConfiguration (json);
    ApIpAddress.restoreConfiguration (json);
    ApFallback.restoreConfiguration (json);
    ApReboot.restoreConfiguration (json);

    ConfigChanged   |= ReadFromJSON (sta_timeout,                     json, F ("WIFI_STA_TIMEOUT"));
    ConfigChanged   |= ReadFromJSON (ap_timeout,                      json, F ("WIFI_AP_TIMEOUT"));

    // DEBUG_V ("     ip: " + ip);
    // DEBUG_V ("gateway: " + gateway);
    // DEBUG_V ("staticNetmask: " + staticNetmask);
    // DEBUG_V ("   ApIp: " + ApIp);

    // DEBUG_V (String("ConfigChanged: ") + String(ConfigChanged));
    // DEBUG_END;
    return ConfigChanged;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::saveConfiguration (JsonObject & json)
{
    // DEBUG_START;

    SSID.saveConfiguration (json);
    WpaKey.saveConfiguration (json);
    DHCP.saveConfiguration (json);
    HostnameCtrl.saveConfiguration (json);
    HotspotName.saveConfiguration (json);
    ApIpAddress.restoreConfiguration (json);
    ApFallback.restoreConfiguration (json);
    ApReboot.restoreConfiguration (json);

    json["WIFI_STA_TIMEOUT"]    = sta_timeout;
    json["WIFI_AP_TIMEOUT"]     = ap_timeout;

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::SetFsmState (fsm_WiFi_state * NewState)
{
    // DEBUG_START;
    // DEBUG_V (String ("pCurrentFsmState: ") + String (uint32_t (pCurrentFsmState), HEX));
    // DEBUG_V (String ("        NewState: 0x") + String (uint32_t (NewState), HEX));
    pCurrentFsmState = NewState;
    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::SetHostname (String &)
{
    // DEBUG_START;

    // Need to reset the WiFi sub system if the host name changes
    reset ();

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::SetUpIp ()
{
    // DEBUG_START;

    do  // once
    {
        if (DHCP.get ())
        {
            Log.verboseln (String (F ("Using DHCP")).c_str ());
            break;
        }
        Log.infoln (String (F ("Using Static IP")).c_str ());
        WiFi.config (StaticIpAddress.GetIpAddress (),
                     StaticGatewayAddress.GetIpAddress (),
                     StaticNetmask.GetIpAddress (),
                     StaticDnsAddress.GetIpAddress ());
    } while (false);

    // DEBUG_END;
}

// -----------------------------------------------------------------------------
int c_WiFiDriver::ValidateConfig ()
{
    // DEBUG_START;

    int response = 0;

    if (sta_timeout < 5)
    {
        sta_timeout = CLIENT_TIMEOUT;
        // DEBUG_V ();
        response++;
    }

    if (ap_timeout < 15)
    {
        ap_timeout = AP_TIMEOUT;
        // DEBUG_V ();
        response++;
    }
    // DEBUG_END;

    return response;
}

// -----------------------------------------------------------------------------
void c_WiFiDriver::WiFiReset ()
{
    // DEBUG_V();
    ResetWiFi = true;
    // DEBUG_V();
}

/*****************************************************************************/
//  FSM Code
/*****************************************************************************/
/*****************************************************************************/
void fsm_WiFi_state_Boot::Poll ()
{
    // _ DEBUG_START;

    // Start trying to connect to the AP
    // _ DEBUG_V (String ("this: ") + String (uint32_t (this), HEX));
    fsm_WiFi_state_ConnectingUsingConfig_imp.Init ();

    // _ DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_Boot::Init ()
{
    // DEBUG_START;

    // DEBUG_V (String ("this: ") + String (uint32_t (this), HEX));
    pWiFiDriver->SetFsmState (this);
    pWiFiDriver->AnnounceState ();
    pWiFiDriver->ConnectionStatusMessage = F ("Disconnected");

    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_WiFi_state_ConnectingUsingConfig::Poll ()
{
    // _ DEBUG_START;

    // wait for the connection to complete via the callback function
    uint32_t CurrentTimeMS = millis ();

    if (WiFi.status () != WL_CONNECTED)
    {
        if (CurrentTimeMS - pWiFiDriver->GetFsmStartTime () > (1000 * pWiFiDriver->Get_sta_timeout ()))
        {
            // _ DEBUG_V (String ("this: ") + String (uint32_t (this), HEX));
            Log.errorln (String (F ("WiFi Failed to connect using Configured Credentials")).c_str ());
            fsm_WiFi_state_ConnectingUsingDefaults_imp.Init ();
        }
    }
    // _ DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectingUsingConfig::Init ()
{
    // DEBUG_START;
    // DEBUG_V (String ("       this: ") + String (uint32_t (this), HEX));
    // DEBUG_V (String ("pWiFiDriver: ") + String (uint32_t (pWiFiDriver), HEX));
    // DEBUG_V();
    String  CurrentSsid         = SSID.get ();
    String  CurrentPassphrase   = WpaKey.get ();

    if (CurrentSsid.isEmpty ())
    {
        // DEBUG_V ();
        fsm_WiFi_state_ConnectingUsingDefaults_imp.Init ();
    }
    else
    {
        pWiFiDriver->SetFsmState (this);
        pWiFiDriver->AnnounceState ();
        pWiFiDriver->SetFsmStartTime (millis ());

        pWiFiDriver->connectWifi (CurrentSsid, CurrentPassphrase);
    }
    // DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectingUsingConfig::OnConnect ()
{
    // DEBUG_START;

    fsm_WiFi_state_ConnectedToAP_imp.Init ();

    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_WiFi_state_ConnectingUsingDefaults::Poll ()
{
    // _ DEBUG_START;

    // wait for the connection to complete via the callback function
    uint32_t CurrentTimeMS = millis ();

    if (WiFi.status () != WL_CONNECTED)
    {
        if (CurrentTimeMS - pWiFiDriver->GetFsmStartTime () > (1000 * pWiFiDriver->Get_sta_timeout ()))
        {
            // _ DEBUG_V (String ("this: ") + String (uint32_t (this), HEX));
            Log.errorln (String (F ("WiFi Failed to connect using default Credentials")).c_str ());
            fsm_WiFi_state_ConnectingAsAP_imp.Init ();
        }
    }
    // _ DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectingUsingDefaults::Init ()
{
    // DEBUG_START;

    // DEBUG_V (String ("this: ") + String (uint32_t (this), HEX));
    pWiFiDriver->SetFsmState (this);
    pWiFiDriver->AnnounceState ();
    pWiFiDriver->SetFsmStartTime (millis ());

    pWiFiDriver->connectWifi (default_ssid, default_passphrase);

    // DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectingUsingDefaults::OnConnect ()
{
    // DEBUG_START;

    // DEBUG_V (String ("this: ") + String (uint32_t (this), HEX));
    fsm_WiFi_state_ConnectedToAP_imp.Init ();

    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_WiFi_state_ConnectingAsAP::Poll ()
{
    // _ DEBUG_START;

    if (0 != WiFi.softAPgetStationNum ())
    {
        fsm_WiFi_state_ConnectedToSta_imp.Init ();
    }
    else
    {
        if (millis () - pWiFiDriver->GetFsmStartTime () > (1000 * pWiFiDriver->Get_ap_timeout ()))
        {
            Log.errorln (String (F ("WiFi STA Failed to connect")).c_str ());
            fsm_WiFi_state_ConnectionFailed_imp.Init ();
        }
    }
    // _ DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectingAsAP::Init ()
{
    // DEBUG_START;

    pWiFiDriver->SetFsmState (this);
    pWiFiDriver->AnnounceState ();

    if (0 != ApFallback.get ())
    {
        WiFi.enableSTA (false);
        WiFi.enableAP (true);
        String FinalSsid = HotspotName.get ();

#ifdef ADD_CHIP_ID
            FinalSsid += int64String (ESP.getEfuseMac (), HEX);
#endif // ifdef ADD_CHIP_ID

        String PSK;
#ifdef REQUIRE_WIFI_AP_PSK
            Log.infoln (String (F ("-> HotSpot Requires PSK")).c_str ());
            PSK = F (AP_PSK_STR);   // Enable PSK.
#endif // ifdef REQUIRE_WIFI_AP_PSK

        WiFi.softAP (FinalSsid.c_str (), PSK.c_str ());

        pWiFiDriver->setIpAddress (WiFi.localIP ());
        pWiFiDriver->setIpSubNetMask (WiFi.subnetMask ());

        Log.infoln (String (String (F ("WiFi SOFTAP:       ssid: '")) + FinalSsid).c_str ());
        Log.infoln (String (String (F ("WiFi SOFTAP: IP Address: '")) + pWiFiDriver->getIpAddress ().toString ()).c_str ());
    }
    else
    {
        Log.verboseln (String (String (F ("WiFi SOFTAP: Not enabled"))).c_str ());
        fsm_WiFi_state_ConnectionFailed_imp.Init ();
    }
    // DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectingAsAP::OnConnect ()
{
    // DEBUG_START;

    fsm_WiFi_state_ConnectedToSta_imp.Init ();

    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_WiFi_state_ConnectedToAP::Poll ()
{
    // _ DEBUG_START;

    // did we get silently disconnected?
    if (WiFi.status () != WL_CONNECTED)
    {
        // DEBUG_V ("WiFi Handle Silent Disconnect");
        WiFi.reconnect ();
    }
    else
    {
        pWiFiDriver->dnsServer.processNextRequest ();

#ifdef OTA_ENB
            ArduinoOTA.handle ();   // OTA Service.
#endif // ifdef OTA_ENB

        pWiFiDriver->UpdateStatusFields ();
    }
    // _ DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectedToAP::Init ()
{
    // DEBUG_START;

    // DEBUG_V (String ("this: ") + String (uint32_t (this), HEX));
    pWiFiDriver->SetFsmState (this);
    pWiFiDriver->AnnounceState ();

    // WiFiDriver.SetUpIp ();

    pWiFiDriver->setIpAddress (WiFi.localIP ());
    pWiFiDriver->setIpSubNetMask (WiFi.subnetMask ());
    pWiFiDriver->ConnectionStatusMessage =
        (DHCP.get ()) ? F ("STA Mode (DHCP)") : F ("STA Mode (Static)");
    pWiFiDriver->UpdateStatusFields ();

    Log.infoln (String (String (F ("Connected with IP: ")) + pWiFiDriver->getIpAddress ().toString ()).c_str ());

    pWiFiDriver->SetIsWiFiConnected (true);

    extern void StartESPUI ();
    StartESPUI ();

#ifdef MDNS_ENB

        // The mDNS initialization is also handled by the ArduinoOTA.begin() function. This code block is a duplicate for MDNS only.
        if (!MDNS.begin (MdnsName.get ().c_str ()))
        {
            // ArduinoOTA.setHostname() MUST use the same name!
            Log.errorln (String (F ("-> Error starting mDNS; Service is disabled.")).c_str ());
        }
        else
        {
            Log.infoln (String (F ("-> Server mDNS has started")).c_str ());
            Log.infoln (String (F ("-> Open http://%s.local in your browser")).c_str (), MdnsName.get ().c_str ());

            MDNS.addService ("http", "tcp", WEBSERVER_PORT);
            MDNS.addServiceTxt ("http", "tcp", "arduino", MdnsName.get ().c_str ());
        }
#endif // ifdef MDNS_ENB

#ifdef OTA_ENB
        otaInit (MdnsName.getStr ());   // Init OTA services.
#endif // ifdef OTA_ENB

    // DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectedToAP::OnDisconnect ()
{
    // DEBUG_START;

    Log.verboseln (String (F ("WiFi Lost the connection to the AP")).c_str ());
    fsm_WiFi_state_ConnectionFailed_imp.Init ();

    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_WiFi_state_ConnectedToSta::Poll ()
{
    // _ DEBUG_START;

    // did we get silently disconnected?
    if (0 == WiFi.softAPgetStationNum ())
    {
        Log.verboseln (String (F ("WiFi Lost the connection to the STA")).c_str ());
        pWiFiDriver->dnsServer.stop ();
        Log.warningln ("-> DNS Server Terminated.");
        fsm_WiFi_state_ConnectionFailed_imp.Init ();
    }
    else
    {
        pWiFiDriver->UpdateStatusFields ();
    }
    // _ DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectedToSta::Init ()
{
    // DEBUG_START;

    pWiFiDriver->SetFsmState (this);
    pWiFiDriver->AnnounceState ();

    // WiFiDriver.SetUpIp ();

    pWiFiDriver->setIpAddress (WiFi.softAPIP ());
    pWiFiDriver->setIpSubNetMask (IPAddress (255, 255, 255, 0));
    pWiFiDriver->ConnectionStatusMessage = F ("AP Mode");
    pWiFiDriver->UpdateStatusFields ();

    Log.infoln (String (String (F ("Connected to a STA, Local AP IP: ")) + pWiFiDriver->getIpAddress ().toString ()).c_str ());

    pWiFiDriver->SetIsWiFiConnected (true);

    if (!pWiFiDriver->dnsServer.start (uint16_t (DNS_PORT), String ("*"), ApIpAddress.GetIpAddress ()))
    {
        Log.errorln (String (F ("WIFI: AP mode DNS Failed to start. No Web Sockets available.")).c_str ());
    }
    // DEBUG_END;
}

/*****************************************************************************/
void fsm_WiFi_state_ConnectedToSta::OnDisconnect ()
{
    // DEBUG_START;

    Log.verboseln (String (F ("WiFi STA Disconnected")).c_str ());
    pWiFiDriver->dnsServer.stop ();
    Log.warningln ("-> DNS Server Terminated.");
    fsm_WiFi_state_ConnectionFailed_imp.Init ();

    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_WiFi_state_ConnectionFailed::Init ()
{
    // DEBUG_START;

    pWiFiDriver->SetFsmState (this);
    pWiFiDriver->AnnounceState ();
    pWiFiDriver->ConnectionStatusMessage = F ("Disconnected");

    if (pWiFiDriver->IsWiFiConnected ())
    {
        pWiFiDriver->SetIsWiFiConnected (false);
        // NetworkMgr.SetWiFiIsConnected (false);
    }
    else
    {
        if (0 != ApReboot.get ())
        {
            // extern bool reboot;
            Log.infoln (String (F ("WiFi Requesting Reboot")).c_str ());

            // reboot = true;
        }
        else
        {
            // DEBUG_V ("WiFi Reboot Disabled.");

            // start over
            fsm_WiFi_state_Boot_imp.Init ();
        }
    }
    // DEBUG_END;
}

/*****************************************************************************/
/*****************************************************************************/
void fsm_WiFi_state_Disabled::Init ()
{
    // DEBUG_START;

    pWiFiDriver->SetFsmState (this);
    pWiFiDriver->AnnounceState ();

    if (pWiFiDriver->IsWiFiConnected ())
    {
        pWiFiDriver->SetIsWiFiConnected (false);
    }
    // DEBUG_END;
}

// -----------------------------------------------------------------------------
c_WiFiDriver WiFiDriver;
