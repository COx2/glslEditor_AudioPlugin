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
	m_statusLabel.setJustificationType(Justification::centredLeft);
	m_statusLabel.setColour(Label::backgroundColourId, Colours::darkcyan);
	m_statusLabel.setColour(Label::textColourId, Colours::white);
	m_statusLabel.setFont(Font(14.0f));
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

	fragmentEditorComp.setBounds(0, 600, 800, 380);

	m_statusLabel.setBounds(0, 980, 800, 20);
}

//==============================================================================
void GlslplugInAudioProcessorEditor::timerCallback()
{
	stopTimer();
	m_GLSLCompo.setShaderProgramFragment(fragmentDocument.getAllContent());
}

void GlslplugInAudioProcessorEditor::codeDocumentTextInserted(const String& /*newText*/, int /*insertIndex*/)
{
	startTimer(shaderLinkDelay);
}

void GlslplugInAudioProcessorEditor::codeDocumentTextDeleted(int /*startIndex*/, int /*endIndex*/)
{
	startTimer(shaderLinkDelay);
}