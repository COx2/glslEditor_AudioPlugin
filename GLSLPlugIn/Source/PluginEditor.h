/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "GLSLComponent.h"
//==============================================================================
/**
*/
class GlslplugInAudioProcessorEditor  : public AudioProcessorEditor,
										private CodeDocument::Listener,
										//private ComboBox::Listener,
										//private Slider::Listener,
										//private Button::Listener,
										private Timer
{
public:
    GlslplugInAudioProcessorEditor (GlslplugInAudioProcessor&);
    ~GlslplugInAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	
	Label m_statusLabel;
	CodeDocument /*vertexDocument,*/ fragmentDocument;
	CodeEditorComponent /*vertexEditorComp,*/ fragmentEditorComp;

	void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GlslplugInAudioProcessor& processor;

	GLSLComponent m_GLSLCompo;

	enum { shaderLinkDelay = 500 };

	void codeDocumentTextInserted(const String& /*newText*/, int /*insertIndex*/) override;
	void codeDocumentTextDeleted(int /*startIndex*/, int /*endIndex*/) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlslplugInAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
