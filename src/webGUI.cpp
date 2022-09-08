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
#include "Gpio19.hpp"
#include "Gpio23.hpp"
#include "Gpio33.hpp"
#include "Diagnostics.hpp"

// ************************************************************************************************
// Local Strings.
String tempStr;

uint16_t    aboutTab    = Control::noParent;
uint16_t    adjTab      = Control::noParent;
uint16_t    backupTab   = Control::noParent;
uint16_t    ctrlTab     = Control::noParent;
uint16_t    diagTab     = Control::noParent;
uint16_t    gpioTab     = Control::noParent;
uint16_t    homeTab     = Control::noParent;
uint16_t    radioTab    = Control::noParent;
uint16_t    rdsTab      = Control::noParent;
uint16_t    wifiTab     = Control::noParent;

uint16_t    aboutLogoID     = Control::noParent;
uint16_t    aboutVersionID  = Control::noParent;

uint16_t    adjUvolID       = Control::noParent;

uint16_t    backupRestoreID     = Control::noParent;
uint16_t    backupRestoreMsgID  = Control::noParent;
uint16_t    backupSaveID        = Control::noParent;
uint16_t    backupSaveMsgID     = Control::noParent;
uint16_t    backupSaveSetID     = Control::noParent;
uint16_t    backupSaveSetMsgID  = Control::noParent;

uint16_t    diagBootID      = Control::noParent;
uint16_t    diagBootMsgID   = Control::noParent;
// uint16_t    diagLogMsgID    = Control::noParent;

// ************************************************************************************************
// applyCustomCss(): Apply custom CSS to Web GUI controls at the start of runtime.
//                   It is called AFTER ESPUI.begin(), see bottom of startGUI().
//                   Note: width and max-width appear to do the same thing. But try both. Avoid widths <30% or expect text wrap.
void initCustomCss (void)
{
    // DEBUG_START;
    // START OF PANEL INLINE STYLES
    ESPUI.   setPanelStyle ( aboutLogoID,    "background-color: white; color: black;");

    ESPUI.   setPanelStyle ( diagBootID,     "color: black;");

#ifdef OldWay
        ESPUI.setPanelStyle ( homeOnAirID,    "font-size: 3.0em;");
#endif // def OldWay


    // ESPUI.setPanelStyle(rdsDspTmID,     "font-size: 1.15em;");
    //  https://github.com/s00500/ESPUI/pull/147#issuecomment-1009821269.

    // END OF PANEL INLINE STYLES.

    // START OF ELEMENT INLINE STYLES

    ESPUI.   setElementStyle (   aboutVersionID,     "background-color: white; color: black; margin-top: 0px;");

    ESPUI.   setElementStyle (   backupRestoreMsgID, CSS_LABEL_STYLE_WHITE);
    ESPUI.   setElementStyle (   backupSaveMsgID,    CSS_LABEL_STYLE_WHITE);
    ESPUI.   setElementStyle (   backupSaveSetMsgID, CSS_LABEL_STYLE_RED);

    ESPUI.   setElementStyle (   diagBootMsgID,      CSS_LABEL_STYLE_WHITE);

#ifdef OldWay
        ESPUI.setElementStyle (   homeOnAirID,        "max-width: 80%;");
#endif // def OldWay

    // ESPUI.setElementStyle(homeLogoID,       "max-width: 45%; background-color: white; color: black;"); // DOES NOT WORK.

    // DEBUG_END;
    // END OF STYLES
}

// ************************************************************************************************
// displaySaveWarning(): Show the "Save Required" Message on all configuration pages.
void displaySaveWarning (void)
{
    ConfigSave.SetSaveNeeded ();
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
    ESPUI.setVerbosity (Verbosity::Quiet);  // Production mode.
    ESPUI.jsonInitialDocumentSize   = 5000;
    ESPUI.jsonUpdateDocumentSize    = 5000;
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

    tempStr.reserve (125);  // Avoid memory re-allocation fragments on the Global String.
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
    ConfigSave.AddControls (ctrlTab, ControlColor::Turquoise);

    //
    // *****************
    // GPIO Tab

    ESPUI.addControl (ControlType::Separator, GPIO_SETTINGS_STR, emptyString, ControlColor::None, gpioTab);
    Gpio19.AddControls(gpioTab,  ControlColor::Dark);
    Gpio23.AddControls(gpioTab,  ControlColor::Dark);
    Gpio33.AddControls(gpioTab,  ControlColor::Dark);
    ConfigSave.AddControls (gpioTab, ControlColor::Dark);

    //
    // *****************
    // Backup Tab
    ConfigSave.AddControls (backupTab, ControlColor::Wetasphalt);

    ESPUI.addControl (ControlType::Separator, SAVE_BACKUP_STR, emptyString, ControlColor::None, backupTab);
    backupSaveID =
        ESPUI.addControl (ControlType::Button,
                          BACKUP_SAV_CFG_STR,
                          BACKUP_SAVE_STR,
                          ControlColor::Wetasphalt,
                          backupTab,
                          & backupCallback);
    backupSaveMsgID = ESPUI.addControl (ControlType::Label, "SAVE_MSG", emptyString, ControlColor::Wetasphalt, backupSaveID);

    backupRestoreID =
        ESPUI.addControl (ControlType::Button,
                          BACKUP_RES_CFG_STR,
                          BACKUP_RESTORE_STR,
                          ControlColor::Wetasphalt,
                          backupTab,
                          & backupCallback);
    backupRestoreMsgID = ESPUI.addControl (ControlType::Label, "RESTORE_MSG", emptyString, ControlColor::Wetasphalt, backupRestoreID);
    // DEBUG_V();

    //
    // ******************
    // Diagnostics Tab
    Diagnostics.AddControls(diagTab, ControlColor::Sunflower);

#ifdef OldWay
        tempStr         = ControllerMgr.GetControllerEnabledFlag (SerialControllerId) ? "" : DIAG_LOG_MSG_STR;
        diagLogMsgID    = ESPUI.addControl (ControlType::Label, "LOG_MSG", tempStr, ControlColor::Sunflower, diagLogID);
#endif // def OldWay

    // DEBUG_V();
    diagBootID =
        ESPUI.addControl (ControlType::Button,
                          DIAG_REBOOT_STR,
                          DIAG_LONG_PRESS_STR,
                          ControlColor::Sunflower,
                          diagTab,
                          & diagBootCallback);
    diagBootMsgID = ESPUI.addControl (ControlType::Label, "REBOOT_MSG", emptyString, ControlColor::Sunflower, diagBootID);

    //
    // ******************
    // About Tab

    tempStr = N_Version;
    tempStr += VERSION_STR;
    tempStr += N_br;
    tempStr += BLD_DATE_STR;
    tempStr += N_br;
    tempStr += AUTHOR_STR;
    tempStr += N_br;
    tempStr += GITHUB_REPO_STR;
    tempStr += N_br;

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
