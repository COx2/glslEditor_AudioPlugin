/*
  ==============================================================================

    PlayerWindow.h
    Created: 9 Feb 2017 12:25:11am
    Author:  COx2

  ==============================================================================
*/

#ifndef PLAYERWINDOW_H_INCLUDED
#define PLAYERWINDOW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "GLSLComponent.h"
#include <queue>
//==============================================================================
/*
This class implements the desktop window that contains an instance of
our MainContentComponent class.
*/
class PlayerWindow : public DocumentWindow,
					public KeyListener,
					private Timer
{
public:
	PlayerWindow(String name);
	~PlayerWindow();

	//==============================================================================
	/* Note: Be careful if you override any DocumentWindow methods - the base
	class uses a lot of them, so by overriding you might break its functionality.
	It's best to do all your work in your content component instead, but if
	you really have to override any DocumentWindow methods, make sure your
	subclass also calls the superclass's method.
	*/
	void closeButtonPressed() override;
	void maximiseButtonPressed() override;
	bool keyPressed(const KeyPress& key, Component* originatingComponent) override;

	void updateShader();
	void timerCallback() override;
	void setMidiCCValue(juce::MidiMessage midiCC);

	enum
	{
		fftOrder = 9,
		fftSize = 1 << fftOrder
	};

	void pushNextSampleIntoFifo(float sample) noexcept;

private:
	const int TITLEBAR_HEIGHT = 26;
	GLSLComponent m_GLSLCompo;

	Label m_statusLabel;
	CodeDocument /*vertexDocument,*/ fragmentDocument;

	enum { shaderLinkDelay = 500 };

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

	bool isCodeEditorShow = true;
	bool isNeedShaderCompile = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerWindow)
};
//==============================================================================


#endif  // PLAYERWINDOW_H_INCLUDED
