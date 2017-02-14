/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GlslplugInAudioProcessor::GlslplugInAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	StaticValues::setShaderCache("");
	StaticValues::setShaderCacheReady(false);

}

GlslplugInAudioProcessor::~GlslplugInAudioProcessor()
{
	delete playerWindow; // (deletes our window)
}

void GlslplugInAudioProcessor::createPlayerWindow()
{
	playerWindow = new PlayerWindow("GLSL Player");
}

void GlslplugInAudioProcessor::deletePlayerWindow()
{
	delete playerWindow;
	playerWindow = nullptr;
}

//==============================================================================
const String GlslplugInAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GlslplugInAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GlslplugInAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double GlslplugInAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GlslplugInAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GlslplugInAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GlslplugInAudioProcessor::setCurrentProgram (int index)
{
}

const String GlslplugInAudioProcessor::getProgramName (int index)
{
    return String();
}

void GlslplugInAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void GlslplugInAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GlslplugInAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GlslplugInAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GlslplugInAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	auto editor = static_cast<GlslplugInAudioProcessorEditor*>(getActiveEditor());

	///////////////////////////////////////////////////////////
	int time;
	MidiMessage m;

	for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
	{
		if (m.isNoteOn())
		{
		}
		else if (m.isNoteOff())
		{
		}
		else if (m.isAftertouch())
		{
		}
		else if (m.isPitchWheel())
		{
		}
		else if (m.isController())
		{
			if(editor != nullptr)
				editor->setMidiCCValue(m);

			if (playerWindow != nullptr)
				playerWindow->setMidiCCValue(m);
		}
	}

	///////////////////////////////////////////////////////////
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
		if (channel == 0)
		{
			if (editor != nullptr)
			{
				for (int i = 0; i < buffer.getNumSamples(); ++i)
					editor->pushNextSampleIntoFifo(channelData[i]);
			}

			if (playerWindow != nullptr)
			{
				for (int i = 0; i < buffer.getNumSamples(); ++i)
					playerWindow->pushNextSampleIntoFifo(channelData[i]);
			}
		}
    }
}

//==============================================================================
bool GlslplugInAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GlslplugInAudioProcessor::createEditor()
{
    return new GlslplugInAudioProcessorEditor (*this);
}

//==============================================================================
void GlslplugInAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

	XmlElement root("Root");
	XmlElement *el;
	el = root.createNewChildElement("FragmentShader");
	el->addTextElement(StaticValues::getShaderCache());
	copyXmlToBinary(root, destData);
}

void GlslplugInAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

	XmlElement* pRoot = getXmlFromBinary(data, sizeInBytes);
	if (pRoot != nullptr)
	{
		juce::XmlElement* pChild;
		for (pChild = (*pRoot).getFirstChildElement(); pChild != nullptr; pChild = pChild->getNextElement() )
		{
			if (pChild->hasTagName("FragmentShader"))
			{
				String text = pChild->getAllSubText();
				StaticValues::setShaderCache(text);
			}
		}
		delete pRoot;
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GlslplugInAudioProcessor();
}
