/*
  *    File: fileSys.cpp
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
  *
  *    Notes:
  *    SPIFFS is depreciated and has been replaced by LittleFS.
  *
  *    Data Files:
  *    All littleFS data files must be in the PixelRadio/data folder.
  *
  *    Here's some advice on how to get all the ESPUI data files to fit in min_spiffs flash space.
  *    ESPUI has 50KB of unneeded files; Delete the following temp files (if present) from the /project data folder:
  *     data/index.min.htm
  *     data/css/normalize.min.css
  *     data/css/style.min.css
  *     data/js/controls.min.js
  *     data/js/graph.min.js
  *     data/js/slider.min.js
  *     data/js/tabbedcontent.min.js
  *     NOTE: Do NOT delete /data/js/zepto.min.js
  *
  *    Base64 GIF Data Files:
  *    GIF Files can be converted to base64 using this tool: https://codebeautify.org/gif-to-base64-converter
  *
  *    How to get data files (Filesystem Image) onto ESP32 during ESP32 Flash:
  *     1. Use tne IDE's Platform->Upload_Filesystem_Image to upload the data directory files.
  *     2. Or use powershell command To serial upload the LittleFS data directory: platformio run --target uploadfs
  *     3. Or for OTA upload use: platformio run --target uploadfs --upload-port <IP_ADDR>
  *        Note: Replace <IP_ADDR> with board's IP (example: 192.168.1.7).
  *     4. Or to copy the Logo Image File to File System use SD Card method. See Github readme.
  *
  */

// *********************************************************************************************

#include <Arduino.h>
#include <ArduinoLog.h>
#include <LittleFS.h>
#include <SD.h>
#include <SPI.h>

#include "config.h"
#include "PixelRadio.h"

#define PixelRadio_LittleFS LittleFS


// *********************************************************************************************
// instalLogoImageFile(): Check to see if a logo image file needs to be copied to the ESP32
// file system. Use SD Card as file source.
// Note: This routine can be used anytime AFTER littlefsInit(); Typically called in startup().
//
void instalLogoImageFile (void)
{
    char logBuff[60 + sizeof (LOGO_GIF_NAME)];
    int16_t sdcFileSize;
    int16_t lfsFileSize;
    SPIClass SPI2 (HSPI);

    if (PixelRadio_LittleFS.exists (LOGO_GIF_NAME))
    {
        sprintf (logBuff, String ( F ("Found \"%s\" Logo Image in File System.")).c_str(), LOGO_GIF_NAME);
        Log.verboseln (logBuff);

        return;
    }

    sprintf (logBuff, String ( F ("Logo Gif File (%s) is Missing. Will Load it From the SD Card.")).c_str(), LOGO_GIF_NAME);
    Log.errorln (logBuff);

    SPI2.begin (SD_CLK_PIN, MISO_PIN, MOSI_PIN, SD_CS_PIN);
    pinMode (MISO_PIN, INPUT_PULLUP);   // MISO requires internal pull-up.
    SD.end ();                          // Reset interface (in case SD card had been swapped).

    if (!SD.begin (SD_CS_PIN, SPI2))
    {
        SD.end ();
        spiSdCardShutDown ();

        sprintf (logBuff, String ( F ("-> SD Card Not Installed. Cannot Load Missing Logo Gif File.")).c_str());
        Log.errorln (logBuff);

        return;  // No SD Card, nothing to do, exit.
    }

    File sdcImageFile;  // SD Card Image File.

    sdcImageFile    = SD.open (LOGO_GIF_NAME, FILE_READ);
    sdcFileSize     = sdcImageFile.size ();

    if (sdcImageFile && (sdcFileSize > 0))
    {
        sprintf (logBuff, String ( F ("-> Installing \"%s\" Logo Gif File From SD Card ...")).c_str(), LOGO_GIF_NAME);
        Log.infoln (logBuff);
        sprintf (logBuff, String ( F ("-> Copying Gif File, Size: %d")).c_str(), sdcImageFile.size ());
    }
    else
    {
        // lfsImageFile.close();
        sdcImageFile.close ();
        SD.end ();
        spiSdCardShutDown ();
        sprintf (logBuff, String ( F ("-> Bad/Missing SD Card Logo File.")).c_str());
        Log.errorln (logBuff);

        return;
    }

    sprintf (logBuff, String ( F ("-> Copying Logo File From SD Card (%d bytes).")).c_str(), sdcFileSize);
    Log.infoln (logBuff);

    // lfsImageFile.close();
    File lfsImageFile;  // LittleFS Image File.

    lfsImageFile = PixelRadio_LittleFS.open (LOGO_GIF_NAME, FILE_WRITE);

    while (sdcImageFile.available ())
    {
        char data = sdcImageFile.read ();
        lfsImageFile.print (data);
    }

    lfsImageFile.close ();
    lfsImageFile    = PixelRadio_LittleFS.open (LOGO_GIF_NAME, FILE_READ);
    lfsFileSize     = lfsImageFile.size ();

    /*  // DEBUG ONLY
      *     Serial.println("START==>");
      *     while (lfsImageFile.available()) {
      *         Serial.write(lfsImageFile.read()); // Send raw data to serial port.
      *     }
      *     Serial.println("<==END");
      */
    sprintf (logBuff, String ( F ("-> Success, Saved Logo Gif to File System (%d bytes).")).c_str(), lfsFileSize);
    Log.infoln (logBuff);

    if (sdcImageFile != lfsImageFile)
    {
        sprintf (logBuff, String ( F ("-> Copied File Does NOT match Source Size (diff= %d bytes).")).c_str(), sdcImageFile - lfsImageFile);
        Log.errorln (logBuff);
    }

    lfsImageFile.close ();
    sdcImageFile.close ();
    SD.end ();
    spiSdCardShutDown ();

    littlefsInit ();
}

// *********************************************************************************************
// littlefsInit(): Initialize LittleFS file system.
void littlefsInit (void)
{
    char logBuff[100];

    unsigned long   filesz      = 0;
    const char      * content1  = "  This text was written to LittleFS because the Filesystem is missing\r\n";
    const char      * content2  = "  >> If you see this message then please Upload the Filesystem Image <<";

    if (!PixelRadio_LittleFS.begin (true))  // true=Format on fail.
    {
        Log.errorln (String ( F ("LittleFS: An Error has occurred while mounting File System")).c_str());
    }
    else
    {
        Log.infoln (String ( F ("LittleFS: Mounted File System, testing ...")).c_str());

        sprintf (logBuff, String ( F ("-> Total Size: %u bytes")).c_str(), PixelRadio_LittleFS.totalBytes ());
        Log.verboseln (logBuff);
        sprintf (logBuff, String ( F ("-> Total Used: %u bytes")).c_str(), PixelRadio_LittleFS.usedBytes ());
        Log.verboseln (logBuff);
        sprintf (logBuff, String ( F ("-> Total Free: %u bytes")).c_str(), PixelRadio_LittleFS.totalBytes () - PixelRadio_LittleFS.usedBytes ());
        Log.verboseln (logBuff);

        File file1 = PixelRadio_LittleFS.open (F("/test.txt"), FILE_READ);

        if (!file1)
        {
            filesz = 0;
            Log.errorln (String ( F ("-> Error opening test.txt file for reading")).c_str());
        }
        else
        {
            filesz = file1.size ();
            Log.verboseln (String ( F ("-> Successfully Opened test.txt file for reading, Size: %u")).c_str(), filesz);
            file1.close ();
        }

        if (filesz == 0)
        {
            file1 = PixelRadio_LittleFS.open (F("/test.txt"), FILE_WRITE);

            if (!file1)
            {
                Log.errorln (String ( F ("-> There was an error opening the test.txt file for writing")).c_str());
            }
            else
            {
                Log.warningln (String ( F ("-> The test.txt file is missing, will create it")).c_str());

                // Serial.println("  Content to write =");
                // Serial.print(content1);
                // Serial.println(content2);

                if (file1.print (content1))
                {
                    file1.print (content2);
                    Log.infoln (String ( F ("-> File was successfully created, written, and closed")).c_str());
                }
                else
                {
                    Log.errorln (String ( F ("-> File write failed")).c_str());
                }

                file1.close ();
            }
        }
        else
        {
            Log.verboseln (F ("-> test.txt file successfully opened, now closed"));
            file1.close ();
        }

        // Let's read the test.txt file from PixelRadio_LittleFS.
        // Data files like this one should be uploaded to the Filesystem Image, see comment section at top of file.
        File file2 = PixelRadio_LittleFS.open ("/test.txt", FILE_READ);

        if (!file2)
        {
            Log.errorln (F ("-> Failed to open file test.txt for reading"));
        }
        else
        {
            sprintf (logBuff, String (F ("-> Reopened test.txt, File size = %u bytes")).c_str(), file2.size ());
            Log.verboseln (logBuff);

            /*
              *             Serial.println(" Contents of test.txt file = ");
              *             while (file2.available())
              *             {
              *                 Serial.write(file2.read()); // Send raw data to serial port.
              *             }
              *             Serial.println();
              */
            file2.close ();
        }

        Log.infoln (F ("-> File System Testing Complete."));
    }
}

// ============================================================================================================================
// makeWebGif(): Get base64 GIF from file system (LittleFS) and save in String encoded for HTML.
const String makeWebGif (String fileName, uint16_t width, uint16_t height, String backGroundColorStr)
{
    uint16_t fileSz = 0;
    char logBuff[90];
    String imageStr;

    imageStr.clear ();
    File imageFile = PixelRadio_LittleFS.open (fileName, FILE_READ);

    if (!imageFile)
    {
        sprintf (logBuff, String ( F ("-> makeWebGif: Error opening Image file (%s)")).c_str(), fileName.c_str ());
        Log.warningln ( logBuff);

        imageFile = PixelRadio_LittleFS.open ("/", FILE_READ);
        if (!imageFile)
        {
            Log.warningln ( F ("-> LittleFS Filesystem is Missing, YOU Need to Upload it."));
        }

        imageStr.clear ();
        imageStr.reserve (13);
        imageStr = F ("IMAGE FILE MISSING");

        return imageStr;
    }
    else
    {
        fileSz = imageFile.size ();

        if (fileSz > 4095)
        {
            sprintf (logBuff, String ( F ("-> makeWebGif: File %s is too large (%u bytes).")).c_str(), fileName.c_str (), fileSz);
            Log.infoln (logBuff);
            imageStr.clear ();
            imageStr.reserve (15);
            imageStr = F ("FILE TOO LARGE");
            imageFile.close ();

            return imageStr;
        }

        imageStr.reserve (fileSz + 200);
        imageStr    = F ("<p style=");
        imageStr    += F ("\"background-color:");
        imageStr    += backGroundColorStr;
        imageStr    += F (";margin-bottom:-3px;margin-top:-2px;margin-left:-6px;margin-right:-6px;\">");
        imageStr    += F ("<img src=\'data:image/gif;base64,");
        imageStr    += imageFile.readString ();
        imageStr    += F ("\'; width=\"");
        imageStr    += String (width);
        imageStr    += F ("\" height=\"");
        imageStr    += String (height);
        imageStr    += F ("\" alt=\"");
        imageStr    += fileName;
        imageStr    += F ("\"/></p>");

        imageFile.close ();
        sprintf (logBuff, String ( F ("-> Image File %s Successfully Loaded (%u bytes).")).c_str(), fileName.c_str (), fileSz);
        Log.infoln (logBuff);

        // Serial.println("\r\n makeWebGif File Contents: BEGIN->[ " + imageStr + " ]<-END\r\n"); // DEBUG ONLY
    }

    return imageStr;
}

// *********************************************************************************************
// EOF
