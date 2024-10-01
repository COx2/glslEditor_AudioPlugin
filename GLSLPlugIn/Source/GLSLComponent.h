/*
  ==============================================================================

    GLSLComponent.cpp
    Created: 31 Jan 2017 4:20:16am
    Author:  COx2

  ==============================================================================
*/

#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

#include "GLSLTypes.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class GLSLComponent : public juce::OpenGLAppComponent,
                      public juce::AsyncUpdater
{
public:
    //==============================================================================
    GLSLComponent();
    ~GLSLComponent();

    //==============================================================================
    void initialise() override;
    void shutdown() override;
    void render() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    void setStatusLabelPtr (juce::Label* _statusLabel);
    void setFragmentDocPtr (juce::CodeDocument* _fragmentDoc);

    //==============================================================================
    void setShaderProgram (const juce::String& vertexShader, const juce::String& fragmentShader);
    void setShaderProgramFragment (const juce::String& _fragmentShader);
    void setShaderProgramVertex (const juce::String& _vertexShader);

    //==============================================================================
    void setMidiCCValue (int ccNumber, float value);
    void setSpectrumValue (int spectrumNumber, float value);
    void setWaveValue (int waveNumber, float value);

    //==============================================================================
    bool isInitialised = false;
    bool isShaderCompileSuccess = false;

    static const juce::String defaultVertexShader;
    static const juce::String defaultFragmentShader;

    struct ShaderPreset
    {
        const char* name;
        const char* fragmentShader;
    };

    static juce::Array<ShaderPreset> getShaderPresets();
private:
    //==============================================================================
    virtual void handleAsyncUpdate() override;

    //==============================================================================
    void createShaders();
    void updateShader();
    void mouseDrag (const juce::MouseEvent& event) override;

    //==============================================================================
    juce::Matrix3D<float> getProjectionMatrix() const;
    juce::Matrix3D<float> getViewMatrix() const;

    //==============================================================================
    juce::String vertexShader;
    juce::String fragmentShader;

    juce::String statusText {};
    juce::Label* statusLabel = nullptr;
    juce::CodeDocument* fragmentDoc = nullptr;

    std::unique_ptr<juce::OpenGLShaderProgram> shader;
    std::unique_ptr<Shape> shape;
    std::unique_ptr<Attributes> attributes;
    std::unique_ptr<Uniforms> uniforms;

    GLfloat timeCounter = 0.0f;

    juce::String newVertexShader, newFragmentShader;
    bool isShaderCompileReady = false;
    float mouseX, mouseY;
    float m_midiCC[128] = { 0 };
    float m_spectrum[256] = { 0 };
    float m_wave[256] = { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GLSLComponent)
};
