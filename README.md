[![Github All Releases](https://img.shields.io/github/downloads/COx2/glslEditor_AudioPlugin/total.svg)]()  [![GitHub release](https://img.shields.io/github/release/COx2/glslEditor_AudioPlugin.svg)](https://github.com/COx2/glslEditor_AudioPlugin/releases)

## glslEditor_AudioPlugin ##
GLSL for Audio Plugin

Project Name : GLSL Editor Audio Plugin (+stand-alone)

Product Name : None

Current Version : v1.0.0

License : GPL v3

[JUCE JAPAN / Outlaw Faction](http://oufac.com/ "Outlaw Faction")

Require : JUCE v5.2.0

## What is this project? ##
GLSL Editor running on VST/AU plugin host Applications Cubase, StudioOne, Ableton Live, Logic, and more...
This Editor already defined uniform variables, and GLSL code compatible for "GLSL Sandbox".
http://glslsandbox.com/

This demo movie has copy and paste the glsl codes from "GLSL Sandbox Gallery".
![Alt Text](VST_GLSL_0134.gif)

## Support Format

+ VST
+ VST3
+ AudioUnit
+ Stand-alone executable

## Usage / uniform variables
Some uniform variables are specific to the GLSL Sandbox and others are provided by this plugin.

▼ GLSL Sandbox Specific
~~~
uniform float time:
 Get current time. Real-time animation can be realized by using this.
 * Ver 1.0 gives the frame counter every 0.2.

uniform vec2 mouse:
 While mouse dragging in the rendering window, you can get mouse coordinates. Both the X and Y directions range from 0 to 1.

uniform vec2 resolution:
 You can get the resolution of the current rendering window.

uniform vec2 surfacePosition:
 You can get the coordinates of the current surface (like billboard).
 ~~~
 
▼ This plug-in original implements
~~~
uniform float midiCC[128]:
 You can obtain MIDI CC events from the DAW (VST host). Get MIDI CC events as an array. "MidiCC [0]" means the value of "CC 0".
 The range of values ​​stored in each array is roughly 0 to 127, depending on the DAW.
 * For MIDI CC specifications, refer to the article of Wikipedia.

uniform float wave[256]:
 You can acquire voice waveform from DAW's mixer. Acquires 256 speech waveforms as an array. "Wave [0]" means the value of the "first sample" of the waveform.
 The range of values ​​stored in each array is roughly 0 to 1, depending on the DAW.

uniform float spectrum[256]:
 It is possible to obtain the value obtained by converting the speech waveform on the mixer of the DAW into the frequency spectrum. Acquire the frequency spectrum of 256 resolution as an array. "Spectrum [0]" means the value of the "lowest frequency" counted from the lower frequency in the frequency spectrum.
 The range of values ​​stored in each array is roughly 0 to 1, depending on the DAW.
 ~~~

## Building ##

So I've built under Windows and Mac OS X.

### Windows Build Instructions ###

1. Download Juce (http://www.juce.com/)
2. Download the VST SDK (http://www.steinberg.net/en/company/developers.html)
3. Run "The Projucer" executable included in Juce.
4. Open juce project file "xxx.jucer"
   - Make any changes to the configure and build options.
   - Save juce project if modified
5. Hit "Save Project and Open in Visual Studio". I use Visual Studio 2015.
6. Select the build: "Release - x64" and set platform to x64(64bit). Otherwise, "Release - Win32" and set platform to x86(32bit).
7. Build and deploy to plugin folder.

### Mac OS X Build Instructions ###

1. Download Juce (http://www.juce.com/)
2. Download the VST SDK (http://www.steinberg.net/en/company/developers.html)
3. Run "The Projucer" executable included in Juce.
4. Open juce project file "xxx.jucer"
   - Make any changes to the configure and build options.
   - Save juce project if modified
5. Hit "Save Project and Open in Xcode". I use Xcode 7.
6. Select the architecture x64(64bit) or x86(32bit).
7. On default setting, When build succeed and automatically deploy to plugin directory.


### Technologies Used ###
  * C++ for the language
  * JUCE for the framework/library
  * Steinberg VST SDK
  * Visual Studio for the IDE
