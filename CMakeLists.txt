cmake_minimum_required(VERSION 3.16)

project(glslEditor_AudioPlugin)

#This is temporarily needed due to a bug in Xcode 15:
if (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "15.0")
        add_compile_definitions(JUCE_SILENCE_XCODE_15_LINKER_WARNING=1)
        set(CMAKE_EXE_LINKER_FLAGS "-Wl,-ld_classic" CACHE INTERNAL "")
    endif ()
endif ()

#First, we'll add the CMake folder, incase we'll need to find_package later:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

#Compile commands, useful for some IDEs like VS-Code
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#Minimum MacOS target, set globally
if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "Minimum OS X deployment version" FORCE)

    #code signing to run on an iOS device:
    #    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer" CACHE STRING "" FORCE)
    #    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "XXXXXXXXXX" CACHE STRING "" FORCE)
else ()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version" FORCE)
endif ()

# From clap: If running into issues when Xcode tries to codesign the CLAP plugin, you may want to add these lines:
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")

option(UniversalBinary "Build universal binary for mac" ON)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif ()

# static linking in Windows
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# For building in multibyte language environment.
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/utf-8)
endif()

# Add JUCE
# Adds all the module sources so they appear correctly in the IDE
set_property(GLOBAL PROPERTY USE_FOLDERS YES)
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/JUCE)

# Add clap-juce-extensions
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/clap-juce-extensions)

# Add plugins project
add_subdirectory(GLSLPlugIn)
