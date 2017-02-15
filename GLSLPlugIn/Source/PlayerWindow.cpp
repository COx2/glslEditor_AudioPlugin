/*
  ==============================================================================

    PlayerWindow.cpp
    Created: 9 Feb 2017 12:50:51am
    Author:  COx2

  ==============================================================================
*/
#include "StaticValues.h"
#include "PlayerWindow.h"

PlayerWindow::PlayerWindow(String name)
	: DocumentWindow(name, Colours::lightgrey, DocumentWindow::allButtons),
	forwardFFT(fftOrder, false),
	fifoIndex(0),
	nextFFTBlockReady(false)
{
	m_GLSLCompo.setStatusLabelPtr(&m_statusLabel);
	m_GLSLCompo.setFragmentDocPtr(&fragmentDocument);
	//m_GLSLCompo.setEditorPtr(this);

	addKeyListener(this);

	setUsingNativeTitleBar(false);
	setTitleBarHeight(TITLEBAR_HEIGHT);

	setResizable(true, true);
	centreWithSize(600, 600);
	
	m_GLSLCompo.setBounds(0, 0, getWidth(), getHeight());
	setContentOwned(&m_GLSLCompo, true);

	setVisible(true);
	startTimer(shaderLinkDelay);
}

PlayerWindow::~PlayerWindow()
{
	stopTimer();
	setContentNonOwned(&m_GLSLCompo, true);
}

void PlayerWindow::closeButtonPressed()
{
	// This is called when the user tries to close this window. Here, we'll just
	// ask the app to quit when this happens, but you can change this to do
	// whatever you need.
	centreWithSize(600, 600);
}

void PlayerWindow::maximiseButtonPressed()
{
	setFullScreen(!isFullScreen());

	if (isFullScreen())
	{
		setTitleBarHeight(0);
	}
	else
	{
		setTitleBarHeight(TITLEBAR_HEIGHT);
	}
}

bool PlayerWindow::keyPressed(const KeyPress& key, Component* originatingComponent)
{
	if (key.getKeyCode() == key.escapeKey)
	{
		maximiseButtonPressed();
	}
	else if (key.getKeyCode() == key.tabKey)
	{
		updateShader();
	}
	return true;
}

//==============================================================================
void PlayerWindow::timerCallback()
{
	if (StaticValues::getNeedShaderSync())
	{
		StaticValues::setNeedShaderSync(false);
		
		stopTimer();

		if(m_GLSLCompo.isInitialised)
			m_GLSLCompo.setShaderProgramFragment(StaticValues::getShaderCache());
		
		startTimer(shaderLinkDelay);
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

void PlayerWindow::updateShader()
{
	m_GLSLCompo.setShaderProgramFragment(StaticValues::getShaderCache());
}

void PlayerWindow::setMidiCCValue(juce::MidiMessage midiCC)
{
	m_midiCCqueue.push(midiCC);
}

void PlayerWindow::sendMidiCCValue()
{
	while (!m_midiCCqueue.empty())
	{
		juce::MidiMessage midiCC = m_midiCCqueue.front();
		m_midiCCqueue.pop();
		m_GLSLCompo.setMidiCCValue(midiCC.getControllerNumber(), midiCC.getControllerValue());
	}
}

void PlayerWindow::pushNextSampleIntoFifo(float sample) noexcept
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

void PlayerWindow::sendNextSpectrum()
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

void PlayerWindow::sendNextWave()
{
	for (int i = 0; i < fftSize; i++)
	{
		m_GLSLCompo.setWaveValue(i, waveData[i]);
	}
}
