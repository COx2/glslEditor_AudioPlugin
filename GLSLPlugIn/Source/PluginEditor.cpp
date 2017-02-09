/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "StaticValues.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GlslplugInAudioProcessorEditor::GlslplugInAudioProcessorEditor (GlslplugInAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
	forwardFFT(fftOrder, false),
	fifoIndex(0),
	nextFFTBlockReady(false),
	fragmentEditorComp(fragmentDocument, nullptr)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1440, 600);
	getTopLevelComponent()->addKeyListener(this);

	m_GLSLCompo.setStatusLabelPtr(&m_statusLabel);
	m_GLSLCompo.setFragmentDocPtr(&fragmentDocument);
	m_GLSLCompo.setEditorPtr(this);
	addAndMakeVisible(m_GLSLCompo);

	Colour editorBackground(Colours::darkgrey);
	Colour editorForeground(Colours::white);

	fragmentEditorComp.setColour(CodeEditorComponent::backgroundColourId, editorBackground);
	fragmentEditorComp.setColour(CodeEditorComponent::defaultTextColourId, editorForeground);
	fragmentDocument.addListener(this);
	addAndMakeVisible(fragmentEditorComp);

	m_statusLabel.setJustificationType(Justification::topLeft);
	m_statusLabel.setColour(Label::backgroundColourId, Colours::darkcyan);
	m_statusLabel.setColour(Label::textColourId, Colours::white);
	m_statusLabel.setFont(Font(14.0f));
	addAndMakeVisible(m_statusLabel);

	startTimer(15);

	if (StaticValues::getShaderCacheReady())
	{
		fragmentDocument.replaceAllContent(StaticValues::getShaderCache());
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

	fragmentEditorComp.setBounds(800, 0, 640, 540);

	m_statusLabel.setBounds(800, 540, 640, 60);

	if (isCodeEditorShow)
	{
		fragmentEditorComp.setVisible(true);
		m_statusLabel.setVisible(true);
		this->setSize(1440, 600);
	}
	else
	{
		fragmentEditorComp.setVisible(false);
		m_statusLabel.setVisible(false);
		this->setSize(800, 600);
	}
}

//==============================================================================
void GlslplugInAudioProcessorEditor::timerCallback()
{
	if (isNeedShaderCompile) 
	{
		isNeedShaderCompile = false;

		stopTimer();
		
		StaticValues::setShaderCache(fragmentDocument.getAllContent());

		m_GLSLCompo.setShaderProgramFragment(StaticValues::getShaderCache());

		setShaderSync();

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

void GlslplugInAudioProcessorEditor::setShaderSync()
{
	StaticValues::setNeedShaderSync(true);
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

bool GlslplugInAudioProcessorEditor::keyPressed(const KeyPress& key, Component* originatingComponent)
{
	//m_statusLabel.setText(m_statusLabel.getText() + "keyCode:" + String(key.getKeyCode()) + "/keyChar:" + String(key.getTextCharacter()), dontSendNotification);

	if (key.getModifiers() == ModifierKeys::ctrlModifier) 
	{
		if (key.getKeyCode() == 75) // "k"
		{
			isCodeEditorShow = !isCodeEditorShow;
			this->resized();
		}
	}
	return true;
}