/*
  ==============================================================================

    StaticValues.h
    Created: 9 Feb 2017 2:00:06am
    Author:  COx2

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class StaticValues
{
public:
    static const String getShaderCache();
    static void setShaderCache (String str);

    static const String getShaderCacheVerified();
    static void setShaderCacheVerified (String str);

    static bool getShaderCacheReady();
    static void setShaderCacheReady (bool b);

    static bool getNeedShaderSync();
    static void setNeedShaderSync (bool b);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StaticValues)
};
