/*
  ==============================================================================

    StaticValues.h
    Created: 9 Feb 2017 2:00:06am
    Author:  COx2

  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>

class StaticValues
{
public:
    static const juce::String getShaderCache();
    static void setShaderCache (juce::String str);

    static const juce::String getShaderCacheVerified();
    static void setShaderCacheVerified (juce::String str);

    static bool getShaderCacheReady();
    static void setShaderCacheReady (bool b);

    static bool getNeedShaderSync();
    static void setNeedShaderSync (bool b);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StaticValues)
};
