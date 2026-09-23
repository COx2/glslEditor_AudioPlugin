/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PlayerWindow.h"

//==============================================================================
/**
*/
class GlslplugInAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================
    GlslplugInAudioProcessor();
    ~GlslplugInAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    using AudioProcessor::processBlock;
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void createPlayerWindow();
    void deletePlayerWindow();
    bool existPlayerWindow() { const ScopedLock sl (playerWindowLock); return playerWindow != nullptr; }

private:
    //==============================================================================
    // playerWindow is created/destroyed from the message thread (via the "Player
    // Window" button) but also read and called into from processBlock() on the
    // audio thread, so all access to it must go through this lock - otherwise the
    // audio thread can end up calling into a window the message thread just deleted.
    CriticalSection playerWindowLock;
    PlayerWindow* playerWindow = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlslplugInAudioProcessor)
};
