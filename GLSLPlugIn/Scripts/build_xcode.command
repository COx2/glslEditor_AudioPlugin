#!/bin/sh

echo '--- Define script directory ---'
SCRIPT_DIRECTORY=$(cd $(dirname $0);pwd) 
cd ${SCRIPT_DIRECTORY}

# Script job will terminate when error occured.
set -e

# Prepare function
function FAILURE() {
  echo "*** Build Failed ***"
  exit
}

function SUCCESS() {
  echo "*** Build Success ***"
  exit
}

echo '--- Set variables ---'
PROJECT_NAME=GLSLPlugIn
ARCHITECTURE=x86_64
BUILD_CONFIG=Release
EXPORTER_NAME=MacOSX

echo '--- Set variables for enabling each plugin format type ---'
ENABLE_VST=true
VST_LEGACY_SDK_PATH="${SCRIPT_DIRECTORY}/../../3rd-party/VST3 SDK"
ENABLE_VST3=false
VST3_SDK_PATH=
ENABLE_AAX=false
AAX_SDK_PATH=
ENABLE_AU=true

echo '--- Set enable/disable concatet version number for binary file ---'
CONCAT_VERSION_NUMBER=false

PROJUCER_EXE="${SCRIPT_DIRECTORY}/../../Projucer/Projucer.app/Contents/MacOS/Projucer"

echo '--- Set Projucer global search path ---'
if test "${ENABLE_VST}" = "true"; then
    if test "${VST_LEGACY_SDK_PATH}" != ""; then
        "${PROJUCER_EXE}" --set-global-search-path osx vstLegacyPath "${VST_LEGACY_SDK_PATH}"
        echo "${VST_LEGACY_SDK_PATH}"
    else
        echo "*** Please set a value to the variable VST_LEGACY_SDK_PATH ***"
        FAILURE
    fi
fi

if test "${ENABLE_VST3}" = "true"; then
    # --- Because plugin client property "JUCE_VST3_CAN_REPLACE_VST2" is set to "Enabled, it makes have to link with VST legacy SDK." ---
    if test "${VST_LEGACY_SDK_PATH}" != ""; then
        "${PROJUCER_EXE}" --set-global-search-path osx vstLegacyPath "${VST_LEGACY_SDK_PATH}"
    else
        echo "*** Please set a value to the variable VST_LEGACY_SDK_PATH ***"
        FAILURE
    fi

    if test "${VST3_SDK_PATH}" != ""; then
        "${PROJUCER_EXE}" --set-global-search-path osx vst3Path "${VST3_SDK_PATH}"
    else
        "${PROJUCER_EXE}" --set-global-search-path osx vst3Path "${SCRIPT_DIRECTORY}/../../Dependencies/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
    fi
fi

if test "${ENABLE_AAX}" = "true"; then
    if test "${AAX_SDK_PATH}" != ""; then
        "${PROJUCER_EXE}" --set-global-search-path osx aaxPath "${AAX_SDK_PATH}"
    else
        echo "*** Please set a value to the variable AAX_SDK_PATH ***"
        FAILURE
    fi
fi

echo '--- Generate IDE project file by Projucer ---'
"${PROJUCER_EXE}" --resave "${SCRIPT_DIRECTORY}/../${PROJECT_NAME}.jucer"

echo '--- Get solution file name from Projucer ---'
SOLUTION_NAME=`${PROJUCER_EXE} --status ${SCRIPT_DIRECTORY}/../${PROJECT_NAME}.jucer | grep "Name:" | awk '{ print $2 }'`

echo '--- Get project version number from Projucer ---'
VERSION_NUMBER=`${PROJUCER_EXE} --get-version ${SCRIPT_DIRECTORY}/../${PROJECT_NAME}.jucer`

echo '--- Show variables ---'
echo 'SCRIPT_DIRECTORY: '${SCRIPT_DIRECTORY}
echo 'PROJECT_NAME: '${PROJECT_NAME}
echo 'VERSION_NUMBER:'${VERSION_NUMBER}
echo 'ARCHITECTURE: '${ARCHITECTURE}
echo 'BUILD_CONFIG: '${BUILD_CONFIG}
echo 'SOLUTION_NAME: '${SOLUTION_NAME}
echo 'EXPORTER_NAME: '${EXPORTER_NAME}

echo '--- Show list of '${SOLUTION_NAME}'.xcodeproj ---'
xcodebuild -project "${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/${SOLUTION_NAME}.xcodeproj" -list

echo '--- Show Xcode build settings ---'
xcodebuild -project "${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/${SOLUTION_NAME}.xcodeproj" \
-alltargets \
-configuration ${BUILD_CONFIG} \
-arch ${ARCHITECTURE} \
-showBuildSettings

echo '--- Run Xcode build Standalone Plugin ---'
xcodebuild -project "${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/${SOLUTION_NAME}.xcodeproj" \
-target "${SOLUTION_NAME} - Standalone Plugin" \
-configuration ${BUILD_CONFIG} \
-arch ${ARCHITECTURE}

if test "${CONCAT_VERSION_NUMBER}" = "true"; then
    echo '--- Rename to adding version number for Standalone Plugin --'
    SRC_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}"
    DEST_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}-${VERSION_NUMBER}"
    if test -e "${DEST_FILE}"; then
        rm -rf "${DEST_FILE}"
    fi
    if test -e "${SRC_FILE}"; then
        mv -f "${SRC_FILE}" "${DEST_FILE}"
    fi
fi

if test "${ENABLE_VST}" = "true"; then
    echo '--- Run Xcode build VST ---'
    xcodebuild -project "${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/${SOLUTION_NAME}.xcodeproj" \
    -target "${SOLUTION_NAME} - VST" \
    -configuration ${BUILD_CONFIG} \
    -arch ${ARCHITECTURE}

    if test "${CONCAT_VERSION_NUMBER}" = "true"; then
        echo '--- Rename to adding version number for VST file --'
        SRC_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}.vst"
        DEST_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}-${VERSION_NUMBER}.vst"
        if test -e "${DEST_FILE}"; then
            rm -rf "${DEST_FILE}"
        fi
        if test -e "${SRC_FILE}"; then
            mv -f "${SRC_FILE}" "${DEST_FILE}"
        fi
    fi
fi

if test "${ENABLE_VST3}" = "true"; then
    echo '--- Run Xcode build VST3 ---'
    xcodebuild -project "${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/${SOLUTION_NAME}.xcodeproj" \
    -target "${SOLUTION_NAME} - VST3" \
    -configuration ${BUILD_CONFIG} \
    -arch ${ARCHITECTURE}

    if test "${CONCAT_VERSION_NUMBER}" = "true"; then
        echo '--- Rename to adding version number for VST3 file --'
        SRC_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}.vst3"
        DEST_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}-${VERSION_NUMBER}.vst3"
        if test -e "${DEST_FILE}"; then
            rm -rf "${DEST_FILE}"
        fi
        if test -e "${SRC_FILE}"; then
            mv -f "${SRC_FILE}" "${DEST_FILE}"
        fi
    fi
fi

if test "${ENABLE_AAX}" = "true"; then
    echo '--- Run Xcode build AAX ---'
    xcodebuild -project "${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/${SOLUTION_NAME}.xcodeproj" \
    -target "${SOLUTION_NAME} - AAX" \
    -configuration ${BUILD_CONFIG} \
    -arch ${ARCHITECTURE}

    if test "${CONCAT_VERSION_NUMBER}" = "true"; then
        echo '--- Rename to adding version number for AAX file --'
        SRC_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}.aaxplugin"
        DEST_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}-${VERSION_NUMBER}.aaxplugin"
        if test -e "${DEST_FILE}"; then
            rm -rf "${DEST_FILE}"
        fi
        if test -e "${SRC_FILE}"; then
            mv -f "${SRC_FILE}" "${DEST_FILE}"
        fi
    fi
fi

if test "${ENABLE_AU}" = "true"; then
    echo '--- Run Xcode build AU ---'
    xcodebuild -project "${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/${SOLUTION_NAME}.xcodeproj" \
    -target "${SOLUTION_NAME} - AU" \
    -configuration ${BUILD_CONFIG} \
    -arch ${ARCHITECTURE}

    if test "${CONCAT_VERSION_NUMBER}" = "true"; then
        echo '--- Rename to adding version number for AU file --'
        SRC_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}.component"
        DEST_FILE="${SCRIPT_DIRECTORY}/../Builds/${EXPORTER_NAME}/build/${BUILD_CONFIG}/${SOLUTION_NAME}-${VERSION_NUMBER}.component"
        if test -e "${DEST_FILE}"; then
            rm -rf "${DEST_FILE}"
        fi
        if test -e "${SRC_FILE}"; then
            mv -f "${SRC_FILE}" "${DEST_FILE}"
        fi
    fi
fi
