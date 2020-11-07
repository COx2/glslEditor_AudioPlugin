@echo off

rem --- Define script directory ---
set SCRIPT_DIRECTORY=%~dp0

rem --- Set variables for MSVC ---
set PROJECT_NAME=GLSLPlugIn

rem --- Generate IDE project file(.sln) by Projucer ---
cd %SCRIPT_DIRECTORY%
..\..\Projucer\Projucer.exe --resave ..\%PROJECT_NAME%.jucer
