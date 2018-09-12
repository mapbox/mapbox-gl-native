@ECHO OFF
SETLOCAL
SET EL=0

7z > NUL
IF %ERRORLEVEL% NEQ 0 ECHO 7z not availabe && GOTO ERROR

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

SET VS_REGVAL=15.0
REM 64bit:
SET VS_REGKEY=HKLM\Software\WOW6432Node\Microsoft\VisualStudio\SxS\VS7
REM 32bit SET VS_REGKEY=HKLM\Software\Microsoft\VisualStudio\SxS\VS7

reg query %VS_REGKEY% /v %VS_REGVAL% 2>nul || (echo No vs2017 registry key present! && SET ERRORLEVEL=1 && GOTO ERROR)

set VS_DIR=
for /f "tokens=2,*" %%a in ('reg query %VS_REGKEY% /v %VS_REGVAL% ^| findstr %VS_REGVAL%') do (
	echo found VS %%b
	set VS_DIR=%%b
)

if not defined VS_DIR (echo Cannot find Visual Studio 2017 directory! && SET ERRORLEVEL=1 && GOTO ERROR)

ECHO using %VS_DIR%

SET VSCMD=%VS_DIR%\VC\Auxiliary\Build\vcvarsall.bat
CALL "%VSCMD%" amd64
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

SET PATH=C:\Program Files\LLVM\bin;%PATH%
SET CC=clang-cl
SET CXX=clang-cl

REM -T v141_clang_c2 ^
REM -T llvm ^
SET BUILDTOOLSET=llvm
SET TOOLS_VERSION=15.0
SET BUILDPLATFORM=x64

cmake -G "Visual Studio 15 2017 Win64" ^
-T %BUILDTOOLSET% ^
-DCMAKE_BUILD_TYPE=%configuration% ^
-DMBGL_PLATFORM=qt ^
-DWITH_QT_DECODERS=ON ^
-DWITH_QT_I18N=ON ^
-DWITH_NODEJS=OFF ^
-DCMAKE_PREFIX_PATH=C:\Qt\5.11.1\msvc2017_64\lib\cmake  ..
IF %ERRORLEVEL% NEQ 0 GOTO ERROR

REM cmake --build . -- -j %NUMBER_OF_PROCESSORS%
REM IF %ERRORLEVEL% NEQ 0 GOTO ERROR


SET MSBUILD_PARALLEL=/maxcpucount:1
IF %NUMBER_OF_PROCESSORS% GEQ 4 SET MSBUILD_PARALLEL=/p:BuildInParallel=true /maxcpucount:2 /p:CL_MPCount=%NUMBER_OF_PROCESSORS%
IF %NUMBER_OF_PROCESSORS% GEQ 8 SET MSBUILD_PARALLEL=/p:BuildInParallel=true /maxcpucount:3 /p:CL_MPCount=%NUMBER_OF_PROCESSORS%
:: since any CL.EXE /MP is able to use all available processors (provided it is given enough
:: sources to compile), using multiple MSBUILD workers only makes sense when you have more
:: processors than sources per directory (because sources from different directories yield
:: different /Fo paths, a separate CL.EXE /MP master process must be run for each directory)
IF DEFINED APPVEYOR SET MSBUILD_PARALLEL=/maxcpucount:1
REM IF NOT DEFINED APPVEYOR SET CL=/MP:%NUMBER_OF_PROCESSORS% /cgthreads8
REM IF NOT DEFINED APPVEYOR SET LINK=/CGTHREADS:8 /time+

msbuild ^
mbgl.sln ^
/nologo ^
/toolsversion:%TOOLS_VERSION% ^
/p:Configuration=%configuration% ^
/p:Platform=%BUILDPLATFORM% ^
/p:StopOnFirstFailure=true %MSBUILD_PARALLEL%

IF %ERRORLEVEL% NEQ 0 GOTO ERROR

GOTO DONE

:ERROR
SET EL=%ERRORLEVEL%
ECHO ~~~~~~~~~~~~~~~~~~~ ERROR %~f0 ~~~~~~~~~~~~~~~~~~~
ECHO ERRORLEVEL^: %EL%

:DONE

ECHO ~~~~~~~~~~~~~~~~~~~ DONE %~f0 ~~~~~~~~~~~~~~~~~~~

EXIT /b %EL%
