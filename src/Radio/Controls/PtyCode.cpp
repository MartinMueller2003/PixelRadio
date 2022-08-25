/*
   File: PtyCode.cpp
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

#include "PtyCode.hpp"
#include "PreEmphasis.hpp"
#include "QN8027RadioApi.hpp"
#include "RfCarrier.hpp"
#include "memdebug.h"

static std::vector <std::pair <String, String>> ListOfUsPtyCodes =
{
    {"No programme type defined", "0"                      },
    {"News",                      "1"                      },
    {"Information",               "2"                      },
    {"Sport",                     "3"                      },
    {"Talk",                      "4"                      },
    {"Rock",                      "5"                      },
    {"Classic Rock",              "6"                      },
    {"Adult Hits",                "7"                      },
    {"Soft Rock",                 "8"                      },
    {"Top 40",                    "9"                      },
    {"Country Music",             "10"                     },
    {"Oldies (Music)",            "11"                     },
    {"Soft Music",                "12"                     },
    {"Nostalgia",                 "13"                     },
    {"Jazz",                      "14"                     },
    {"Classical",                 "15"                     },
    {"Rhythm & Blues",            "16"                     },
    {"Soft Rhythm & Blues",       "17"                     },
    {"Language",                  "18"                     },
    {"Religious Music",           "19"                     },
    {"Religious Talk",            "20"                     },
    {"Personality",               "21"                     },
    {"Public",                    "22"                     },
    {"College",                   "23"                     },
    {"Not assigned 1",            "24"                     },
    {"Not assigned 2",            "25"                     },
    {"Not assigned 3",            "26"                     },
    {"Not assigned 4",            "27"                     },
    {"Not assigned 5",            "28"                     },
    {"Weather",                   "29"                     },
    {"Emergency Test",            "30"                     },
    {"Emergency",                 "31"                     },
};

static std::vector <std::pair <String, String>> ListOfEuPtyCodes =
{
    {"No programme type defined", "0"                     },
    {"News",                      "1"                     },
    {"Current affairs",           "2"                     },
    {"Information",               "3"                     },
    {"Sport",                     "4"                     },
    {"Education",                 "5"                     },
    {"Drama",                     "6"                     },
    {"Culture",                   "7"                     },
    {"Soft Science",              "8"                     },
    {"Varied",                    "9"                     },
    {"Popular Music (Pop)",       "10"                    },
    {"Rock Music",                "11"                    },
    {"Easy Listening",            "12"                    },
    {"Light Classical",           "13"                    },
    {"Serious Classical",         "14"                    },
    {"Other Music",               "15"                    },
    {"Weather",                   "16"                    },
    {"Finance",                   "17"                    },
    {"Children's Programmes",     "18"                    },
    {"Social Affairs",            "19"                    },
    {"Religion",                  "20"                    },
    {"Phone-in",                  "21"                    },
    {"Travel",                    "22"                    },
    {"Leisure",                   "23"                    },
    {"Jazz Music",                "24"                    },
    {"Country Music",             "25"                    },
    {"National Music",            "26"                    },
    {"Oldies Music",              "27"                    },
    {"Folk Music",                "28"                    },
    {"Documentary",               "29"                    },
    {"Alarm Test",                "30"                    },
    {"Alarm",                     "31"                    },
};

static const PROGMEM char     RDS_PTY_CODE_STR        [] = "PTY CODE<br>Station Genre";
static const PROGMEM char     RDS_PTY_CODE            [] = "RDS_PTY_CODE";

// *********************************************************************************************
cPtyCode::cPtyCode () :   cChoiceListControl (RDS_PTY_CODE, RDS_PTY_CODE_STR, "Top 40", &ListOfUsPtyCodes)
{
    // _ DEBUG_START;
    // _ DEBUG_END;
}

// *********************************************************************************************
void cPtyCode::AddControls (uint16_t value, ControlColor color)
{
    // DEBUG_START;

    cChoiceListControl::AddControls (value, color);
    setPtyCodeOptionValues ();

    // DEBUG_END;
}

// ************************************************************************************************
bool cPtyCode::set (const String & value, String & ResponseMessage, bool ForceUpdate)
{
    // DEBUG_START;

    // DEBUG_V (       String ("          value: '") + value + "'");
    // DEBUG_V (       String ("   DataValueStr: '") + DataValueStr + "'");
    // DEBUG_V (       String ("CurrentRegion 9: '") + (*ChoiceVector)[9].first + "'");

    bool Response = cChoiceListControl::set (value, ResponseMessage, ForceUpdate);
    // DEBUG_V (       String ("          get32: ") + String (get32 ()));

    // DEBUG_V (       String ("CurrentRegion 9: '") + (*ChoiceVector)[9].first + "'");

    if (Response || ForceUpdate)
    {
        QN8027RadioApi.setPtyCode (get32 (), RfCarrier.get ());
    }
    // DEBUG_V (       String ("   DataValueStr: ") + DataValueStr);
    // DEBUG_V (       String ("          get32: ") + String (get32 ()));
    // DEBUG_V (       String ("ResponseMessage: ") + ResponseMessage);
    // DEBUG_V (       String ("       Response: ") + String (Response));

    // DEBUG_END;
    return Response;
}

// ************************************************************************************************
void cPtyCode::setPtyCodeOptionValues ()
{
    // DEBUG_START;

    // DEBUG_V (       String ("       DataValueStr: ") + String (DataValueStr));
    // DEBUG_V (       String ("PreEmphasis.get32(): ") + String (PreEmphasis.get32 ()));
    // DEBUG_V (       String ("    CurrentRegion 9: '") + (*ChoiceVector)[9].first + "'");

    // DEBUG_V (       String ("    US 9: '") + ListOfUsPtyCodes[9].first + "'");
    // DEBUG_V (       String ("    EU 9: '") + ListOfEuPtyCodes[9].first + "'");

    if (0 == PreEmphasis.get32 ())
    {
        // DEBUG_V ("US");
        RefreshOptionList (&ListOfUsPtyCodes);
    }
    else
    {
        // DEBUG_V ("EU");
        RefreshOptionList (&ListOfEuPtyCodes);
    }
    // DEBUG_V (String ("    CurrentRegion 9: '") + (*ChoiceVector)[9].first + "'");

    // DEBUG_END;
}

// *********************************************************************************************
cPtyCode PtyCode;

// *********************************************************************************************
// OEF
