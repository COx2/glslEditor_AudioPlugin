/*
  ==============================================================================

    StaticValues.h
    Created: 9 Feb 2017 2:00:06am
    Author:  COx2

  ==============================================================================
*/

#ifndef STATICVALUES_H_INCLUDED
#define STATICVALUES_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class StaticValues
{
public:
	static const String getShaderCache();
	static void setShaderCache(String str);

	static bool getShaderCacheReady();
	static void setShaderCacheReady(bool b);

	static bool getNeedShaderSync();
	static void setNeedShaderSync(bool b);
};

#endif  // STATICVALUES_H_INCLUDED
