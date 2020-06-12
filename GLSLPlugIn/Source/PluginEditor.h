/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#include <queue>
//==============================================================================
/**
*/
class GlslplugInAudioProcessorEditor  : public AudioProcessorEditor,
										public CodeDocument::Listener,
										public KeyListener,
	                                    public Button::Listener,										
										private Timer
{
public:
    GlslplugInAudioProcessorEditor (GlslplugInAudioProcessor&);
    ~GlslplugInAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void timerCallback() override;
	void setMidiCCValue(juce::MidiMessage midiCC);
	void setShaderSync();
	void pushNextSampleIntoFifo(float sample) noexcept;

	GlslplugInAudioProcessor& processor;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
	GLSLComponent m_GLSLCompo;
	Label m_statusLabel;
	CodeDocument /*vertexDocument,*/ fragmentDocument;
	CodeEditorComponent /*vertexEditorComp,*/ fragmentEditorComp;
	ToggleButton m_SyncModeSwitch;
	TextButton m_SyncButton;
	TextButton m_PlayWndButton;

	enum { shaderLinkDelay = 500 };
	enum
	{
		fftOrder = 9,
		fftSize = 1 << fftOrder
	};
	void codeDocumentTextInserted(const String& /*newText*/, int /*insertIndex*/) override;
	void codeDocumentTextDeleted(int /*startIndex*/, int /*endIndex*/) override;

	std::queue<juce::MidiMessage> m_midiCCqueue;
	void sendMidiCCValue();
	void sendNextSpectrum();
	void sendNextWave();

	// FFT 
    dsp::FFT forwardFFT;
	float fifo[fftSize];
	float fftData[2 * fftSize];
	int fifoIndex;
	bool nextFFTBlockReady;

	// Wave
	bool nextWaveBlockReady;
	float waveData[fftSize];

	// KeyListener
	//bool keyPressed(const KeyPress& key) override;
	bool keyPressed(const KeyPress& key, Component* originatingComponent) override;
	
	/** Called when the button is clicked. */
	virtual void buttonClicked(Button*) override;
	
	bool isShaderSyncAuto = false;
	bool isNeedShaderCompile = false;
	
	int wndFullSizeW = 1024;
	int wndFullSizeH = 576;
	
	enum GUIState {
		Default,
		PreviewOnly,
		EditorOnly,
	};
	GUIState m_guiState = GUIState::Default;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlslplugInAudioProcessorEditor)
};
