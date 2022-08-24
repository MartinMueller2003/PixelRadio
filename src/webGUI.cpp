/*
   File: webGUI.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
   license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   IMPORTANT:
   =========
   PixelRadio was originally developed to use LITTLEFS to serve the web pages. But
   there's an unresolved ESPUI library bug that causes random ESP32 crashes during
   browser page refreshes or page re-vists. Using EPUI's Embedded mode avoids this
   problem. So for now, do NOT switch to the LITTLEFS option to serve the web pages.

   ESPUI NOTES:
   ============
   NOTE 1. This project uses iangray001's patched release of ESPUI Library created
   on Feb-16-2022.
   I've found that the PR releases often introduce undesireable bugs. So I suggest
   staying with this release until there is a strong reason to try a newer version.

   NOTE 2.
   Custom Updates to the ESPUI Library:
   Any edits to the ESPUI library must be done in the Project's library folder. After
   editing, the ESPUI minified and .h files must be prepared (rebuilt) using the
   following command in platformio terminal: python .scripts/prepare_static_ui_sources.py -a
     Comment 1: It may be necessary to install the support programs (watch the error
      messages). Typically you may need to install the following (use Platformio Terminal):
      pip install csscompressor
      pip install jsmin
      pip install htmlmin
     Comment 2: The python file has been modified (src = "\PixelRadio\lib\ESPUI\data" and
     target = "\PixelRadio\lib\ESPUI\src").
     The prepared data files must be copied to this project's data folder. See below for
     more details.

   NOTE 3.
   Data Files: (Useful advice when using ESPUI's LITTLEFS)
   If LITTLEFS is used then ALL .html, .js, and .css data files must be in
   the PixelRadio/data folder.
   Here's some advice on how to get all the ESPUI data files to fit in a min_spiffs build:
   ESPUI has 50KB of unneeded files. Delete the following files (if present) from
   the /project data folder:
    data/index.min.htm
    data/css/normalize.min.css
    data/css/style.min.css
    data/js/controls.min.js
    data/js/graph.min.js
    data/js/slider.min.js
    data/js/tabbedcontent.min.js
    NOTE: Do NOT delete /data/js/zepto.min.js

    After the deletions the following files MUST remain in your data folder:
    /index.htm"
    /css/style.css
    /css/normalize.css
    /js/zepto.min.js
    /js/controls.js
    /js/slider.js
    /js/graph.js
    /js/tabbedcontent.js
    RadioLogo225x75_base64.gif

    NOTE 4.
    The browser's ESPUI interface can be redrawn by using ESPUI.jsonReload().
    However, it only redraws the currently displayed settings and ignores any programmable
    changes to system variables.

    NOTE 5.
    When new versions of ESPUI are installed please edit the dataIndexHTML.h and
    change <title>Control<title> section to <title>PixelRadio</title>
    This will report the device as "PixelRadio" on network scans.
    Note: If login is enabled then the device will report "Espressif, Inc."
 */

// ************************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <ESPUI.h>

#include "config.h"
#include "ConfigSave.hpp"
#include "ControllerMgr.h"
#include "globals.h"
#include "language.h"
#include "memdebug.h"
#include "PixelRadio.h"
#include "radio.hpp"
#include "LoginUser.hpp"
#include "LoginPassword.hpp"

// ************************************************************************************************
// Local Strings.
String tempStr;

uint16_t        aboutTab        = Control::noParent;
uint16_t        adjTab          = Control::noParent;
uint16_t        backupTab       = Control::noParent;
uint16_t        ctrlTab         = Control::noParent;
uint16_t        diagTab         = Control::noParent;
uint16_t        gpioTab         = Control::noParent;
uint16_t        homeTab         = Control::noParent;
uint16_t        radioTab        = Control::noParent;
uint16_t        rdsTab          = Control::noParent;
uint16_t        wifiTab         = Control::noParent;

uint16_t        aboutLogoID     = Control::noParent;
uint16_t        aboutVersionID  = Control::noParent;

uint16_t        adjSaveID       = Control::noParent;
uint16_t        adjSaveMsgID    = Control::noParent;
uint16_t        adjUvolID       = Control::noParent;

uint16_t        backupRestoreID         = Control::noParent;
uint16_t        backupRestoreMsgID      = Control::noParent;
uint16_t        backupSaveID            = Control::noParent;
uint16_t        backupSaveMsgID         = Control::noParent;
uint16_t        backupSaveSetID         = Control::noParent;
uint16_t        backupSaveSetMsgID      = Control::noParent;

uint16_t        ctrlSaveID      = Control::noParent;
uint16_t        ctrlSaveMsgID   = Control::noParent;
uint16_t        wifiSaveID      = Control::noParent;
uint16_t        wifiSaveMsgID   = Control::noParent;

uint16_t        diagBootID      = Control::noParent;
uint16_t        diagBootMsgID   = Control::noParent;
uint16_t        diagLogID       = Control::noParent;
uint16_t        diagLogMsgID    = Control::noParent;
uint16_t        diagMemoryID    = Control::noParent;
uint16_t        diagTimerID     = Control::noParent;
uint16_t        diagVbatID      = Control::noParent;
uint16_t        diagVdcID       = Control::noParent;

uint16_t        radioSaveID     = Control::noParent;
uint16_t        radioSaveMsgID  = Control::noParent;

uint16_t        gpio19ID        = Control::noParent;
uint16_t        gpio23ID        = Control::noParent;
uint16_t        gpio33ID        = Control::noParent;
uint16_t        gpio19MsgID     = Control::noParent;
uint16_t        gpio23MsgID     = Control::noParent;
uint16_t        gpio33MsgID     = Control::noParent;
uint16_t        gpioSaveID      = Control::noParent;
uint16_t        gpioSaveMsgID   = Control::noParent;

uint16_t        rdsSaveID       = Control::noParent;
uint16_t        rdsSaveMsgID    = Control::noParent;
uint16_t        rdsSnameID      = Control::noParent;

// ************************************************************************************************
// applyCustomCss(): Apply custom CSS to Web GUI controls at the start of runtime.
//                   It is called AFTER ESPUI.begin(), see bottom of startGUI().
//                   Note: width and max-width appear to do the same thing. But try both. Avoid widths <30% or expect text wrap.
void initCustomCss (void)
{
    // DEBUG_START;
    // START OF PANEL INLINE STYLES
    ESPUI.      setPanelStyle ( aboutLogoID,    "background-color: white; color: black;");

#ifdef OldWay
        ESPUI.  setPanelStyle ( ctrlMqttPortID, "font-size: 1.25em;");
#endif // def OldWay

    ESPUI.      setPanelStyle ( diagBootID,     "color: black;");
    ESPUI.      setPanelStyle ( diagLogID,      "color: black;");
    ESPUI.      setPanelStyle ( diagMemoryID,   "color: black; font-size: 1.25em;");
    ESPUI.      setPanelStyle ( diagTimerID,    "color: black; font-size: 1.25em;");
    ESPUI.      setPanelStyle ( diagVbatID,     "color: black; font-size: 1.25em;");
    ESPUI.      setPanelStyle ( diagVdcID,      "color: black; font-size: 1.25em;");

#ifdef OldWay
        ESPUI.  setPanelStyle ( homeOnAirID,    "font-size: 3.0em;");
#endif // def OldWay


    // ESPUI.setPanelStyle(rdsDspTmID,     "font-size: 1.15em;");
    //  https://github.com/s00500/ESPUI/pull/147#issuecomment-1009821269.

    // END OF PANEL INLINE STYLES.

    // START OF ELEMENT INLINE STYLES

    ESPUI.      setElementStyle (       aboutVersionID,         "background-color: white; color: black; margin-top: 0px;");

    ESPUI.      setElementStyle (       adjSaveMsgID,           CSS_LABEL_STYLE_RED);

    ESPUI.      setElementStyle (       backupRestoreMsgID,     CSS_LABEL_STYLE_WHITE);
    ESPUI.      setElementStyle (       backupSaveMsgID,        CSS_LABEL_STYLE_WHITE);
    ESPUI.      setElementStyle (       backupSaveSetMsgID,     CSS_LABEL_STYLE_RED);

    ESPUI.      setElementStyle (       ctrlSaveMsgID,          CSS_LABEL_STYLE_RED);

    ESPUI.      setElementStyle (       diagBootMsgID,          CSS_LABEL_STYLE_BLACK);
    ESPUI.      setElementStyle (       diagMemoryID,           "max-width: 40%;");
    ESPUI.      setElementStyle (       diagLogMsgID,           CSS_LABEL_STYLE_BLACK);
    ESPUI.      setElementStyle (       diagTimerID,            "max-width: 50%;");
    ESPUI.      setElementStyle (       diagVbatID,             "max-width: 30%;");
    ESPUI.      setElementStyle (       diagVdcID,              "max-width: 30%;");

    ESPUI.      setElementStyle (       gpio19MsgID,            CSS_LABEL_STYLE_WHITE);
    ESPUI.      setElementStyle (       gpio23MsgID,            CSS_LABEL_STYLE_WHITE);
    ESPUI.      setElementStyle (       gpio33MsgID,            CSS_LABEL_STYLE_WHITE);
    ESPUI.      setElementStyle (       gpioSaveMsgID,          CSS_LABEL_STYLE_RED);

#ifdef OldWay
        ESPUI.  setElementStyle (       homeOnAirID,            "max-width: 80%;");
#endif // def OldWay

    // ESPUI.setElementStyle(homeLogoID,       "max-width: 45%; background-color: white; color: black;"); // DOES NOT WORK.

    ESPUI.      setElementStyle (       radioSaveMsgID, CSS_LABEL_STYLE_RED);
    ESPUI.      setElementStyle (       rdsSaveMsgID,   CSS_LABEL_STYLE_BLACK);

    ESPUI.      setElementStyle (       wifiSaveMsgID,  CSS_LABEL_STYLE_MAROON);

    // DEBUG_END;
    // END OF STYLES
}

// ************************************************************************************************
// displaySaveWarning(): Show the "Save Required" Message on all configuration pages.
void displaySaveWarning (void)
{
    ConfigSave.SetSaveNeeded ();
#ifdef OldWay
        ESPUI.  print ( adjSaveMsgID,           SAVE_SETTINGS_MSG_STR);
        ESPUI.  print ( backupSaveSetMsgID,     SAVE_SETTINGS_MSG_STR);
        ESPUI.  print ( ctrlSaveMsgID,          SAVE_SETTINGS_MSG_STR);
        ESPUI.  print ( gpioSaveMsgID,          SAVE_SETTINGS_MSG_STR);
        ESPUI.  print ( radioSaveMsgID,         SAVE_SETTINGS_MSG_STR);
        ESPUI.  print ( rdsSaveMsgID,           SAVE_SETTINGS_MSG_STR);
        ESPUI.  print ( wifiSaveMsgID,          SAVE_SETTINGS_MSG_STR);
#endif // def OldWay
}

// ************************************************************************************************
// startGUI(): Must be called once in startup, AFTER wifiConnect()
void startGUI (void)
{
    // DEBUG_START;

    buildGUI ();
    // DEBUG_V();

    // These have been moved to Heap and no longer impact stack
    // ESPUI.setVerbosity(Verbosity::VerboseJSON);                        // Debug mode.
    ESPUI.setVerbosity (Verbosity::Quiet);      // Production mode.
    ESPUI.jsonInitialDocumentSize       = 5000;
    ESPUI.jsonUpdateDocumentSize        = 5000;
    // DEBUG_V();

    // DEBUG_V();
    initCustomCss ();
    // DEBUG_END;
}

// ************************************************************************************************
void StartESPUI ()
{
    // DEBUG_START;
    if ((LoginUser.get ().isEmpty ()) || (LoginPassword.get ().isEmpty ()))
    {   // Missing credentials, use automatic login.
        // DEBUG_V();
        ESPUI.begin ("PixelRadio");
        // DEBUG_V();

        // Don't use LITTLEFS, browser refreshes will crash.
        // ESPUI.beginLITTLEFS(APP_NAME_STR);
    }
    else
    {
        // DEBUG_V();
        ESPUI.begin ("PixelRadio", LoginUser.get ().c_str (), LoginPassword.get ().c_str ());
        // DEBUG_V();

        // Don't use LITLEFS, browser refreshes will crash.
        // ESPUI.beginLITTLEFS ( APP_NAME_STR, LoginUser.getStr().c_str (), LoginPassword.getStr().c_str ());
    }
    // DEBUG_END;
}

// ************************************************************************************************
// updateUiGpioMsg(): Update the GPIO Boot Control's Message Label Element.
bool updateUiGpioMsg (uint8_t pin, String & ControllerName, bool PinState)
{
    // DEBUG_START;
    uint16_t msgID;

    switch (pin)
    {
        case GPIO19_PIN:
        {
            msgID = gpio19MsgID;
            break;
        }

        case GPIO23_PIN:
        {
            msgID = gpio23MsgID;
            break;
        }

        case GPIO33_PIN:
        {
            msgID = gpio33MsgID;
            break;
        }
    }   // switch

    ESPUI.print (msgID, String (F ("{ SET TO ")) + ((PinState) ? GPIO_OUT_HI_STR : GPIO_OUT_LO_STR) + F (" BY ") + ControllerName + F (" }"));

    // DEBUG_END;
    return true;
}

// ************************************************************************************************
// updateUiFreeMemory(): Update the Free Heap Memory on the diagTab.
void updateUiFreeMemory (void)
{
    char logBuff[40];
    static uint32_t oldMillis = millis ();

    if (millis () > oldMillis + FREE_MEM_UPD_TIME)
    {
        oldMillis       = millis ();
        tempStr         = ESP.getFreeHeap ();
        tempStr         += " Bytes";
        ESPUI.print (diagMemoryID, tempStr);
        sprintf (logBuff, "Free Heap Memory: %s.", tempStr.c_str ());
        Log.verboseln (logBuff);
    }
}

// *********************************************************************************************
// updateUiDiagTimer(): Update Elapsed Time on diagTab Page. Show Days + HH:MM:SS.
void updateUiDiagTimer (void)
{
    char timeBuff[30];
    static uint8_t      seconds         = 0;
    static uint8_t      minutes         = 0;
    static uint8_t      hours           = 0;
    static int16_t      days            = 0;
    static uint32_t previousMillis      = millis ();
    uint32_t currentMillis              = millis ();

    if ((currentMillis - previousMillis) >= 1000)
    {
        previousMillis = millis () - ((currentMillis - previousMillis) - 1000);
        seconds++;

        if (seconds >= 60)
        {
            seconds = 0;
            minutes++;

            if (minutes >= 60)
            {
                minutes = 0;
                hours++;

                if (hours >= 24)
                {
                    hours = 0;
                    days++;
                }
            }
        }
        sprintf (timeBuff, "Days:%u + %02u:%02u:%02u", days, hours, minutes, seconds);
        ESPUI.print (diagTimerID, timeBuff);
    }
}

// ************************************************************************************************
void updateUiVolts (void)
{
    extern uint32_t     paVolts;
    static uint32_t     previousMillis = 0;
    char logBuff[60];

    if (previousMillis == 0)
    {
        previousMillis = millis ();     // Initialize First entry;
    }
    else if (millis () - previousMillis >= VOLTS_UPD_TIME)
    {
        previousMillis  = millis ();
        tempStr         = String (vbatVolts, 1);
        tempStr         += " VDC";
        ESPUI.print (diagVbatID, tempStr);
        sprintf (logBuff, "Health Check, System Voltage: %01.1f VDC.", vbatVolts);
        Log.verboseln (logBuff);

        tempStr = String (paVolts, 1);
        tempStr += " VDC";
        ESPUI.print (diagVdcID, tempStr);
        sprintf (logBuff, "Health Check, RF AMP Voltage: %01.1f VDC.", paVolts);
        Log.verboseln (logBuff);
    }
}

// ************************************************************************************************
// buildGUI(): Create the Web GUI. Must call this
//    Enable the following option if you want sliders to be continuous (update during move) and not discrete (update on
// stop):
//    ESPUI.sliderContinuous = true; // Beware, this will spam the webserver with a lot of messages!
//
// Example of image inside label control.
// ESPUI.addControl(ControlType::Label, "IMAGE", (makeWebGif("/RadioLogo100x45_base64.gif", 100, 45, "white")),ControlColor::Peterriver);
// Save this example!
//
void buildGUI (void)
{
    // DEBUG_START;

    extern uint32_t paVolts;

    tempStr.reserve (125);      // Avoid memory re-allocation fragments on the Global String.
    char charBuff[25];
    float tempFloat;

    // ************
    // Menu Tabs
    homeTab     = ESPUI.addControl (ControlType::Tab, "HOME", HOME_TAB_STR);
    adjTab      = ESPUI.addControl (ControlType::Tab, "ADJ", ADJUST_TAB_STR);
    radioTab    = ESPUI.addControl (ControlType::Tab, "RADIO", RADIO_TAB_STR);
    rdsTab      = ESPUI.addControl (ControlType::Tab, "RDS", RDS_TAB_STR);
    wifiTab     = ESPUI.addControl (ControlType::Tab, "WIFI", WIFI_TAB_STR);
    ctrlTab     = ESPUI.addControl (ControlType::Tab, "CNTRL", CTRL_TAB_STR);
    gpioTab     = ESPUI.addControl (ControlType::Tab, "GPIO", GPIO_TAB_STR);
    backupTab   = ESPUI.addControl (ControlType::Tab, "BACKUP", BACKUP_TAB_STR);
    diagTab     = ESPUI.addControl (ControlType::Tab, "DIAG", DIAG_TAB_STR);
    aboutTab    = ESPUI.addControl (ControlType::Tab, "ABOUT", N_About);

    // ************
    // Home Tab
    ESPUI.addControl (ControlType::Separator, HOME_FM_SEP_STR, emptyString, ControlColor::None, homeTab);
    Radio.AddHomeControls (homeTab, ControlColor::Peterriver);
    WiFiDriver.addHomeControls (homeTab, ControlColor::Peterriver);

    // **************
    // Adjust Tab
    Radio.AddAdjControls (adjTab, ControlColor::Wetasphalt);
    ConfigSave.AddControls (adjTab, ControlColor::Wetasphalt);

    // ************
    // Radio Tab
    Radio.AddRadioControls (radioTab, ControlColor::Emerald);
    ConfigSave.AddControls (radioTab, ControlColor::Emerald);

    // RF Power Control is not compatible with the RF Amp Circutry.
    // Low Power levels do not correctly excite the PA Transistor.
    // RfPower.AddControl(radioTab);

    //
    // *****************
    // Local RDS Tab
    ESPUI.addControl (ControlType::Separator, RDS_GENERAL_SET_STR, emptyString, ControlColor::None, rdsTab);
    Radio.AddRdsControls (rdsTab, ControlColor::Alizarin);
    ConfigSave.AddControls (rdsTab, ControlColor::Alizarin);

    //
    // *************
    //  WiFi Tab
    WiFiDriver.addControls (wifiTab, ControlColor::Carrot);
    ConfigSave.AddControls (wifiTab, ControlColor::Carrot);

    //
    // *************
    //  Controller Tab
    ControllerMgr.AddControls (ctrlTab, ControlColor::Turquoise);
    // ESPUI.addControl(ControlType::Separator, CTRL_USB_SERIAL_STR, emptyString, ControlColor::None, ctrlTab);
    ConfigSave.AddControls (ctrlTab, ControlColor::Turquoise);

    //
    // *****************
    // GPIO Tab

    ESPUI.      addControl (    ControlType::Separator, GPIO_SETTINGS_STR,      emptyString,            ControlColor::None,     gpioTab);
    gpio19ID =
        ESPUI.addControl (ControlType::Select, GPIO_19_STR, gpio19BootStr, ControlColor::Dark, gpioTab, &gpioCallback);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_FT_STR,        GPIO_INP_FT_STR,        ControlColor::None,     gpio19ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_PU_STR,        GPIO_INP_PU_STR,        ControlColor::None,     gpio19ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_PD_STR,        GPIO_INP_PD_STR,        ControlColor::None,     gpio19ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_OUT_LO_STR,        GPIO_OUT_LO_STR,        ControlColor::None,     gpio19ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_OUT_HI_STR,        GPIO_OUT_HI_STR,        ControlColor::None,     gpio19ID);
    gpio19MsgID = ESPUI.addControl (ControlType::Label, "GPIO_MSG", " ", ControlColor::None, gpio19ID);

    gpio23ID =
        ESPUI.addControl (ControlType::Select, GPIO_23_STR, gpio23BootStr, ControlColor::Dark, gpioTab, &gpioCallback);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_FT_STR,        GPIO_INP_FT_STR,        ControlColor::None,     gpio23ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_PU_STR,        GPIO_INP_PU_STR,        ControlColor::None,     gpio23ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_PD_STR,        GPIO_INP_PD_STR,        ControlColor::None,     gpio23ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_OUT_LO_STR,        GPIO_OUT_LO_STR,        ControlColor::None,     gpio23ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_OUT_HI_STR,        GPIO_OUT_HI_STR,        ControlColor::None,     gpio23ID);
    gpio23MsgID = ESPUI.addControl (ControlType::Label, "GPIO_MSG", " ", ControlColor::None, gpio23ID);

    gpio33ID =
        ESPUI.addControl (ControlType::Select, GPIO_33_STR, gpio33BootStr, ControlColor::Dark, gpioTab, &gpioCallback);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_FT_STR,        GPIO_INP_FT_STR,        ControlColor::None,     gpio33ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_PU_STR,        GPIO_INP_PU_STR,        ControlColor::None,     gpio33ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_INP_PD_STR,        GPIO_INP_PD_STR,        ControlColor::None,     gpio33ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_OUT_LO_STR,        GPIO_OUT_LO_STR,        ControlColor::None,     gpio33ID);
    ESPUI.      addControl (    ControlType::Option,    GPIO_OUT_HI_STR,        GPIO_OUT_HI_STR,        ControlColor::None,     gpio33ID);
    gpio33MsgID = ESPUI.addControl (ControlType::Label, "GPIO_MSG", " ", ControlColor::None, gpio33ID);

    ConfigSave. AddControls (gpioTab, ControlColor::Dark);

    //
    // *****************
    // Backup Tab
    ConfigSave. AddControls (backupTab, ControlColor::Wetasphalt);

    ESPUI.      addControl (ControlType::Separator, SAVE_BACKUP_STR, emptyString, ControlColor::None, backupTab);
    backupSaveID =
        ESPUI.addControl (ControlType::Button,
                          BACKUP_SAV_CFG_STR,
                          BACKUP_SAVE_STR,
                          ControlColor::Wetasphalt,
                          backupTab,
                          &backupCallback);
    backupSaveMsgID = ESPUI.addControl (ControlType::Label, "SAVE_MSG", emptyString, ControlColor::Wetasphalt, backupSaveID);

    backupRestoreID =
        ESPUI.addControl (ControlType::Button,
                          BACKUP_RES_CFG_STR,
                          BACKUP_RESTORE_STR,
                          ControlColor::Wetasphalt,
                          backupTab,
                          &backupCallback);
    backupRestoreMsgID = ESPUI.addControl (ControlType::Label, "RESTORE_MSG", emptyString, ControlColor::Wetasphalt, backupRestoreID);
    // DEBUG_V();

    //
    // ******************
    // Diagnostics Tab

    ESPUI.      addControl (ControlType::Separator, DIAG_HEALTH_SEP_STR, emptyString, ControlColor::None, diagTab);
    tempStr     = String (vbatVolts, 1);
    tempStr     += " VDC";
    diagVbatID  = ESPUI.addControl (ControlType::Label, DIAG_VBAT_STR, tempStr, ControlColor::Sunflower, diagTab);

    tempStr     = String (paVolts, 1);
    tempStr     += " VDC";
    diagVdcID   = ESPUI.addControl (ControlType::Label, DIAG_VDC_STR, tempStr, ControlColor::Sunflower, diagTab);

    ESPUI.      addControl (    ControlType::Separator, DIAG_DEBUG_SEP_STR, emptyString, ControlColor::None, diagTab);
    diagLogID =
        ESPUI.addControl (ControlType::Select, DIAG_LOG_LVL_STR, logLevelStr, ControlColor::Sunflower, diagTab, &diagLogCallback);
    ESPUI.      addControl (    ControlType::Option,
                                DIAG_LOG_SILENT_STR,
                                DIAG_LOG_SILENT_STR,
                                ControlColor::None,
                                diagLogID);
    ESPUI.      addControl (ControlType::Option,
                            DIAG_LOG_FATAL_STR,
                            DIAG_LOG_FATAL_STR,
                            ControlColor::None,
                            diagLogID);
    ESPUI.      addControl (ControlType::Option,
                            DIAG_LOG_ERROR_STR,
                            DIAG_LOG_ERROR_STR,
                            ControlColor::None,
                            diagLogID);
    ESPUI.      addControl (ControlType::Option,
                            DIAG_LOG_WARN_STR,
                            DIAG_LOG_WARN_STR,
                            ControlColor::None,
                            diagLogID);
    ESPUI.      addControl (ControlType::Option,
                            DIAG_LOG_INFO_STR,
                            DIAG_LOG_INFO_STR,
                            ControlColor::None,
                            diagLogID);
    ESPUI.      addControl (ControlType::Option,
                            DIAG_LOG_TRACE_STR,
                            DIAG_LOG_TRACE_STR,
                            ControlColor::None,
                            diagLogID);
    ESPUI.      addControl (ControlType::Option,
                            DIAG_LOG_VERB_STR,
                            DIAG_LOG_VERB_STR,
                            ControlColor::None,
                            diagLogID);
#ifdef OldWay
        tempStr         = ControllerMgr.GetControllerEnabledFlag (SerialControllerId) ? "" : DIAG_LOG_MSG_STR;
        diagLogMsgID    = ESPUI.addControl (ControlType::Label, "LOG_MSG", tempStr, ControlColor::Sunflower, diagLogID);
#endif // def OldWay

    ESPUI.      addControl (ControlType::Separator, DIAG_SYSTEM_SEP_STR, emptyString, ControlColor::None, diagTab);
    // DEBUG_V();

    tempStr             = ESP.getFreeHeap ();
    tempStr             += " Bytes";
    diagMemoryID        = ESPUI.addControl (ControlType::Label, DIAG_FREE_MEM_STR, tempStr, ControlColor::Sunflower, diagTab);

    diagTimerID = ESPUI.addControl (ControlType::Label, DIAG_RUN_TIME_STR, emptyString, ControlColor::Sunflower, diagTab);

    diagBootID =
        ESPUI.addControl (ControlType::Button,
                          DIAG_REBOOT_STR,
                          DIAG_LONG_PRESS_STR,
                          ControlColor::Sunflower,
                          diagTab,
                          &diagBootCallback);
    diagBootMsgID = ESPUI.addControl (ControlType::Label, "REBOOT_MSG", emptyString, ControlColor::Sunflower, diagBootID);

    //
    // ******************
    // About Tab

    tempStr     = N_Version;
    tempStr     += VERSION_STR;
    tempStr     += N_br;
    tempStr     += BLD_DATE_STR;
    tempStr     += N_br;
    tempStr     += AUTHOR_STR;
    tempStr     += N_br;
    tempStr     += GITHUB_REPO_STR;
    tempStr     += N_br;

    /*(makeWebGif("/RadioLogo225x75_base64.gif", 225, 75, "white")),*/
    static String WebGif = (makeWebGif ("/RadioLogo225x75_base64.gif", 200, 66, "white"));
    // DEBUG_V(String("WebGif.Length: ") + String(WebGif.length()));
    aboutLogoID = ESPUI.addControl (ControlType::Label,
                                    N_About,
                                    WebGif,
                                    ControlColor::None,
                                    aboutTab);

    aboutVersionID = ESPUI.addControl (ControlType::Label,
                                       N_About_PixelRadio,
                                       tempStr,
                                       ControlColor::None,
                                       aboutLogoID);

    // this gets set as a side effect of the control setup.
    ConfigSave.ClearSaveNeeded ();

    // DEBUG_END;
}
