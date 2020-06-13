/*
  ==============================================================================

    GLSLComponent.cpp
    Created: 31 Jan 2017 4:20:16am
    Author:  COx2

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GLSLTypes.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class GLSLComponent   : public OpenGLAppComponent,
                        public AsyncUpdater
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
	void paint(Graphics& g) override;
	void resized() override;

    //==============================================================================
	void setStatusLabelPtr(Label* _statusLabel);
	void setFragmentDocPtr(CodeDocument* _fragmentDoc);

    //==============================================================================
	void setShaderProgram(const String& vertexShader, const String& fragmentShader);
	void setShaderProgramFragment(const String& _fragmentShader);
	void setShaderProgramVertex(const String& _vertexShader);

    //==============================================================================
	void setMidiCCValue(int ccNumber, float value);
	void setSpectrumValue(int spectrumNumber, float value);
	void setWaveValue(int waveNumber, float value);

    //==============================================================================
	bool isInitialised = false;
	bool isShaderCompileSuccess = false;

    static const String defaultVertexShader;
    static const String defaultFragmentShader;

private:
    //==============================================================================
    virtual void handleAsyncUpdate() override;

	//==============================================================================
	void createShaders();
	void updateShader();
	void mouseDrag(const MouseEvent& event) override;

    //==============================================================================
	Matrix3D<float> getProjectionMatrix() const;
	Matrix3D<float> getViewMatrix() const;
	
    //==============================================================================
    String vertexShader;
    String fragmentShader;

    String statusText{};
	Label* statusLabel = nullptr;
	CodeDocument* fragmentDoc = nullptr;

    std::unique_ptr<OpenGLShaderProgram> shader;
    std::unique_ptr<Shape> shape;
    std::unique_ptr<Attributes> attributes;
    std::unique_ptr<Uniforms> uniforms;

	GLfloat timeCounter = 0.0f;

    String newVertexShader, newFragmentShader;
	bool isShaderCompileReady = false;
	float mouseX, mouseY;
	float m_midiCC[128] = {0};
	float m_spectrum[256] = {0};
	float m_wave[256] = {0};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GLSLComponent)
};
