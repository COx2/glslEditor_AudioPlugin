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

	startTimer(30);
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
		startTimer(30);
	}

	if (!m_midiCCqueue.empty()) {
		sendMidiCCValue();
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
		//auto cText = m_statusLabel.getText();
		//cText += " /" + String(midiCC.getControllerNumber()) + "-" + String(midiCC.getControllerValue());
		//m_statusLabel.setText(cText, dontSendNotification);
	}
}