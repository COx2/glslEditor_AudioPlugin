project(GLSLPlugIn VERSION 2.0.0)

set (BaseTargetName GLSLPlugIn)

juce_add_plugin("${BaseTargetName}"
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "JUCE_JAPAN"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Cox2
    PLUGIN_CODE Webg
    FORMATS Standalone VST3 AU #AUv3 #LV2
    LV2URI "urn:juce-japan:glslplugin"
    PRODUCT_NAME "GLSLPlugIn")

clap_juce_extensions_plugin(TARGET "${BaseTargetName}"
    CLAP_ID "com.juce-japan.glslplugin"
    CLAP_FEATURES audio-effect filter mono stereo)

juce_add_binary_data("${BaseTargetName}_BundleResource"
    HEADER_NAME "BundleResource.h"
    NAMESPACE "BundleResource"
    SOURCES
        Source/Resources/base.obj
    )

# Need to build BinaryData with -fPIC flag on Linux
set_target_properties("${BaseTargetName}_BundleResource"
    PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE)

target_sources(${BaseTargetName}
    PRIVATE
        Source/StaticValues.cpp
        Source/StaticValues.h
        Source/GLSLComponent.cpp
        Source/GLSLComponent.h
        Source/GLSLTypes.h
        Source/PlayerWindow.cpp
        Source/PlayerWindow.h
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h)

target_compile_definitions(${BaseTargetName}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# Use PUBLIC to effect linked target like VST3, Standalone, CLAP, ...
target_compile_features(${BaseTargetName}
    PUBLIC
        cxx_std_17)

target_include_directories(${BaseTargetName}
    PUBLIC
        ${choc_SOURCE_DIR})

target_link_libraries(${BaseTargetName}
    PRIVATE
        "${BaseTargetName}_BundleResource"
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_audio_devices
        juce::juce_dsp
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Add cmake install task for this project.
if(TARGET ${BaseTargetName}_VST3)
    get_target_property(VST3_PRODUCT_FILE ${BaseTargetName}_VST3 JUCE_PLUGIN_ARTEFACT_FILE)
    message("VST3_PRODUCT_FILE: ${VST3_PRODUCT_FILE}")

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "/Library/Audio/Plug-Ins/VST3"
                COMPONENT ${BaseTargetName}_VST3)

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "C:/Program Files/Common Files/VST3"
                COMPONENT ${BaseTargetName}_VST3)

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "/usr/lib/vst3"
                COMPONENT ${BaseTargetName}_VST3)

    endif()
endif()

if(TARGET ${BaseTargetName}_CLAP)
    get_target_property(CLAP_PRODUCT_FILE ${BaseTargetName}_CLAP JUCE_PLUGIN_ARTEFACT_FILE)
    message("CLAP_PRODUCT_FILE: ${CLAP_PRODUCT_FILE}")

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

        install(FILES "${CLAP_PRODUCT_FILE}"
                DESTINATION "/Library/Audio/Plug-Ins/CLAP"
                COMPONENT ${BaseTargetName}_CLAP)

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

        install(FILES "${CLAP_PRODUCT_FILE}"
                DESTINATION "C:/Program Files/Common Files/CLAP"
                COMPONENT ${BaseTargetName}_CLAP)

    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

        install(FILES "${VST3_PRODUCT_FILE}"
                DESTINATION "/usr/lib/clap"
                COMPONENT ${BaseTargetName}_CLAP)
    endif()
endif()
