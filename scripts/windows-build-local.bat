@ECHO OFF
SETLOCAL
SET EL=0

REM this file is supposed to be called from the root of the repository
REM otherwise path references will not work

IF NOT EXIST C:\Qt\5.11.1\msvc2017_64\lib SET EL=1 && ECHO QT not found && GOTO ERROR
SET PATH=C:\Program Files\7-Zip;%PATH%

REM set env vars normally set by AppVeyor
SET configuration=Release
SET APPVEYOR_BUILD_FOLDER=%CD%
SET CMAKE_VERSION=cmake-3.10.1-win64-x64
SET CMAKE_URL=https://cmake.org/files/v3.10/%CMAKE_VERSION%.zip

CALL scripts\windows-build-appveyor.bat
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

GOTO DONE

:ERROR
SET EL=%ERRORLEVEL%
ECHO ~~~~~~~~~~~~~~~~~~~ ERROR %~f0 ~~~~~~~~~~~~~~~~~~~
ECHO ERRORLEVEL^: %EL%

:DONE

ECHO ~~~~~~~~~~~~~~~~~~~ DONE %~f0 ~~~~~~~~~~~~~~~~~~~

EXIT /b %EL%
