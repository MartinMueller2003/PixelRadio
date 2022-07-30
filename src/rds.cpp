/*
   File: rds.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this
   license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.

   Note 1: RDS RadioText is sent in a 57KHz sub-carrier. Equivalent bit rate is ~475 baud.
 */

// ************************************************************************************************

#include <Arduino.h>
#include <ArduinoLog.h>
#include <string>
#include "PixelRadio.h"
#include "globals.h"
#include "QN8027Radio.h"
#include "Controllers/ControllerMgr.h"

// ************************************************************************************************
extern QN8027Radio radio;

#ifdef OldWay


// ************************************************************************************************
// processRDS(): Local RDS RadioText Display Handler for homeTab. 
//               Must be installed in main loop.
//               Display time = rdsMsgTime associated with each message.
//               A round robbin scheduler is used and user can select which messages to show.
void processRDS(void) 
{
    char logBuff[75 + RDS_TEXT_MAX_SZ];
    static uint8_t loop       = 0;
    uint32_t currentMillis    = 0;
    static uint32_t rdsMillis = 0;     // Timer for RDS Web GUI updates.
    static uint32_t cntMillis = 0;     // Timer for ICStation FM Tx services.
    static String   rdsRefreshPsnStr;
    static String   rdsRefreshTextStr; // RadioText Refresh, Message to repeat.

    rdsRefreshPsnStr.reserve(10);
    rdsRefreshTextStr.reserve(70);

    currentMillis = millis();

    if (cntMillis == 0) {
        updateUiRDSTmr(true);         // Clear Display
        cntMillis = currentMillis; // Initialize First entry;
    }
    else if (testModeFlg) {
        updateTestTones(false);
        cntMillis = currentMillis;
        rdsMillis = currentMillis - rdsMsgTime + 500;         // Schedule next "normal" RadioText in 0.5Sec.
        return;
    }
    else if (currentMillis - cntMillis >= RDS_MSG_UPD_TIME)
    { // One Second Updates.
        cntMillis = currentMillis;

        if (!rfCarrierFlg) {
            updateUiRDSTmr(true);                            // Clear Displayed Elapsed Timer.
            displayRdsText();
            rdsMillis = currentMillis - rdsMsgTime + 500; // Schedule next RadioText in 0.5Sec.
            return;
        }
        else
        {
            c_ControllerMgr::CurrentRdsMsgInfo_t CurrentRdsMsgInfo;
            if(ControllerMgr.GetNextRdsMsgToDisplay(CurrentRdsMsgInfo))
            {
                sprintf(logBuff, "Controller Sending RDS Program Service Name (%s)", CurrentRdsMsgInfo.ProgramServiceName.c_str());
                Log.infoln(logBuff);
                radio.sendStationName(rdsRefreshPsnStr);
                sprintf(logBuff, "Controller Sending RDS RadioText (%s).", CurrentRdsMsgInfo.MsgText.c_str());
                Log.infoln(logBuff);
                radio.sendRadioText(CurrentRdsMsgInfo.MsgText.c_str());
                updateUiRdsText(CurrentRdsMsgInfo.MsgText);
            }
        }
    }
        else if (!checkLocalControllerAvail() && !checkAnyRdsControllerAvailable()) {
        {
            updateUiRDSTmr(0);                            // Clear Displayed Elapsed Timer.
            displayRdsText();
            rdsMillis = currentMillis - rdsMsgTime + 500; // Schedule next RadioText in 0.5Sec.
            return;
        }
        else if (ControllerMgr.GetActiveTextFlag(SerialControllerId) && 
                 ControllerMgr.GetStopFlag(SerialControllerId))
        {
            ControllerMgr.SetStopFlag(SerialControllerId, false);
            ControllerMgr.SetActiveTextFlag(SerialControllerId, false);
            rdsMillis           = millis() - rdsMsgTime; // Force Countdown Timeout.
            Log.infoln("Serial Controller's RadioText has Been Stopped.");
        }
        else if (ControllerMgr.GetActiveTextFlag(MqttControllerId) &&
                 ControllerMgr.GetStopFlag(MqttControllerId))
        {
            ControllerMgr.SetStopFlag(MqttControllerId, false);
            ControllerMgr.SetActiveTextFlag(MqttControllerId, false);
            rdsMillis         = millis() - rdsMsgTime; // Force Countdown Timeout.
            Log.infoln("MQTT Controller's RadioText has Been Stopped.");
        }
        else if (ControllerMgr.GetActiveTextFlag(HttpControllerId) &&
                 ControllerMgr.GetStopFlag(HttpControllerId))
        {
            ControllerMgr.SetStopFlag(HttpControllerId, false);
            ControllerMgr.SetActiveTextFlag(HttpControllerId, false);
            rdsMillis         = millis() - rdsMsgTime; // Force Countdown Timeout.
            Log.infoln("MQTT Controller's RadioText has Been Stopped.");
        }
        else if (ControllerMgr.GetControllerEnabledFlag(SerialControllerId) &&
                 ControllerMgr.GetControllerEnabledFlag(SerialControllerId))
        { // Priority #1, New Serial RadioText.
            ControllerMgr.SetTextFlag(SerialControllerId, false);
            ControllerMgr.SetStopFlag(SerialControllerId, false);

            ControllerMgr.SetActiveTextFlag(SerialControllerId, true);
            ControllerMgr.SetActiveTextFlag(MqttControllerId,   false);
            ControllerMgr.SetActiveTextFlag(HttpControllerId,   false);
            ControllerMgr.SetActiveTextFlag(LocalControllerId,  false);

            rdsRefreshPsnStr  = ControllerMgr.GetRdsProgramServiceName(SerialControllerId);
            rdsRefreshTextStr = ControllerMgr.GetPayloadText(SerialControllerId);
            rdsMsgTime        = ControllerMgr.GetRdsMsgTime(SerialControllerId);
            rdsMillis         = currentMillis;

            sprintf(logBuff,
                    "Serial Controller RDS Will Use: PI=0x%04X, PTY=%u.",
                    ControllerMgr.GetPiCode(SerialControllerId),
                    ControllerMgr.GetPtyCode(SerialControllerId));
            Log.infoln(logBuff);
            radio.setPiCode(ControllerMgr.GetPiCode(SerialControllerId)); // Set Serial Controller's Pi Code.
            radio.setPtyCode(ControllerMgr.GetPtyCode(SerialControllerId)); // Set Serial Controller's PTY Code.

            sprintf(logBuff, "Serial Controller Sending RDS Program Service Name (%s)", rdsRefreshPsnStr.c_str());
            Log.infoln(logBuff);
            radio.sendStationName(rdsRefreshPsnStr);
            sprintf(logBuff, "Serial Controller Sending RDS RadioText (%s).", rdsRefreshTextStr.c_str());
            Log.infoln(logBuff);
            radio.sendRadioText(rdsRefreshTextStr);
            updateUiRdsText(rdsRefreshTextStr);
            ControllerMgr.Display(SerialControllerId);
        }
        else if (ControllerMgr.GetControllerEnabledFlag(MqttControllerId) &&
                 ControllerMgr.GetTextFlag(MqttControllerId) &&
                 !ControllerMgr.GetActiveTextFlag(SerialControllerId))
        { // Priority #2, New MQTT RadioText
            ControllerMgr.SetTextFlag(MqttControllerId, false);
            ControllerMgr.SetStopFlag(MqttControllerId, false);

            ControllerMgr.SetActiveTextFlag(MqttControllerId, true);
            ControllerMgr.SetActiveTextFlag(HttpControllerId, false);
            ControllerMgr.SetActiveTextFlag(LocalControllerId, false);

            rdsRefreshPsnStr  = ControllerMgr.GetRdsProgramServiceName(MqttControllerId);
            rdsRefreshTextStr = ControllerMgr.GetPayloadText(MqttControllerId);
            rdsMsgTime        = ControllerMgr.GetRdsMsgTime(MqttControllerId);
            rdsMillis         = currentMillis;

            sprintf(logBuff,
                    "MQTT Controller RDS Will Use: PI=0x%04X, PTY=%u.",
                    ControllerMgr.GetPiCode(MqttControllerId),
                    ControllerMgr.GetPtyCode(MqttControllerId));
            Log.infoln(logBuff);
            radio.setPiCode(ControllerMgr.GetPiCode(MqttControllerId)); // Set MQQT Controller's Pi Code.
            radio.setPtyCode(ControllerMgr.GetPtyCode(MqttControllerId)); // Set MQTT Controller's PTY Code.

            sprintf(logBuff, "MQTT Controller Sending RDS Program Service Name (%s)", rdsRefreshPsnStr.c_str());
            Log.infoln(logBuff);
            radio.sendStationName(rdsRefreshPsnStr);
            sprintf(logBuff, "MQTT Controller Sending RDS RadioText (%s).", rdsRefreshTextStr.c_str());
            Log.infoln(logBuff);
            radio.sendRadioText(rdsRefreshTextStr);
            updateUiRdsText(rdsRefreshTextStr);
            ControllerMgr.Display(MqttControllerId);
        }
        else if (ControllerMgr.GetControllerEnabledFlag(HttpControllerId) &&
                 ControllerMgr.GetTextFlag(HttpControllerId) &&
                 !ControllerMgr.GetActiveTextFlag(SerialControllerId) &&
                 !ControllerMgr.GetActiveTextFlag(MqttControllerId))
        { // Priority #3, New HTTP RadioText
            ControllerMgr.SetTextFlag(HttpControllerId, false);
            ControllerMgr.SetStopFlag(HttpControllerId, false);
            ControllerMgr.SetActiveTextFlag(HttpControllerId, true);
            ControllerMgr.SetActiveTextFlag(LocalControllerId, false);

            rdsRefreshPsnStr  = ControllerMgr.GetRdsProgramServiceName(HttpControllerId);
            rdsRefreshTextStr = ControllerMgr.GetPayloadText(HttpControllerId);
            rdsMsgTime        = ControllerMgr.GetRdsMsgTime(HttpControllerId);
            rdsMillis         = currentMillis;

            sprintf(logBuff,
                    "HTTP Controller RDS Will Use: PI=0x%04X, PTY=%u.",
                    ControllerMgr.GetPiCode(HttpControllerId),
                    ControllerMgr.GetPtyCode(HttpControllerId));
            Log.infoln(logBuff);
            radio.setPiCode(ControllerMgr.GetPiCode(HttpControllerId)); // Set HTTP Controller's Pi Code.
            radio.setPtyCode(ControllerMgr.GetPtyCode(HttpControllerId)); // Set HTTP Controller's PTY Code.

            sprintf(logBuff, "HTTP Controller Sending RDS Program Service Name (%s)", rdsRefreshPsnStr.c_str());
            Log.infoln(logBuff);
            radio.sendStationName(rdsRefreshPsnStr);
            sprintf(logBuff, "HTTP Controller Sending RDS RadioText (%s).", rdsRefreshTextStr.c_str());
            Log.infoln(logBuff);
            radio.sendRadioText(rdsRefreshTextStr);
            updateUiRdsText(rdsRefreshTextStr);
            ControllerMgr.Display(HttpControllerId);
        }
        else if (!checkLocalRdsAvail()  && checkAnyRdsControllerAvailable() && !ControllerMgr.IsControllerActive()) {
            updateUiRDSTmr(0);                             // Clear Displayed Elapsed Timer.
            displayRdsText();
            rdsMillis = currentMillis - rdsMsgTime + 500;  // Schedule next RadioText in 0.5Sec.
            return;
        }
        else if (currentMillis - rdsMillis < rdsMsgTime) { // RadioText Message Display Time has not ended yet.
            updateUiRDSTmr(rdsMillis);                     // Update Countdown time on GUI homeTab.

            if (rfCarrierFlg) {                            // When On-Air Send Program Service Name (PSN) every second.
                sprintf(logBuff, "Refreshing RDS Program Service Name: %s.", rdsRefreshPsnStr.c_str());
                Log.traceln(logBuff);
                radio.sendStationName(rdsRefreshPsnStr);

                if (((currentMillis - rdsMillis) / 1000) % RDS_REFRESH_TM == 0) { // Time to refresh RadioText.
                    if (rdsRefreshTextStr.length() && (currentMillis - rdsMillis >= RDS_MSG_UPD_TIME)) {
                        sprintf(logBuff, "Refreshing RDS RadioText Message: %s", rdsRefreshTextStr.c_str());
                        Log.traceln(logBuff);
                        radio.sendRadioText(rdsRefreshTextStr);
                    }
                }
            }
        }

    if (!rfCarrierFlg) {                               // Radio Turned Off, nothing else to do. Exit.
        return;
    }
    else if (rdsMillis == 0) {                         // First Function Call, Init RDS Message Countdown Timer Time.
        rdsMillis = millis() + rdsMsgTime;
    }
    else if (currentMillis - rdsMillis < rdsMsgTime) { // Countdown still active. We're done for now, exit.
        return;
    }

    /* Countdown Now Zero. RDS RadioText Message Time has ended. */
    updateUiRDSTmr(rdsMillis); // Show "Expired" on GUI homeTab's RadioText Timer.

    /* Let's Check to see who supplied the RadioText and terminate it. */
    if (ControllerMgr.GetActiveTextFlag(SerialControllerId)) { // USB Serial RDS Controller is #1 Priority.
        ControllerMgr.SetActiveTextFlag(SerialControllerId, false);
        Log.infoln("Serial Controller's RDS Time has Ended.");
    }
    else if (ControllerMgr.GetActiveTextFlag(MqttControllerId)) { // MQTT RDS Controller is #2 Priority.
        ControllerMgr.SetActiveTextFlag(MqttControllerId, false);
        Log.infoln("MQTT Controller's RDS Time has Ended.");
    }
    else if (ControllerMgr.GetActiveTextFlag(HttpControllerId)) { // HTTP RDS Controller is #3 Priority.
        ControllerMgr.SetActiveTextFlag(HttpControllerId, false);
        Log.infoln("HTTP Controller's RDS Time has Ended.");
    }
    else if (checkLocalRdsAvail()) { // Local RDS is Lowest Priority.
        ControllerMgr.SetActiveTextFlag(LocalControllerId, true);
        rdsMsgTime = ControllerMgr.GetRdsMsgTime(LocalControllerId);
        rdsRefreshPsnStr = ControllerMgr.GetRdsProgramServiceName(LocalControllerId);

        sprintf(logBuff,
                "Local Controller RDS Will Use: PI=0x%04X, PTY=%u.",
                ControllerMgr.GetPiCode(LocalControllerId),
                ControllerMgr.GetPtyCode(LocalControllerId));
        Log.infoln(logBuff);
        radio.setPiCode(ControllerMgr.GetPiCode(LocalControllerId)); // Set Local Controller's Pi Code.
        radio.setPtyCode(ControllerMgr.GetPtyCode(LocalControllerId)); // Set Local Controller's PTY Code.

        sprintf(logBuff, "Local Controller Sending RDS Station Name (%s).", ControllerMgr.GetRdsProgramServiceName(LocalControllerId).c_str());
        Log.infoln(logBuff);
        radio.sendStationName(ControllerMgr.GetRdsProgramServiceName(LocalControllerId));

        sprintf(logBuff, "Local Controller Sending RDS RadioText Message");
        Log.infoln(logBuff);

        // Find Next Available Local RadioText Message.
        if ((loop == 0) && !rdsText1EnbFlg) { // RDS Message Disabled, next.
            Log.traceln("-> RDS Text Msg1 is Disabled, Skip to Next");
            loop = 1;
        }

        if ((loop == 1) && !rdsText2EnbFlg) { // RDS Message Disabled, next.
            Log.traceln("-> RDS Text Msg2 is Disabled, Skip to Next");
            loop = 2;
        }

        if ((loop == 2) && !rdsText3EnbFlg) { // This can't happen!. Reset Loop.
            Log.traceln("-> RDS Text Msg3 is Disabled, Skip to Beginning");
            loop = 0;
        }

        updateUiRdsText(" "); // Clear RDS RadioText In Web UI; Use single-space char!

        // Send the chosen RadioText.
        if (loop == 0) {
            if (rdsText1EnbFlg) {
                rdsTextMsgStr     = rdsTextMsg1Str;
                rdsRefreshTextStr = rdsTextMsgStr;
                radio.sendRadioText(rdsTextMsgStr);
                updateUiRdsText(rdsTextMsgStr);
                ControllerMgr.Display(LocalControllerId);
                rdsMillis = millis();
            }
        }
        else if (loop == 1) {
            if (rdsText2EnbFlg) {
                rdsTextMsgStr     = rdsTextMsg2Str;
                rdsRefreshTextStr = rdsTextMsgStr;
                radio.sendRadioText(rdsTextMsgStr);
                updateUiRdsText(rdsTextMsgStr);
                ControllerMgr.Display(LocalControllerId);
                rdsMillis = millis();
            }
        }
        else if (loop == 2) {
            if (rdsText3EnbFlg) {
                rdsTextMsgStr     = rdsTextMsg3Str;
                rdsRefreshTextStr = rdsTextMsgStr;
                radio.sendRadioText(rdsTextMsgStr);
                updateUiRdsText(rdsTextMsgStr);
                ControllerMgr.Display(LocalControllerId);
                rdsMillis = millis();
            }
        }
        else {
            Log.traceln("-> Will Resync Local RDS RadioText on Next Round.");
        }

        sprintf(logBuff, "-> RDS RadioText Sent \"%s\".", rdsTextMsgStr.c_str());
        Log.infoln(logBuff);

        if (++loop > 2) {
            loop = 0;
        }
    }
    else { // No available RadioText message.
        ControllerMgr.SetActiveTextFlag(SerialControllerId, false);
        ControllerMgr.SetActiveTextFlag(MqttControllerId, false);
        ControllerMgr.SetActiveTextFlag(HttpControllerId, false);
        ControllerMgr.SetActiveTextFlag(LocalControllerId, false);

        loop              = 0;                            // Reset Local RadioText to first message.
        rdsRefreshTextStr = "";                           // Erase RadioText Refresh message.
        rdsMillis         = millis() - rdsMsgTime + 1000; // Schedule next RadioText in 1Sec.
        Log.warningln("-> No RDS RadioText Available, Nothing Sent.");
        ControllerMgr.Display(NullControllerId);
    }

    updateUiRDSTmr(rdsMillis); // Refresh Countdown time on GUI homeTab.
}
}

// ************************************************************************************************
// resetControllerRdsValues(): Reset the Local, Serial, MQTT, and HTTP Controllers' RDS Initial
// Runtime Values. These values can be changed by Serial/MQTT/HTTP controller commands.
// Call this function during boot, after restoreConfiguration().
void resetControllerRdsValues(void)
{
    // Local RDS Controller
    radio.setPiCode(ControllerMgr.GetPiCode(LocalControllerId)); // Local RDS PI Code is Fixed Value.
    radio.setPtyCode(ControllerMgr.GetPtyCode(LocalControllerId)); // Default RDS PTY Code.

    // USB Serial RDS Controller. All values can be changed during runtime by Serial Commands.
    ControllerMgr.SetRdsProgramServiceName(SerialControllerId, ControllerMgr.GetRdsProgramServiceName(LocalControllerId)); 	// Default Program Service Name (Mimic Local Controller).
    ControllerMgr.SetPayloadText(SerialControllerId, ""); 																	// Clear Controller's RadioText Message.
    ControllerMgr.SetPiCode(SerialControllerId, ControllerMgr.GetPiCode(LocalControllerId)); 								// Default PI Code (Mimic Local Controller).
    ControllerMgr.SetPtyCode(SerialControllerId, ControllerMgr.GetPiCode(LocalControllerId));                               // Default PTY Code (Mimic Local Controller).
    ControllerMgr.SetRdsMsgTime(SerialControllerId, ControllerMgr.GetRdsMsgTime(LocalControllerId)); 						// Default RDS Message Time (Mimic Local Controller),

    // MQTT RDS Controller. All values can be changed during runtime by MQTT Commands.
    ControllerMgr.SetRdsProgramServiceName(MqttControllerId, ControllerMgr.GetRdsProgramServiceName(LocalControllerId)); 	// Default Program Service Name (Mimic Local Controller).
    ControllerMgr.SetPayloadText(MqttControllerId, ""); 																	// Clear Controller's RadioText Message.
    ControllerMgr.SetPiCode(MqttControllerId, ControllerMgr.GetPiCode(LocalControllerId)); 									// Default PI Code (Mimic Local Controller).
    ControllerMgr.SetPtyCode(MqttControllerId, ControllerMgr.GetPiCode(LocalControllerId));                                 // Default PTY Code (Mimic Local Controller).
	ControllerMgr.SetRdsMsgTime(MqttControllerId, ControllerMgr.GetRdsMsgTime(LocalControllerId)); 							// Default RDS Message Time (Mimic Local Controller),

    // HTTP RDS Controller. All values can be changed during runtime by HTTP Commands.
    ControllerMgr.SetRdsProgramServiceName(HttpControllerId, ControllerMgr.GetRdsProgramServiceName(LocalControllerId)); 	// Default Program Service Name (Mimic Local Controller).
    ControllerMgr.SetPayloadText(HttpControllerId, ""); 																	// Clear Controller's RadioText Message.
    ControllerMgr.SetPiCode(HttpControllerId, ControllerMgr.GetPiCode(LocalControllerId)); 									// Default PI Code (Mimic Local Controller).
    ControllerMgr.SetPtyCode(HttpControllerId, ControllerMgr.GetPiCode(LocalControllerId));                                 // Default PTY Code (Mimic Local Controller).
    ControllerMgr.SetRdsMsgTime(HttpControllerId, ControllerMgr.GetRdsMsgTime(LocalControllerId)); 							// Default RDS Message Time (Mimic Local Controller),

}
#endif // def OldWay

// ************************************************************************************************
// EOF
