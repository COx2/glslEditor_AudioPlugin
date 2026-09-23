juce_add_binary_data(GLSLPlugInData SOURCES Source/Resources/base.obj)

juce_add_plugin(GLSLPlugIn
    PRODUCT_NAME "GLSLPlugIn"
    PLUGIN_NAME "GLSLPlugIn"
    DESCRIPTION "GLSLPlugIn"
    COMPANY_NAME "JUCE JAPAN"
    COMPANY_COPYRIGHT "JUCE JAPAN"
    BUNDLE_ID "com.JuceJapan.GLSLPlugIn"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE X25u
    FORMATS Standalone VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    NEEDS_CURL FALSE
    NEEDS_WEB_BROWSER FALSE
)

target_sources(GLSLPlugIn
    PRIVATE
        Source/GLSLComponent.cpp
        Source/StaticValues.cpp
        Source/PlayerWindow.cpp
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_compile_definitions(GLSLPlugIn
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(GLSLPlugIn
    PRIVATE
        GLSLPlugInData
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_cryptography
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_opengl
        juce::juce_video
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_generate_juce_header(GLSLPlugIn)

# Adds a GLSLPlugIn_CLAP target alongside the Standalone/VST3 ones, using the
# same CLAP_ID/company identity as the other formats' BUNDLE_ID.
clap_juce_extensions_plugin(TARGET GLSLPlugIn
    CLAP_ID "com.JuceJapan.GLSLPlugIn"
    CLAP_FEATURES "audio-effect" "analyzer"
)
