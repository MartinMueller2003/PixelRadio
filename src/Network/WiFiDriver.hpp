#pragma once
/*
    WiFiDriver.hpp
    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
    Version: 1.1.0
    Creation: Dec-16-2021
    Revised:  Jun-13-2022
    Revision History: See PixelRadio.cpp
    Project Leader: T. Black (thomastech)
    Contributors: thomastech, MartinMueller2003

    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
*/

#include "PixelRadio.h"
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include "WiFiDriverUi.hpp"

class fsm_WiFi_state;

class c_WiFiDriver : public c_WiFiDriverUi
{
public:
                c_WiFiDriver    ();
    virtual     ~c_WiFiDriver   ();

    void        Begin           ();
    void        GetStatus       (JsonObject & json);
    bool        restoreConfiguration (JsonObject & json);
    void        saveConfiguration (JsonObject & json);

    IPAddress   getIpAddress    () { return CurrentIpAddress; }
    void        setIpAddress    (IPAddress NewAddress ) { CurrentIpAddress = NewAddress; }
    IPAddress   getIpSubNetMask () { return CurrentNetmask; }
    void        setIpSubNetMask (IPAddress NewAddress) { CurrentNetmask = NewAddress; }
    void        connectWifi     (const String & ssid, const String & passphrase);
    void        reset           ();
    void        Poll            ();

    void        SetFsmState     (fsm_WiFi_state * NewState);
    void        AnnounceState   ();
    void        SetFsmStartTime (uint32_t NewStartTime)    { FsmTimerWiFiStartTime = NewStartTime; }
    uint32_t    GetFsmStartTime (void)                     { return FsmTimerWiFiStartTime; }
    bool        IsWiFiConnected () { return ReportedIsWiFiConnected; }
    void        SetIsWiFiConnected (bool value) { ReportedIsWiFiConnected = value; }
    void        GetDriverName   (String & Name) { Name = F("WiFiDrv"); }
    uint32_t    Get_sta_timeout () { return sta_timeout; }
    uint32_t    Get_ap_timeout  () { return ap_timeout; }
    bool        Get_ap_fallbackIsEnabled () { return ap_fallbackIsEnabled; }
    bool        Get_RebootOnWiFiFailureToConnect () { return RebootOnWiFiFailureToConnect; }
    String      GetConfig_ssid  () { return ssid; }
    String      GetConfig_passphrase () { return passphrase; }
    void        GetHostname     (String & name);
    void        SetHostname     (String & name);
    void        Disable         ();
    void        Enable          ();
    bool        UsingDhcp       () { return (UseDhcp && !ValidateStaticSettings()); }

private:
    uint32_t    NextPollTime            = 0;
    uint32_t    PollInterval            = 1000;

    bool    ReportedIsWiFiConnected = false;

    int     ValidateConfig  ();
    void    SetUpIp         ();

    void    onWiFiConnect   (const WiFiEvent_t event, const WiFiEventInfo_t info);
    void    onWiFiDisconnect (const WiFiEvent_t event, const WiFiEventInfo_t info);

    void    onWiFiStaConn   (const WiFiEvent_t event, const WiFiEventInfo_t info);
    void    onWiFiStaDisc   (const WiFiEvent_t event, const WiFiEventInfo_t info);

protected:
    friend class fsm_WiFi_state_Boot;
    friend class fsm_WiFi_state_ConnectingUsingConfig;
    friend class fsm_WiFi_state_ConnectingUsingDefaults;
    friend class fsm_WiFi_state_ConnectedToAP;
    friend class fsm_WiFi_state_ConnectingAsAP;
    friend class fsm_WiFi_state_ConnectedToSta;
    friend class fsm_WiFi_state_ConnectionFailed;
    friend class fsm_WiFi_state_Disabled;
    friend class fsm_WiFi_state;
    fsm_WiFi_state * pCurrentFsmState = nullptr;
    uint32_t         FsmTimerWiFiStartTime = 0;
    DNSServer   dnsServer;

}; // c_WiFiDriver

/*****************************************************************************/
/*
*	Generic fsm base class.
*/
/*****************************************************************************/
// forward declaration
/*****************************************************************************/
class fsm_WiFi_state
{
protected:
    c_WiFiDriver * pWiFiDriver = nullptr;
public:
    fsm_WiFi_state() {}
    virtual ~fsm_WiFi_state() {}

    virtual void Poll (void) = 0;
    virtual void Init (void) = 0;
    virtual void GetStateName (String & sName) = 0;
    virtual void OnConnect (void) = 0;
    virtual void OnDisconnect (void) = 0;
            void GetDriverName (String & Name) { Name = F("WiFiDrv"); }
            void SetParent (c_WiFiDriver * parent) { pWiFiDriver = parent; }
}; // fsm_WiFi_state

/*****************************************************************************/
// Wait for polling to start.
class fsm_WiFi_state_Boot : public fsm_WiFi_state
{
public:
    fsm_WiFi_state_Boot() {}
    virtual ~fsm_WiFi_state_Boot() {}

    virtual void Poll (void);
    virtual void Init (void);
    virtual void GetStateName (String & sName) { sName = F ("Boot"); }
    virtual void OnConnect (void)             { /* ignore */ }
    virtual void OnDisconnect (void)          { /* ignore */ }

}; // fsm_WiFi_state_Boot

/*****************************************************************************/
class fsm_WiFi_state_ConnectingUsingConfig : public fsm_WiFi_state
{
public:
    fsm_WiFi_state_ConnectingUsingConfig() {}
    virtual ~fsm_WiFi_state_ConnectingUsingConfig() {}

    virtual void Poll (void);
    virtual void Init (void);
    virtual void GetStateName (String & sName) { sName = F ("Connecting Using Config Credentials"); }
    virtual void OnConnect (void);
    virtual void OnDisconnect (void) {}

}; // fsm_WiFi_state_ConnectingUsingConfig

/*****************************************************************************/
class fsm_WiFi_state_ConnectingUsingDefaults : public fsm_WiFi_state
{
public:
    fsm_WiFi_state_ConnectingUsingDefaults() {}
    virtual ~fsm_WiFi_state_ConnectingUsingDefaults() {}

    virtual void Poll (void);
    virtual void Init (void);
    virtual void GetStateName (String & sName) { sName = F ("Connecting Using Default Credentials"); }
    virtual void OnConnect (void);
    virtual void OnDisconnect (void) {}

}; // fsm_WiFi_state_ConnectingUsingConfig

/*****************************************************************************/
class fsm_WiFi_state_ConnectedToAP : public fsm_WiFi_state
{
public:
    fsm_WiFi_state_ConnectedToAP() {}
    virtual ~fsm_WiFi_state_ConnectedToAP() {}

    virtual void Poll (void);
    virtual void Init (void);
    virtual void GetStateName (String & sName) { sName = F ("Connected To AP"); }
    virtual void OnConnect (void) {}
    virtual void OnDisconnect (void);

}; // fsm_WiFi_state_ConnectedToAP

/*****************************************************************************/
class fsm_WiFi_state_ConnectingAsAP : public fsm_WiFi_state
{
public:
    fsm_WiFi_state_ConnectingAsAP() {}
    virtual ~fsm_WiFi_state_ConnectingAsAP() {}

    virtual void Poll (void);
    virtual void Init (void);
    virtual void GetStateName (String & sName) { sName = F ("Connecting As AP"); }
    virtual void OnConnect (void);
    virtual void OnDisconnect (void) {}

}; // fsm_WiFi_state_ConnectingAsAP

/*****************************************************************************/
class fsm_WiFi_state_ConnectedToSta : public fsm_WiFi_state
{
public:
    fsm_WiFi_state_ConnectedToSta() {}
    virtual ~fsm_WiFi_state_ConnectedToSta() {}

    virtual void Poll (void);
    virtual void Init (void);
    virtual void GetStateName (String & sName) { sName = F ("Connected To STA"); }
    virtual void OnConnect (void) {}
    virtual void OnDisconnect (void);

}; // fsm_WiFi_state_ConnectedToSta

/*****************************************************************************/
class fsm_WiFi_state_ConnectionFailed : public fsm_WiFi_state
{
public:
    fsm_WiFi_state_ConnectionFailed() {}
    virtual ~fsm_WiFi_state_ConnectionFailed() {}

    virtual void Poll(void) {}
    virtual void Init (void);
    virtual void GetStateName (String & sName) { sName = F ("Connection Failed"); }
    virtual void OnConnect (void) {}
    virtual void OnDisconnect (void) {}

}; // fsm_WiFi_state_Rebooting

/*****************************************************************************/
class fsm_WiFi_state_Disabled : public fsm_WiFi_state
{
public:
    fsm_WiFi_state_Disabled() {}
    virtual ~fsm_WiFi_state_Disabled() {}

    virtual void Poll (void) {}
    virtual void Init (void);
    virtual void GetStateName (String & sName) { sName = F ("Disabled"); }
    virtual void OnConnect (void) {}
    virtual void OnDisconnect (void) {}

}; // fsm_WiFi_state_Disabled

extern c_WiFiDriver WiFiDriver;