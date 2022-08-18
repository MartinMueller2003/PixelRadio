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
#pragma once

// *********************************************************************************************
#include <Arduino.h>

// *********************************************************************************************

extern bool     newAutoRfFlg;
extern bool     newGpio19Flg;
extern bool     newGpio23Flg;
extern bool     newGpio33Flg;
extern bool     rebootFlg;
extern uint8_t usbVol;

extern float vbatVolts;

// GUI Strings
extern String   gpio19BootStr;
extern String   gpio23BootStr;
extern String   gpio33BootStr;
extern String   gpio19CtrlStr;
extern String   gpio23CtrlStr;
extern String   gpio33CtrlStr;
extern String   logLevelStr;
