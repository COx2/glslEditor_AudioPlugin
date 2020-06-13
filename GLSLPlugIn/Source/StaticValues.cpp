/*
  ==============================================================================

    StaticValues.cpp
    Created: 9 Feb 2017 4:09:52am
    Author:  COx2

  ==============================================================================
*/

#include "StaticValues.h"

static String ShaderCache;
static String ShaderCacheVerified;
static bool isShaderCacheReady;
static bool isNeedShaderSync;

const String StaticValues::getShaderCache() { return ShaderCache; }
void StaticValues::setShaderCache(String str) { 
	ShaderCache = str;
	isShaderCacheReady = true;
}

const String StaticValues::getShaderCacheVerified() { return ShaderCacheVerified; }
void StaticValues::setShaderCacheVerified(String str) {
	ShaderCacheVerified = str;
	isShaderCacheReady = true;
}

bool StaticValues::getShaderCacheReady() { return isShaderCacheReady; }
void StaticValues::setShaderCacheReady(bool b) { isShaderCacheReady = b; }

bool  StaticValues::getNeedShaderSync() { return isNeedShaderSync; }
void  StaticValues::setNeedShaderSync(bool b) { isNeedShaderSync = b; }