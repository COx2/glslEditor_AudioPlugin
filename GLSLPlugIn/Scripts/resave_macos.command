#!/bin/sh

echo '--- Define script directory ---'
SCRIPT_DIRECTORY=$(cd $(dirname $0);pwd) 

# Script job will terminate when error occured.
set -e

echo '--- Set variables ---'
PROJECT_NAME=GLSLPlugIn
PROJUCER_EXE=${SCRIPT_DIRECTORY}/../../Projucer/Projucer.app/Contents/MacOS/Projucer

echo '--- Generate IDE project file by Projucer ---'
${PROJUCER_EXE} --resave ${SCRIPT_DIRECTORY}/../${PROJECT_NAME}.jucer
