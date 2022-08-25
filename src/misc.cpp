/*
   File: misc.cpp
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
#include "globals.h"
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
// *********************************************************************************************
uint8_t i2cScanner (void)
{
#ifdef OldWay
        byte count = 0;
        String devStr;

        Log.infoln (String (F ("Scanning i2c for Devices ...")).c_str ());

        for (byte addr = 0x01; addr < 0x7f; addr++)
        {
            Wire.beginTransmission (addr);          // Begin I2C transmission Address (i)

            if (Wire.endTransmission (true) == 0)   // Receive 0 = success (ACK response)
            {
                switch (addr)
                {
                    case QN8027_I2C_ADDR:
                    {
                        devStr = "FM Tx QN8027";
                        break;
                    }

                    default:
                    {
                        devStr = "Unknown";
                    }
                }   // switch

                Log.infoln (String (F ("-> Found i2c address: 0x%02X (%s)")).c_str (), addr, devStr.c_str ());
                count++;
            }
        }

        if (count == 0)
        {
            Log.errorln (String (F ("-> No i2c Devices Found.")).c_str ());
        }
        else
        {
            Log.infoln (String (F ("-> Scanning Complete, Found %u i2c Devices.")).c_str (), count);
        }
        return count;

#endif // def OldWay

    return 0;
}

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
// rebootSystem(): Reboot the System if global rebootFlg = true.
//                 Function must be in main loop. But can also be used in a function.
//                 Do not directly call this function in a ESPUI callback. Use main loop instead.
void rebootSystem (void)
{
    String tmpStr;

    if (rebootFlg)
    {
        tmpStr  = "USER REBOOT!";
        tmpStr  += "\r\n\r\n";
        tmpStr  += "          ******************\r\n";
        tmpStr  += "          * SYSTEM REBOOT! *\r\n";
        tmpStr  += "          ******************\r\n\r\n";
        Serial.println (tmpStr.c_str ());   // MUST use default serial print, not serial Log.

        Serial.flush ();
        delay (1000);
        ESP.restart ();
    }
}

// *********************************************************************************************
// setGpioBootPins(): Set the GPIO Pin Boot States using Web UI settings.
void setGpioBootPins (void)
{
    if (gpio19BootStr == GPIO_OUT_LO_STR)
    {
        pinMode (GPIO19_PIN, OUTPUT);
        digitalWrite (GPIO19_PIN, LOW);
    }
    else if (gpio19BootStr == GPIO_OUT_HI_STR)
    {
        pinMode (GPIO19_PIN, OUTPUT);
        digitalWrite (GPIO19_PIN, HIGH);
    }
    else if (gpio19BootStr == GPIO_INP_FT_STR)
    {
        pinMode (GPIO19_PIN, INPUT);
    }
    else if (gpio19BootStr == GPIO_INP_PU_STR)
    {
        pinMode (GPIO19_PIN, INPUT_PULLUP);
    }
    else if (gpio19BootStr == GPIO_INP_PD_STR)
    {
        pinMode (GPIO19_PIN, INPUT_PULLDOWN);
    }
    else
    {
        Log.errorln (String (F ("-> setGpioBootPins: GPIO-19 Has Invalid Value.")).c_str ());
    }

    if (gpio23BootStr == GPIO_OUT_LO_STR)
    {
        pinMode (GPIO23_PIN, OUTPUT);
        digitalWrite (GPIO23_PIN, LOW);
    }
    else if (gpio23BootStr == GPIO_OUT_HI_STR)
    {
        pinMode (GPIO23_PIN, OUTPUT);
        digitalWrite (GPIO23_PIN, HIGH);
    }
    else if (gpio23BootStr == GPIO_INP_FT_STR)
    {
        pinMode (GPIO23_PIN, INPUT);
    }
    else if (gpio23BootStr == GPIO_INP_PU_STR)
    {
        pinMode (GPIO23_PIN, INPUT_PULLUP);
    }
    else if (gpio23BootStr == GPIO_INP_PD_STR)
    {
        pinMode (GPIO23_PIN, INPUT_PULLDOWN);
    }
    else
    {
        Log.errorln (String (F ("-> setGpioBootPins: GPIO-23 Has Invalid Value.")).c_str ());
    }

    if (gpio33BootStr == GPIO_OUT_LO_STR)
    {
        pinMode (GPIO33_PIN, OUTPUT);
        digitalWrite (GPIO33_PIN, LOW);
    }
    else if (gpio33BootStr == GPIO_OUT_HI_STR)
    {
        pinMode (GPIO33_PIN, OUTPUT);
        digitalWrite (GPIO33_PIN, HIGH);
    }
    else if (gpio33BootStr == GPIO_INP_FT_STR)
    {
        pinMode (GPIO33_PIN, INPUT);
    }
    else if (gpio33BootStr == GPIO_INP_PU_STR)
    {
        pinMode (GPIO33_PIN, INPUT_PULLUP);
    }
    else if (gpio33BootStr == GPIO_INP_PD_STR)
    {
        pinMode (GPIO33_PIN, INPUT_PULLDOWN);
    }
    else
    {
        Log.errorln (String (F ("-> setGpioBootPins: GPIO-33 Has Invalid Value.")).c_str ());
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

    for (i = 0; i < intStr.length (); i++)  // Arg must be integer >= 0.
    {
        char c = intStr.charAt (i);

        if ((c == '-') || (c < '0') || (c > '9'))
        {
            return false;
        }
    }

    return true;
}

// *********************************************************************************************
// updateGpioBootPins(): Update the GPIO Boot Pins if the Web UI has changed GPIO settings.
void updateGpioBootPins (void)
{
    if (newGpio19Flg || newGpio23Flg || newGpio33Flg)
    {
        newGpio19Flg    = false;
        newGpio23Flg    = false;
        newGpio33Flg    = false;
        setGpioBootPins ();
    }
}
