@echo off

set NULL_VAL=null
set NODE_VER=%NULL_VAL%
set NODE_EXEC=node-v10.15.3-x86.msi

node -v >.tmp_nodever
set /p NODE_VER=<.tmp_nodever
del .tmp_nodever

IF "%NODE_VER%"=="%NULL_VAL%" (
    NET SESSION >nul 2>&1
    IF %ERRORLEVEL% NEQ 0 (
        echo This setup needs admin permissions. Please run this file as admin.
        pause
        exit
    )

	echo.
	echo Node.js is not installed! Please press a key to download and install it from the website that will open.
	PAUSE
	start "" http://nodejs.org/dist/v10.15.3/%NODE_EXEC%
	echo.
	echo.
	echo After you have installed Node.js, press a key to shut down this process. Please restart it again afterwards.
    call npm install
	PAUSE
	EXIT
) ELSE (
	echo A version of Node.js ^(%NODE_VER%^) is installed. Proceeding...
)

set GULP_PATH=%NULL_VAL%
where gulp > .tempGulpPath
set /p GULP_PATH=<.tempGulpPath
del .tempGulpPath

IF "%GULP_PATH%"=="%NULL_VAL%" (
    NET SESSION >nul 2>&1
    IF %ERRORLEVEL% NEQ 0 (
        echo This setup needs admin permissions. Please run this file as admin.
        pause
        exit
    )
	echo.
	echo Install gulp

    call npm install
    call npm install --global
    call npm audit fix

) ELSE (
    echo gulp is installed. Proceeding...
)

echo execute gulp
gulp
pause