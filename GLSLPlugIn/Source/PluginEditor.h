/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include <queue>
//==============================================================================
/**
*/
class GlslplugInAudioProcessorEditor : public AudioProcessorEditor,
                                       public CodeDocument::Listener,
                                       public KeyListener,
                                       public Button::Listener,
                                       private Timer
{
public:
    GlslplugInAudioProcessorEditor (GlslplugInAudioProcessor&);
    ~GlslplugInAudioProcessorEditor() override;

    //==============================================================================
    using Component::keyPressed;

    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void setMidiCCValue (juce::MidiMessage midiCC);
    void setShaderSync();
    void pushNextSampleIntoFifo (float sample) noexcept;

    GlslplugInAudioProcessor& audioProcessor;

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

    enum
    {
        shaderLinkDelay = 500
    };
    enum
    {
        fftOrder = 9,
        fftSize = 1 << fftOrder
    };
    void codeDocumentTextInserted (const String& /*newText*/, int /*insertIndex*/) override;
    void codeDocumentTextDeleted (int /*startIndex*/, int /*endIndex*/) override;

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
    bool keyPressed (const KeyPress& key, Component*) override;

    /** Called when the button is clicked. */
    virtual void buttonClicked (Button*) override;

    bool isShaderSyncAuto = false;
    bool isNeedShaderCompile = false;

    // -i/--input command line support: loads a fragment shader from disk, compiles
    // it the same way a pasted-in shader would be, prints the result to the
    // console, and quits with a 0 (success) or 1 (failure) exit code. Intended for
    // headless/scripted testing of shader files without driving the GUI.
    void checkForCommandLineInput();
    String commandLineInputSource;
    bool commandLineInputPending = false;
    bool commandLineInputAwaitingResult = false;

    int wndFullSizeW = 1024;
    int wndFullSizeH = 576;

    enum GUIState
    {
        Default,
        PreviewOnly,
        EditorOnly,
    };
    GUIState m_guiState = GUIState::Default;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlslplugInAudioProcessorEditor)
};
