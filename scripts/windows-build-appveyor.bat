@ECHO OFF
SETLOCAL
SET EL=0

ECHO Powershell version^:
powershell $PSVersionTable.PSVersion
IF %ERRORLEVEL% NEQ 0 ECHO powershell not availabe && GOTO ERROR

FOR /F "tokens=*" %%i in ('powershell Get-ExecutionPolicy') do SET PSPOLICY=%%i
ECHO Powershell execution policy^: %PSPOLICY%
IF NOT "%PSPOLICY%"=="Unrestricted" powershell Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Unrestricted -Force
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
FOR /F "tokens=*" %%i in ('powershell Get-ExecutionPolicy') do SET PSPOLICY=%%i
ECHO Powershell execution policy now is^: %PSPOLICY%

IF NOT EXIST cmake.zip powershell Invoke-WebRequest $env:CMAKE_URL -OutFile $env:APPVEYOR_BUILD_FOLDER\\cmake.zip
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

IF NOT EXIST C:\%CMAKE_VERSION% 7z -y x cmake.zip -oC:\ | %windir%\system32\FIND "ing archive"
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

SET PATH=C:\%CMAKE_VERSION%\bin;%PATH%

IF EXIST %APPVEYOR_BUILD_FOLDER%\build RMDIR /Q /S %APPVEYOR_BUILD_FOLDER%\build
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
MKDIR %APPVEYOR_BUILD_FOLDER%\build
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
CD %APPVEYOR_BUILD_FOLDER%\build
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

REM call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
call "C:\Program Files (x86)\Microsoft Visual Studio\Preview\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

cmake -G "Visual Studio 15 Win64" ^
-DMBGL_PLATFORM=qt ^
-DWITH_QT_DECODERS=ON ^
-DWITH_QT_I18N=ON ^
-DWITH_NODEJS=OFF ^
-DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_PREFIX_PATH=C:\Qt\5.11.1\msvc2017_64\lib\cmake ^
-DCMAKE_MAKE_PROGRAM="%APPVEYOR_BUILD_FOLDER%\platform\qt\ninja.exe" ..
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

REM cmake --build . -- -j %NUMBER_OF_PROCESSORS%
REM IF %ERRORLEVEL% NEQ 0 GOTO ERROR

GOTO DONE

:ERROR
SET EL=%ERRORLEVEL%
ECHO ~~~~~~~~~~~~~~~~~~~ ERROR %~f0 ~~~~~~~~~~~~~~~~~~~
ECHO ERRORLEVEL^: %EL%

:DONE

ECHO ~~~~~~~~~~~~~~~~~~~ DONE %~f0 ~~~~~~~~~~~~~~~~~~~

EXIT /b %EL%
