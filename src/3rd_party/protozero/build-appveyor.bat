@ECHO OFF
SETLOCAL
SET EL=0

ECHO =========== %~f0 ===========

ECHO platform^: %platform%
ECHO configuration^: %configuration%

SET MSBUILD_VERBOSITY=normal
IF NOT "%1"=="" SET MSBUILD_VERBOSITY=%1
ECHO MSBUILD_VERBOSITY^: %MSBUILD_VERBOSITY%

SET PATH=C:\Program Files\7-Zip;%PATH%
SET PATH=c:\python27;%PATH%
SET PATH="C:\Program Files (x86)\MSBuild\14.0\bin";%PATH%
SET PATH=%~dp0deps\protobuf;%PATH%

REM add unix style "find" to front of PATH
REM used to glob files in gyp
IF DEFINED APPVEYOR SET PATH=C:\Program Files\Git\usr\bin;%PATH%
IF NOT DEFINED APPVEYOR SET PATH=%GIT_INSTALL_ROOT%\bin;%PATH%
WHERE find

IF EXIST %configuration% ECHO deleting %configuration% && RD /Q /S %configuration%
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

IF EXIST protozero.sln DEL protozero.sln
IF EXIST protozero.sdf DEL protozero.sdf
DEL *.vcxproj
DEL *.vcxproj.filters

IF exist deps\gyp (ECHO gyp already cloned) ELSE (git clone --quiet --depth 1 https://chromium.googlesource.com/external/gyp.git deps/gyp)
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

SET protobuf_sdk=protozero-dep-protobuf-2.6.1.7z
IF EXIST %protobuf_sdk% (ECHO protobuf already downloaded) ELSE (ECHO downloading protobuf ... && powershell Invoke-WebRequest https://mapbox.s3.amazonaws.com/windows-builds/windows-build-deps/$env:protobuf_sdk -OutFile $pwd\$env:protobuf_sdk)
IF %ERRORLEVEL% NEQ 0 GOTO ERROR
IF EXIST deps\protobuf (ECHO protobuf already extracted) ELSE (CALL 7z x -y %protobuf_sdk% | %windir%\system32\FIND "ing archive")
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

FOR /F %%x in ('find test/ -name "testcase.proto"') DO "deps\protobuf\%platform%\%configuration%\protoc" --cpp_out=. %%x
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

REM note windows requires --generator-output to be absolute
python deps/gyp/gyp_main.py gyp/protozero.gyp --depth=. -f msvs -G msvs_version=2015
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

SET MSBUILD_PLATFORM=%platform%
IF /I "%MSBUILD_PLATFORM%" == "x86" set MSBUILD_PLATFORM=Win32

msbuild gyp/protozero.sln ^
/nologo ^
/maxcpucount:%NUMBER_OF_PROCESSORS% ^
/p:BuildInParellel=true ^
/p:Configuration=%configuration%;Platform=%MSBUILD_PLATFORM% ^
/verbosity:%MSBUILD_VERBOSITY% ^
/consoleloggerparameters:Summary
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

ECHO running gyp\%configuration%\%platform%\tests.exe ...
gyp\%configuration%\%platform%\tests.exe
:: IF %ERRORLEVEL% NEQ 0 GOTO ERROR

ECHO running gyp\%configuration%\%platform%\writer_tests.exe ...
gyp\%configuration%\%platform%\writer_tests.exe
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

GOTO DONE

:ERROR
ECHO ~~~~~~~~~ ERROR  %~f0 ~~~~~~~~~~~
SET EL=%ERRORLEVEL%
ECHO ERRORLEVEL^: %EL%

:DONE

ECHO DONE %~f0 %platform% %configuration%
EXIT /B %EL%
