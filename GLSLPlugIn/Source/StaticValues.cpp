/*
  ==============================================================================

    StaticValues.cpp
    Created: 9 Feb 2017 4:09:52am
    Author:  COx2

  ==============================================================================
*/

#include "StaticValues.h"

static juce::String ShaderCache;
static juce::String ShaderCacheVerified;
static bool isShaderCacheReady;
static bool isNeedShaderSync;

const juce::String StaticValues::getShaderCache() { return ShaderCache; }
void StaticValues::setShaderCache (juce::String str)
{
    ShaderCache = str;
    isShaderCacheReady = true;
}

const juce::String StaticValues::getShaderCacheVerified() { return ShaderCacheVerified; }
void StaticValues::setShaderCacheVerified (juce::String str)
{
    ShaderCacheVerified = str;
    isShaderCacheReady = true;
}

bool StaticValues::getShaderCacheReady() { return isShaderCacheReady; }
void StaticValues::setShaderCacheReady (bool b) { isShaderCacheReady = b; }

bool StaticValues::getNeedShaderSync() { return isNeedShaderSync; }
void StaticValues::setNeedShaderSync (bool b) { isNeedShaderSync = b; }