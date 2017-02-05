/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GlslplugInAudioProcessorEditor::GlslplugInAudioProcessorEditor (GlslplugInAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
	//m_GLSLCompo(&m_statusLabel),
	forwardFFT(fftOrder, false),
	fifoIndex(0),
	nextFFTBlockReady(false),
	fragmentEditorComp(fragmentDocument, nullptr)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 1000);

	addAndMakeVisible(m_GLSLCompo);
	m_GLSLCompo.setStatusLabel(&m_statusLabel);

	Colour editorBackground(Colours::darkgrey);
	Colour editorForeground(Colours::white);

	addAndMakeVisible(fragmentEditorComp);
	fragmentEditorComp.setColour(CodeEditorComponent::backgroundColourId, editorBackground);
	fragmentEditorComp.setColour(CodeEditorComponent::defaultTextColourId, editorForeground);
	fragmentDocument.addListener(this);

	addAndMakeVisible(m_statusLabel);
	m_statusLabel.setJustificationType(Justification::topLeft);
	m_statusLabel.setColour(Label::backgroundColourId, Colours::darkcyan);
	m_statusLabel.setColour(Label::textColourId, Colours::white);
	m_statusLabel.setFont(Font(14.0f));

	startTimer(15);

	if (isShaderCacheReady) 
	{
		fragmentDocument.replaceAllContent(ShaderCache);
	}
}

GlslplugInAudioProcessorEditor::~GlslplugInAudioProcessorEditor()
{
}

//==============================================================================
void GlslplugInAudioProcessorEditor::paint (Graphics& g)
{
}

void GlslplugInAudioProcessorEditor::resized()
{
	Rectangle<int> area(getLocalBounds().reduced(4));
	Rectangle<int> bottom(area.removeFromBottom(75));

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
	m_GLSLCompo.setBounds(0, 0, 800, 600);

	fragmentEditorComp.setBounds(0, 600, 800, 340);

	m_statusLabel.setBounds(0, 940, 800, 60);
}

//==============================================================================
void GlslplugInAudioProcessorEditor::timerCallback()
{
	if (isNeedShaderCompile) {
		stopTimer();
		m_GLSLCompo.setShaderProgramFragment(fragmentDocument.getAllContent());

		ShaderCache = fragmentDocument.getAllContent();
		isShaderCacheReady = true;

		startTimer(60);
	}

	// MIDI CC
	if (!m_midiCCqueue.empty()) {
		sendMidiCCValue();
	}

	// Wave
	if (nextWaveBlockReady)
	{
		sendNextWave();
		nextWaveBlockReady = false;
	}

	// FFT
	if (nextFFTBlockReady)
	{
		sendNextSpectrum();
		nextFFTBlockReady = false;
	}
}

void GlslplugInAudioProcessorEditor::codeDocumentTextInserted(const String& /*newText*/, int /*insertIndex*/)
{
	startTimer(shaderLinkDelay);
	isNeedShaderCompile = true;
}

void GlslplugInAudioProcessorEditor::codeDocumentTextDeleted(int /*startIndex*/, int /*endIndex*/)
{
	startTimer(shaderLinkDelay);
	isNeedShaderCompile = true;
}

void GlslplugInAudioProcessorEditor::setMidiCCValue(juce::MidiMessage midiCC)
{
	m_midiCCqueue.push(midiCC);
}

void GlslplugInAudioProcessorEditor::sendMidiCCValue()
{
	while (!m_midiCCqueue.empty())
	{
		juce::MidiMessage midiCC = m_midiCCqueue.front();
		m_midiCCqueue.pop();
		m_GLSLCompo.setMidiCCValue(midiCC.getControllerNumber(), midiCC.getControllerValue());
	}
}

void GlslplugInAudioProcessorEditor::pushNextSampleIntoFifo(float sample) noexcept
{
	// if the fifo contains enough data, set a flag to say
	// that the next line should now be rendered..
	if (fifoIndex == fftSize)
	{
		if (!nextWaveBlockReady)
		{
			zeromem(waveData, sizeof(waveData));
			memcpy(waveData, fifo, sizeof(fifo));
			nextWaveBlockReady = true;
		}

		if (!nextFFTBlockReady)
		{
			zeromem(fftData, sizeof(fftData));
			memcpy(fftData, fifo, sizeof(fifo));
			nextFFTBlockReady = true;
		}

		fifoIndex = 0;
	}

	fifo[fifoIndex++] = sample;
}

void GlslplugInAudioProcessorEditor::sendNextSpectrum()
{
	// then render our FFT data..
	forwardFFT.performFrequencyOnlyForwardTransform(fftData);

	// find the range of values produced, so we can scale our rendering to
	// show up the detail clearly
	for (int i = 0; i < fftSize; i++) 
	{
		auto spectrumVal = fftData[i];
		m_GLSLCompo.setSpectrumValue(i, spectrumVal * spectrumVal);
	}
}

void GlslplugInAudioProcessorEditor::sendNextWave() 
{
	for (int i = 0; i < fftSize; i++)
	{
		m_GLSLCompo.setWaveValue(i, waveData[i]);
	}
}