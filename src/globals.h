/*
   File: globals.h
   Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
   Version: 1.1.0
   Creation: Dec-16-2021
   Revised:  Jun-13-2022
   Revision History: See PixelRadio.cpp
   Project Leader: T. Black (thomastech)
   Contributors: thomastech, dkulp

   (c) copyright T. Black 2021-2022.
   Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
   This Code was formatted with the uncrustify extension.
 */

// *********************************************************************************************
#pragma once

// *********************************************************************************************

extern bool newVgaGainFlg;
extern bool newAutoRfFlg;
extern bool newCarrierFlg;
extern bool newDigGainFlg;
extern bool newFreqFlg;
extern bool newGpio19Flg;
extern bool newGpio23Flg;
extern bool newGpio33Flg;
extern bool newInpImpFlg;
extern bool newAudioModeFlg;
extern bool newMuteFlg;
extern bool newPreEmphFlg;
extern bool newRfPowerFlg;
extern bool muteFlg;
extern bool rebootFlg;
extern bool rfAutoFlg;
extern bool rfCarrierFlg;
extern bool stereoEnbFlg;
extern bool testModeFlg;

extern uint8_t analogVol;
extern uint8_t fmRadioTestCode;
extern uint8_t usbVol;

extern uint32_t rdsMsgTime;

extern uint16_t fmFreqX10;
extern float vbatVolts;
extern float paVolts;

// GUI Strings
extern String preEmphasisStr;
extern String digitalGainStr;
extern String gpio19BootStr;
extern String gpio23BootStr;
extern String gpio33BootStr;
extern String gpio19CtrlStr;
extern String gpio23CtrlStr;
extern String gpio33CtrlStr;
extern String inpImpedStr;
extern String logLevelStr;
extern String rfPowerStr;
extern String userNameStr;
extern String userPassStr;
extern String vgaGainStr;
