#!/usr/bin/env python3

import os
import filecmp
import shutil

Import("env")

f = open("./MyEnv.txt", "w")
f.write(env.Dump())
f.close()

PROJECT_DIR = env['PROJECT_DIR']
PIOENV      = env['PIOENV']
BOARD       = env['BOARD']
PROGNAME    = env['PROGNAME']
BOARD_MCU   = env['BOARD_MCU']
SRC_DIR     = PROJECT_DIR + "/src"

# print("PROJECT_DIR = " + PROJECT_DIR)
# print("PIOENV = " + PIOENV)
# print("BOARD = " + BOARD)
# print("PROGNAME = " + PROGNAME)
# print("BOARD_MCU = " + BOARD_MCU)
# print("SRC_DIR = " + SRC_DIR)

ExtensionsOfInterest = ('.h', '.hpp', '.c', '.cpp')

def ProcessDirectory(DirPath):
    # print("Display Files for: '" + DirPath + "'")
    for file in os.scandir(DirPath):
        if file.is_dir():
            ProcessDirectory(file.path)
        elif file.is_file():
            if file.path.endswith(ExtensionsOfInterest):
                SRC_FILE_NAME = file.path
                DST_FILE_NAME = SRC_FILE_NAME + ".txt"
                CRT_FILE_NAME = SRC_FILE_NAME + ".uncrustify"
                COMMAND = PROJECT_DIR + "/uncrustify.exe -c " + PROJECT_DIR + \
                    "/uncrustify.cfg -f " + SRC_FILE_NAME + " > " + DST_FILE_NAME
                # print("SRC_FILE_NAME : '" + SRC_FILE_NAME + "'")
                # print("DST_FILE_NAME : '" + DST_FILE_NAME + "'")
                # print("COMMAND: '" + COMMAND + "'")
                os.system(COMMAND)
                result = filecmp.cmp(SRC_FILE_NAME, DST_FILE_NAME, shallow=False)
                # print(result)
                if(False == result):
                    # print("Copy File: " + DST_FILE_NAME)
                    shutil.copyfile(DST_FILE_NAME, SRC_FILE_NAME)
                if os.path.exists(DST_FILE_NAME):
                    os.remove(DST_FILE_NAME)
                if os.path.exists(CRT_FILE_NAME):
                    os.remove(CRT_FILE_NAME)

ProcessDirectory(SRC_DIR)
