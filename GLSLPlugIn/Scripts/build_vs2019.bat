@echo off

rem --- Define script directory ---
set SCRIPT_DIRECTORY=%~dp0
cd %SCRIPT_DIRECTORY%

rem --- Set variables for MSVC ---
set PROJECT_NAME=GLSLPlugIn
set EXPORTER_NAME=VisualStudio2019
set MSVC_VERSION=2019
set MSVC_OFFERING=Community
set ARCHITECTURE=x64
set BUILD_CONFIG=Debug

rem --- Set variables for enabling each plugin format type ---
set ENABLE_VST=false
set VST_LEGACY_SDK_PATH=
set ENABLE_VST3=false
set VST3_SDK_PATH=
set ENABLE_AAX=false
set AAX_SDK_PATH=

rem --- Set enable/disable concatet version number for binary file ---
set CONCAT_VERSION_NUMBER=false

rem --- Set Projucer's global search path ---
cd %SCRIPT_DIRECTORY%
if %ENABLE_VST%==true (
    if not "%VST_LEGACY_SDK_PATH%"=="" (
        ..\..\Projucer\Projucer.exe --set-global-search-path windows vstLegacyPath %VST_LEGACY_SDK_PATH%
    ) else (
        echo "Please set a value to the variable VST_LEGACY_SDK_PATH"
        goto FAILURE
    )
)
if %ENABLE_VST3%==true (
    rem --- Because plugin client property "JUCE_VST3_CAN_REPLACE_VST2" is set to "Enabled, it makes have to link with VST legacy SDK." ---
    if not "%VST_LEGACY_SDK_PATH%"=="" (
        ..\..\Projucer\Projucer.exe --set-global-search-path windows vstLegacyPath %VST_LEGACY_SDK_PATH%
    ) else (
        echo "Please set a value to the variable VST_LEGACY_SDK_PATH"
        goto FAILURE
    )

    if not "%VST3_SDK_PATH%"=="" (
        ..\..\Projucer\Projucer.exe --set-global-search-path windows vst3Path %VST3_SDK_PATH%
    ) else (
        ..\..\Projucer\Projucer.exe --set-global-search-path windows vst3Path ..\..\Dependencies\JUCE\modules\juce_audio_processors\format_types\VST3_SDK
    )
)
if %ENABLE_AAX%==true (
    if not "%AAX_SDK_PATH%"=="" (
        ..\..\Projucer\Projucer.exe --set-global-search-path windows aaxPath %AAX_SDK_PATH%
    ) else (
        echo "Please set a value to the variable AAX_SDK_PATH"
        goto FAILURE
    )
)

rem --- Generate IDE project file(.sln) by Projucer ---
cd %SCRIPT_DIRECTORY%
..\..\Projucer\Projucer.exe --resave ..\%PROJECT_NAME%.jucer

rem --- Get solution file name from Projucer ---
cd %SCRIPT_DIRECTORY%
for /f "usebackq delims=" %%a in (`..\..\Projucer\Projucer.exe --status ..\%PROJECT_NAME%.jucer ^| find "Name:"`) do set SOLUTION_NAME=%%a
for /f "tokens=1,2 delims= " %%a in ("%SOLUTION_NAME%") do set SOLUTION_NAME=%%b

rem --- Get project version number from Projucer ---
for /f "usebackq delims=" %%a in (`..\..\Projucer\Projucer.exe --get-version ..\%PROJECT_NAME%.jucer`) do set VERSION_NUMBER=%%a

rem --- Start Visual Studio Developer Command Line Tool ---
if "%MSVC_OFFERING%"=="" (
    echo "Please set a value to the variable MSVC_OFFERING like Community, Professional, Enterprise"
    goto FAILURE
)
call "C:\Program Files (x86)\Microsoft Visual Studio\%MSVC_VERSION%\%MSVC_OFFERING%\Common7\Tools\VsDevCmd.bat"

rem --- Build by MSBuild ---
cd %SCRIPT_DIRECTORY%
call :CLEAN_SOLUTION
if %ERRORLEVEL% neq 0 goto FAILURE

call :BUILD_STANDALONE
if %ERRORLEVEL% neq 0 goto FAILURE

call :BUILD_VST
if %ERRORLEVEL% neq 0 goto FAILURE

call :BUILD_VST3
if %ERRORLEVEL% neq 0 goto FAILURE

call :BUILD_AAX
if %ERRORLEVEL% neq 0 goto FAILURE

:BUILD_FINISH
echo "BUILD_FINISH"
goto SUCCESS

:CLEAN_SOLUTION
rem --- Clean solution --- 
MSBuild ..\Builds\%EXPORTER_NAME%\%SOLUTION_NAME%.sln /t:Clean
if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%
exit /B 0

:BUILD_STANDALONE
rem --- Build Standalone format ---
MSBuild ..\Builds\%EXPORTER_NAME%\%SOLUTION_NAME%.sln /t:"%SOLUTION_NAME% - Standalone Plugin":Rebuild /p:Configuration=%BUILD_CONFIG%;Platform=%ARCHITECTURE%
if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%

if %CONCAT_VERSION_NUMBER%==true (
    rem --- Rename to adding version number for Standalone file --
    move /y "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\Standalone Plugin\%SOLUTION_NAME%.exe" ^
         "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\Standalone Plugin\%SOLUTION_NAME%-%VERSION_NUMBER%.exe"
    if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%
)
exit /B 0

:BUILD_VST
rem --- Build VST format ---
if %ENABLE_VST%==true (
    MSBuild ..\Builds\%EXPORTER_NAME%\%SOLUTION_NAME%.sln /t:"%SOLUTION_NAME% - VST":Rebuild /p:Configuration=%BUILD_CONFIG%;Platform=%ARCHITECTURE%
    if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%

    if %CONCAT_VERSION_NUMBER%==true (
        rem --- Rename to adding version number for VST file --
        if exist "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\VST\%SOLUTION_NAME%.dll" (
            move /y "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\VST\%SOLUTION_NAME%.dll" ^
                "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\VST\%SOLUTION_NAME%-%VERSION_NUMBER%.dll"
        )
        if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%
    )
)
exit /B 0

:BUILD_VST3
rem --- Build VST3 format ---
if %ENABLE_VST3%==true (
    MSBuild ..\Builds\%EXPORTER_NAME%\%SOLUTION_NAME%.sln /t:"%SOLUTION_NAME% - VST3":Rebuild /p:Configuration=%BUILD_CONFIG%;Platform=%ARCHITECTURE%
    if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%

    if %CONCAT_VERSION_NUMBER%==true (
        rem --- Rename to adding version number for VST3 file --
        if exist "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\VST3\%SOLUTION_NAME%.vst3" (
            move /y "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\VST3\%SOLUTION_NAME%.vst3" ^
                "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\VST3\%SOLUTION_NAME%-%VERSION_NUMBER%.vst3"
        )
        if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%
    )
)
exit /B 0

:BUILD_AAX
rem --- Build AAX format ---
if %ENABLE_AAX%==true (
    MSBuild ..\Builds\%EXPORTER_NAME%\%SOLUTION_NAME%.sln /t:"%SOLUTION_NAME% - AAX":Rebuild /p:Configuration=%BUILD_CONFIG%;Platform=%ARCHITECTURE%
    if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%

    if %CONCAT_VERSION_NUMBER%==true (
        rem --- Rename to adding version number for AAX file --
        if exist "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\AAX\%SOLUTION_NAME%.aaxplugin" (
            move /y "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\AAX\%SOLUTION_NAME%.aaxplugin" ^
                "..\Builds\%EXPORTER_NAME%\%ARCHITECTURE%\%BUILD_CONFIG%\AAX\%SOLUTION_NAME%-%VERSION_NUMBER%.aaxplugin"
        )
        if %ERRORLEVEL% neq 0 exit /B %ERRORLEVEL%
    )
)
exit /B 0

:FAILURE
echo ErrorLevel:%ERRORLEVEL%
echo ***Build Failed***
exit 1

:SUCCESS
echo ***Build Success***
exit /B 0