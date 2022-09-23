/*
  *    File: misc.cpp
  *    Project: PixelRadio, an RBDS/RDS FM Transmitter (QN8027 Digital FM IC)
  *    Version: 1.1.0
  *    Creation: Dec-16-2021
  *    Revised:  Jun-13-2022
  *    Revision History: See PixelRadio.cpp
  *    Project Leader: T. Black (thomastech)
  *    Contributors: thomastech
  *
  *    (c) copyright T. Black 2021-2022, Licensed under GNU GPL 3.0 and later, under this license absolutely no warranty is given.
  *    This Code was formatted with the uncrustify extension.
  */

// *********************************************************************************************
#include "language.h"
#include "PixelRadio.h"
#include "radio.hpp"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>

// *********************************************************************************************
// *********************************************************************************************
// initEprom():  Initialize EEPROM emulation.
// Note: EEPROM is Currently not used in PixelRadio; Available for future use.
void initEprom (void)
{
    EEPROM.begin (EEPROM_SZ);

    if (EEPROM.read (INIT_ADDR) != INIT_BYTE)
    {
        EEPROM.write (INIT_ADDR, INIT_BYTE);

        // EEPROM.write(VOL_SET_ADDR, spkrVolSwitch);
        EEPROM.commit ();
        Log.warningln (String (F ("Initialized Virgin EEPROM (detected first use).")).c_str ());
    }
    else
    {
        // spkrVolSwitch = EEPROM.read(VOL_SET_ADDR);
        Log.verboseln (String (F ("Restored settings from EEPROM.")).c_str ());
    }
}

// *********************************************************************************************
// spiSdCardShutDown(): Shutdown SD Card SPI Interface and idle important ESP32 Boot Mode Pins.
// This function should be called after any SPI usage (such as RD/WR SD Card). It will allow for
// more reliable Code Flashing if the SD Card remains installed.
void spiSdCardShutDown (void)
{
    pinMode (SD_CS_PIN, OUTPUT);                // SD Card Chip Select.
    digitalWrite (SD_CS_PIN, HIGH);             // CS Active Low. High = Disable SD Card.

    pinMode (   MISO_PIN,   INPUT_PULLDOWN);    // SD D0, Allow pin to Pulldown Low (for reliable Flashing).
    pinMode (   MOSI_PIN,   INPUT_PULLUP);      // SD CMD, Allow pin to Pullup High (for reliable Flashing).
    pinMode (   SD_CLK_PIN, INPUT_PULLUP);      // SD CLK.
}

// *********************************************************************************************
// strIsUint(): Check if string is unsigned integer. Return true if uint.
bool strIsUint (String intStr)
{
    uint8_t i;

    if (intStr.length () == 0)
    {
        return false;
    }

    for (i = 0;i < intStr.length ();i++)    // Arg must be integer >= 0.
    {
        char c = intStr.charAt (i);

        if ((c == '-') || (c < '0') || (c > '9'))
        {
            return false;
        }
    }

    return true;
}
