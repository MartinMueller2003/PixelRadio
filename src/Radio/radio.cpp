/*
   File: radio.cpp
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech

   (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#include <Arduino.h>
#include <ArduinoLog.h>
#include <Wire.h>
#include "PixelRadio.h"
#include "globals.h"
#include "language.h"
#include "radio.hpp"
#include "QN8027RadioApi.hpp"
#include "memdebug.h"

static const uint16_t toneList[] =
{ TONE_NONE, TONE_NONE, TONE_NONE, TONE_A3, TONE_E4, TONE_A3, TONE_C4, TONE_C5, TONE_F4, TONE_F4, TONE_A4, TONE_NONE };
static const uint8_t listSize = sizeof(toneList) / sizeof(uint16_t);

// *********************************************************************************************
void cRadio::begin()
{
    // DEBUG_START;

    QN8027RadioApi.begin(); // If QN8027 fails we will warn user on UI homeTab.
    Log.infoln(String(F("FM Radio RDS/RBDS Started.")).c_str());

    // DEBUG_END;
}

// ************************************************************************************************
// getAudioGain(): Compute the Audio Gain.
// Radio Audio Gain in dB = ((Analog Input Gain Step + 1) * 3) - (Input Impedance Step * 3)
// Please note that the formula has been modified based on real-world measurements.
// The official formula uses 6dB Impedance steps vs the revised use of 3dB.
int8_t cRadio::getAudioGain(void)
{
    // DEBUG_START;
    int8_t audioGain = 0;
#ifdef OldWay
    int8_t impedance;
    int8_t vgaGain;

    if (vgaGainStr.equals(VGA_GAIN0_STR))
    {
        vgaGain = 0;
    }
    else if (vgaGainStr.equals(VGA_GAIN1_STR))
    {
        vgaGain = 1;
    }
    else if (vgaGainStr.equals(VGA_GAIN2_STR))
    {
        vgaGain = 2;
    }
    else if (vgaGainStr.equals(VGA_GAIN3_STR))
    {
        vgaGain = 3;
    }
    else if (vgaGainStr.equals(VGA_GAIN4_STR))
    {
        vgaGain = 4;
    }
    else if (vgaGainStr.equals(VGA_GAIN5_STR))
    {
        vgaGain = 5;
    }
    else 
    {
        vgaGain = 0;
        Log.errorln("getAudioGain: Undefined vgaGainStr!");
    }

    if (inpImpedStr.equals(INP_IMP05K_STR))
    {
        impedance = 0;
    }
    else if (inpImpedStr.equals(INP_IMP10K_STR))
    {
        impedance = 1;
    }
    else if (inpImpedStr.equals(INP_IMP20K_STR))
    {
        impedance = 2;
    }
    else if (inpImpedStr.equals(INP_IMP40K_STR))
    {
        impedance = 3;
    }
    else 
    {
        impedance = 0;
        Log.errorln(F("displayAudioGain: Undefined inpImpedStr!"));
    }

    audioGain = ((vgaGain + 1) * 3) - (impedance * 3);
#endif // def OldWay

    // DEBUG_END;
    return audioGain;
}

// *********************************************************************************************
void cRadio::Poll()
{
    /// DEBUG_START;

    updateTestTones(false);

    unsigned long now = millis();

    do // once
    {
        // has it been one second since the last update?
        if (1000 > (now - CurrentMsgLastUpdateTime))
        {
            // need to wait a bit longer
            break;
        }
        CurrentMsgLastUpdateTime = now;
        /// DEBUG_V("One second later");

        // has the current message output expired?
        if(now < CurrentMsgEndTime)
        {
            /// DEBUG_V("still waiting");
            updateRdsMsgRemainingTime(now);
            break;
        }

        /// DEBUG_V("time for a new message");
        ControllerMgr.GetNextRdsMessage(RdsMsgInfo);
        CurrentMsgEndTime = now + RdsMsgInfo.DurationMilliSec;
        if(!LastMessageSent.equals(RdsMsgInfo.Text))
        {
            /// DEBUG_V("Display the new message");
            LastMessageSent = RdsMsgInfo.Text;
            setRdsMessage();
            updateUiRdsText(RdsMsgInfo.Text);
        }
        updateRdsMsgRemainingTime(now);

    } while (false);

    /// DEBUG_END;
}

// *********************************************************************************************
void cRadio::restoreConfiguration(JsonObject & config)
{
    // DEBUG_START;

    ReadFromJSON(rfCarrierFlg,      config, F("RADIO_RF_CARR_FLAG"));
    ReadFromJSON(preEmphasisStr,    config, F("RADIO_PRE_EMPH_STR"));
    ReadFromJSON(rfPowerStr,        config, F("RADIO_POWER_STR"));
    ReadFromJSON(vgaGainStr,        config, F("ANALOG_GAIN_STR"));
    ReadFromJSON(digitalGainStr,    config, F("DIGITAL_GAIN_STR"));

    AudioInputImpedance.restoreConfiguration(config);
    AudioMute.restoreConfiguration(config);
    AudioMode.restoreConfiguration(config);

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::saveConfiguration(JsonObject & config)
{
    // DEBUG_START;

    AudioInputImpedance.restoreConfiguration(config);
    AudioMute.saveConfiguration(config);
    AudioMode.saveConfiguration(config);

    config[F("RADIO_FM_FREQ")]          = fmFreqX10;      // Use radio.setFrequency(MHZ) when restoring this uint16 value.
    config[F("RADIO_AUTO_FLAG")]        = rfAutoFlg;      // Use radio.radioNoAudioAutoOFF(0/1) when restoring this uint8 Value.
    config[F("RADIO_RF_CARR_FLAG")]     = rfCarrierFlg;   // Use radio.RDS(0/1) when restoring this uint8 value. 1=CarrierOn.
    config[F("RADIO_PRE_EMPH_STR")]     = preEmphasisStr; // Use radio.setPreEmphTime50(0/1), uint8 value. Not working?
    config[F("RADIO_POWER_STR")]        = rfPowerStr;     // Use radio.setTxPower(20-75) when restoring this uint8 value.

    config[F("ANALOG_VOLUME")]          = analogVol;        // Requires custom function, not written yet.
    config[F("ANALOG_GAIN_STR")]        = vgaGainStr;       // Use radio.setTxInputBufferGain(0-5) when restoring this Int value.
    
    config[F("RDS_PI_CODE")]            = PiCode; // Use radio.setPiCode() when restoring this hex value.
    config[F("RDS_PTY_CODE")]           = PtyCode;
    config[F("RDS_PROG_SERV_STR")]      = ProgramServiceName;

    config[F("DIGITAL_GAIN_STR")]       = digitalGainStr;   // Use radio.setTxDigitalGain(0/1/2) when restoring this Int value.

    // DEBUG_END;
}

// *********************************************************************************************
// setDigitalGain(): Set the Tx Digital Gain on the QN8027 chip.
void cRadio::setDigitalGain(void)
{
    // DEBUG_START;

    uint8_t gainVal = 0;

    // Log.verboseln(String(F( "-> Digital Audio Gain Set to: %s.")).c_str(), digitalGainStr.c_str());

    if (digitalGainStr.equals(DIG_GAIN0_STR))
    {
        gainVal = 0; // 0dB.
    }
    else if (digitalGainStr.equals(DIG_GAIN1_STR))
    {
        gainVal = 1; // 1dB.
    }
    else if (digitalGainStr.equals(DIG_GAIN2_STR))
    {
        gainVal = 2; // 2dB.
    }
    else 
    {
        Log.errorln(String(F("setDigitalGain: Invalid Value, Using Default")).c_str());
        digitalGainStr = DIG_GAIN_DEF_STR;
        gainVal        = 0;
    }
#ifdef OldWay

    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        delay(5);
        FmRadio.setTxDigitalGain(gainVal);
        delay(5);
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
// setFrequency(): Set the Radio Frequency on the QN8027.
void cRadio::setFrequency(uint32_t value)
{
    // DEBUG_START;

    fmFreqX10 = value;
    setFrequency();
    updateUiFrequency(fmFreqX10);

    // DEBUG_END;
}

// *********************************************************************************************
// setFrequency(): Set the Radio Frequency on the QN8027.
void cRadio::setFrequency(void)
{
    // DEBUG_START;

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        waitForIdle(5);
        FmRadio.setFrequency((float(fmFreqX10)) / 10.0f);
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setPiCode(uint16_t value)
{
    // DEBUG_START;

    PiCode = value;
    setPiCode();
    String Response = String(F("0x")) + String(PiCode, HEX);
    ESPUI.print(rdsPiID, Response);

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setPiCode()
{
    // DEBUG_START;

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.setPiCode(PiCode);
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
// ssetPreEmphasis(): Set the QN8027 chip's pre-Emphasis.
// ON = 50uS (Eur/UK/Australia), OFF = 75uS (North America/Japan).
void cRadio::setPreEmphasis(void)
{
    // DEBUG_START;
#ifdef OldWay

    if (preEmphasisStr.equals(PRE_EMPH_EUR_STR))
    {
        emphVal = PRE_EMPH_EUR_VAL;
    }
    else if (preEmphasisStr.equals(PRE_EMPH_USA_STR))
    {
        emphVal = PRE_EMPH_USA_VAL;
    }
    else 
    {
        Log.errorln("setPreEmphasis: Invalid Value, Using Default");
        preEmphasisStr = PRE_EMPH_DEF_STR;
        emphVal        = PRE_EMPH_DEF_VAL;
    }

    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.setPreEmphTime50(emphVal);
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }

#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setProgramServiceName()
{
    // DEBUG_START;
#ifdef OldWay

    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.sendStationName(ProgramServiceName);
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }

#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setPtyCode()
{
    // DEBUG_START;

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.setPtyCode(PtyCode);
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
void cRadio::setRdsMessage()
{
    // DEBUG_START;

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        waitForIdle(50);
        FmRadio.sendRadioText(RdsMsgInfo.Text);
        xSemaphoreGiveRecursive(RadioSemaphore);

        Log.traceln(String(F("Refreshing RDS RadioText Message: %s")).c_str(), RdsMsgInfo.Text.c_str());
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
// setRfAutoOff():
// IMPORTANT: Sending RDS Messages and/or using updateUiAudioLevel() Will prevent 60S Turn Off.
// rfAutoOff = true: Turn-off RF Carrier if Audio is missing for > 60 seconds.
// rfAutoOff = false: Never turn off.
void cRadio::setRfAutoOff(void)
{
    // DEBUG_START;

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        setRfCarrier(OFF);
        FmRadio.radioNoAudioAutoOFF(rfAutoFlg ? ON : OFF );
        setRfCarrier();
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
// setRfCarrier(): Set the QN8027 chip's RF Carrier Output (On/Off).
void cRadio::setRfCarrier(void)
{
    // DEBUG_START;

    setRfCarrier(rfCarrierFlg);

    // DEBUG_END;
}

// *********************************************************************************************
// setRfCarrier(): Set the QN8027 chip's RF Carrier Output (On/Off).
void cRadio::setRfCarrier(bool value)
{
    // DEBUG_START;
    // DEBUG_V(String("value: ") + String(value));

#ifdef OldWay
    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        waitForIdle(10);
        FmRadio.Switch(value ? ON : OFF ); // Update QN8027 Carrier.
        waitForIdle(25);
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
// setRfPower(): Set the QN8027 chip's RF Power Output. Max 121dBuVp.
// Note: Power is not changed until RF Carrier is toggled Off then On.
void cRadio::setRfPower(void)
{
    // DEBUG_START;

#ifdef OldWay
    uint8_t pwrVal = RF_HIGH_POWER;

    // Log.verboseln(String(F( "-> RF Power Set to: %s")).c_str(), rfPowerStr.c_str());

    if (rfPowerStr.equals(RF_PWR_LOW_STR))
    {
        pwrVal = RF_LOW_POWER;
    }
    else if (rfPowerStr.equals(RF_PWR_MED_STR))
    {
        pwrVal = RF_MED_POWER;
    }
    else if (rfPowerStr.equals(RF_PWR_HIGH_STR))
    {
        pwrVal = RF_HIGH_POWER;
    }
    else 
    {
        Log.errorln(String(F("setRfPower: Invalid Value, Using High Power")).c_str());
        rfPowerStr = RF_PWR_HIGH_STR;
        pwrVal     = RF_HIGH_POWER;
    }

    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        waitForIdle(10);
        FmRadio.setTxPower(pwrVal);
        waitForIdle(10);

        if (rfCarrierFlg)
        {
            setRfCarrier(OFF);
            setRfCarrier();
        }
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay
    // DEBUG_END;
}

// *********************************************************************************************
// setVgaGain(): Set the Tx Input Buffer Gain (Analog Gain) on the QN8027 chip.
void cRadio::setVgaGain(void)
{
    // DEBUG_START;
    uint8_t gainVal = 0;

    if (vgaGainStr.equals(VGA_GAIN0_STR))
    {
        gainVal = 0x00;
    }
    else if (vgaGainStr.equals(VGA_GAIN1_STR))
    {
        gainVal = 0x01;
    }
    else if (vgaGainStr.equals(VGA_GAIN2_STR))
    {
        gainVal = 0x02;
    }
    else if (vgaGainStr.equals(VGA_GAIN3_STR))
    {
        gainVal = 0x03;
    }
    else if (vgaGainStr.equals(VGA_GAIN4_STR))
    {
        gainVal = 0x04;
    }
    else if (vgaGainStr.equals(VGA_GAIN5_STR))
    {
        gainVal = 0x05;
    }
    else 
    {
        Log.errorln(String(F("setVgaGain: Invalid Value, Using Default")).c_str());
        vgaGainStr = VGA_GAIN3_STR;
        gainVal    = 0x03;
    }
#ifdef OldWay

    if (RadioSemaphore)
    {
        xSemaphoreTakeRecursive(RadioSemaphore, portMAX_DELAY);
        delay(5);
        FmRadio.setTxInputBufferGain(gainVal);
        delay(5);
        xSemaphoreGiveRecursive(RadioSemaphore);
    }
#endif // def OldWay
    // DEBUG_END;
}

// ************************************************************************************************
// updateUiRfCarrier(): Updates the GUI's RF Carrier on homeTab and radiotab.
void cRadio::updateUiRfCarrier(void)
{
    // DEBUG_START;
#ifdef OldWay

    extern uint32_t paVolts;

    ESPUI.updateControlValue(radioRfEnbID, String(rfCarrierFlg ? F("1") : F("0")));

    if (rfCarrierFlg == true) 
    {
        if (fmRadioTestCode == FM_TEST_FAIL) 
        {
            ESPUI.print(homeOnAirID, RADIO_FAIL_STR);   // Update homeTab panel.
        }
        else if (fmRadioTestCode == FM_TEST_VSWR) 
        {
            ESPUI.print(homeOnAirID, RADIO_VSWR_STR);   // Update homeTab panel.
        }
        else if ((paVolts < PA_VOLT_MIN) || (paVolts > PA_VOLT_MAX)) 
        {
            ESPUI.print(homeOnAirID, RADIO_VOLT_STR);   // Update homeTab panel.
        }
        else 
        {
            ESPUI.print(homeOnAirID, RADIO_ON_AIR_STR); // Update homeTab panel.
        }
    }
    else 
    {
        ESPUI.print(homeOnAirID, RADIO_OFF_AIR_STR); // Update homeTab panel.
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
// updateOnAirSign(): Turn on the "On Air" LED Sign if RF Carrier is present.
void cRadio::updateOnAirSign(void)
{
    // DEBUG_START;
#ifdef OldWay
new way: call GPIO and tell it to update the output and the UI.

    if (rfCarrierFlg)
    {
        digitalWrite(ON_AIR_PIN, SIGN_ON);
    }
    else 
    {
        digitalWrite(ON_AIR_PIN, SIGN_OFF);
    }
#endif // def OldWay

    // DEBUG_END;
}

// *********************************************************************************************
// updateTestTones(): Test Tone mode creates cascading audio tones for Radio Installation Tests.
//                    On entry, true will Reset Tone Sequence.
//                    The tone sequence is sent every five seconds.
//                    Also Sends Special RadioText message with approximate Elapsed Time.
void cRadio::updateTestTones(bool resetTimerFlg)
{
    /// DEBUG_START;
    
    char rdsBuff[sizeof(AUDIO_TEST_STR) + 25];
    static bool rstFlg          = false; // Reset State Machine FLag.
    static bool goFlg           = false; // Go Flag, send new tone sequence if true.
    static bool toneFlg         = false; // Tone Generator is On Flag.
    static uint8_t  hours       = 0;
    static uint8_t  minutes     = 0;
    static uint8_t  seconds     = 0;
    static uint8_t  state       = 0;
    static uint32_t clockMillis = millis();
    static uint32_t timerMillis = clockMillis;
    uint32_t currentMillis      = clockMillis;

    do // once
    {
        if (resetTimerFlg)
        { // Caller requests a full state machine reset on next Test Tone.
            rstFlg = true;
            break;
        }

        if (!testModeFlg) 
        {
            /// DEBUG_V("Not In Test Mode")
            goFlg = false;
            state = 0;
            digitalWrite(MUX_PIN, TONE_OFF); // Switch Audio Mux chip to Line-In.

            if (toneFlg == true) 
            {
                /// DEBUG_V("Kill active tone generator.");
                toneFlg = false;
                toneOff(TONE_PIN, TEST_TONE_CHNL);
            }
            break;
        }

        /// DEBUG_V();
        digitalWrite(MUX_PIN, TONE_ON); // Switch Audio Mux chip to Test Tones.

        if (rstFlg == true) 
        {           // State machine reset was requested.
            rstFlg      = false;
            goFlg       = true;         // Request tone sequence now.
            clockMillis = millis();
            timerMillis = clockMillis;
            hours       = 0;
            minutes     = 0;
            seconds     = 0;
            state       = 0;
        }

        // Update the test tone clock. HH:MM:SS will be sent as RadioText.
        if ((currentMillis - clockMillis) >= 1000) 
        {
            clockMillis = millis() - ((currentMillis - clockMillis) - 1000);
            seconds++;

            if (seconds >= 60) 
            {
                seconds = 0;
                minutes++;

                if (minutes >= 60) 
                {
                    minutes = 0;
                    hours++;

                    if (hours >= 100) 
                    { // Clock wraps at 99:59:59.
                        hours = 0;
                    }
                }
            }
        }

        if (seconds % 5 == 0) 
        { // Send new tone sequence every five seconds.
            goFlg = true;
        }

        if (goFlg && (millis() >= timerMillis + TEST_TONE_TIME)) 
        {
            timerMillis = millis();
            toneFlg     = false;
            toneOff(TONE_PIN, TEST_TONE_CHNL);
            delay(5);               // Allow a bit of time for tone channel to shutdown.

            if (state < listSize) 
            { // Cycle through the tone table.
                if (state == 0) 
                {   // Time to send RadioText message.
                    state++;
                    sprintf(rdsBuff, "%s  [ %02u:%02u:%02u ]", AUDIO_TEST_STR, hours, minutes, seconds);
                    String tmpStr = rdsBuff;
                    sendStationName(AUDIO_PSN_STR);
                    sendRadioText(tmpStr);

                    updateUiRdsText(tmpStr);

                    Log.verboseln(String(F("New Test Tone Sequence, RadioText Sent.")).c_str());
                    break;                // We will send the tones on next entry.
                }

                if (toneList[state] > 0) 
                { // Time to send tones from toneList[] table).
                    toneFlg = true;
                    toneOn(TONE_PIN, toneList[state], TEST_TONE_CHNL);
                }

                state++;
            }
            else 
            { // At end of Tone table, done with this sequence.
                state = 0;
                goFlg = false;
            }
        }
    } while (false);

    /// DEBUG_END;
}

// *********************************************************************************************
// waitForIdle(): Wait for QN8027 to enter Idle State. This means register command has executed.
void cRadio::waitForIdle(uint16_t waitMs) 
{
    // DEBUG_START;
#ifdef OldWay
    uint8_t stateCode1;
    uint8_t stateCode2;

    do // once
    {
        stateCode1 = FmRadio.getStatus() & 0x07;
        // DEBUG_V(String("stateCode1: 0x") + String(stateCode1, HEX));

        // make sure the delay is a minimum of 5ms
        if(5 > waitMs) {waitMs = 5;}

        do
        {
            waitMs -= 5;

            delay(5);

            stateCode2 = FmRadio.getStatus() & 0x07;
            // DEBUG_V(String("stateCode2: 0x") + String(stateCode2, HEX));

            if ((stateCode2 == 0x02) || (stateCode2 == 0x05))
            {
                break;
            }
            else if  (stateCode1 != stateCode2) 
            {
                FmRadio.getStatus(); // Clear next status byte.
                break;
            }
        } while (5 <= waitMs);

    } while (false);

    // DEBUG_V(String("waitForIdle End Status is: 0x") + String(FmRadio.getStatus() & 0x07, HEX));
#endif // def OldWay
    // DEBUG_END;
}

#ifdef OldWay
// *********************************************************************************************
// updateRadioSettings(): Update Any Radio Setting that has changed by the Web UI.
// This routine must be placed in main loop();
// QN8027 specific settings must not be changed in the Web UI Callbacks due to ESP32 core
// limitations. So instead the callbacks set a flag that tells this routine to perform the action.
void cRadio::updateRadioSettings(void)
{
    // DEBUG_START;

    if (newAutoRfFlg) 
    {
        newAutoRfFlg = false;
        setRfAutoOff();
    }

    // DEBUG_END;
}
#endif // def OldWay

// *********************************************************************************************
cRadio  Radio;

// *********************************************************************************************
// OEF
