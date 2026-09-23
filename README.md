[![Github All Releases](https://img.shields.io/github/downloads/COx2/glslEditor_AudioPlugin/total.svg)]()  [![GitHub release](https://img.shields.io/github/release/COx2/glslEditor_AudioPlugin.svg)](https://github.com/COx2/glslEditor_AudioPlugin/releases)

## glslEditor_AudioPlugin ##
GLSL for Audio Plugin

Project Name : GLSL Editor Audio Plugin (+stand-alone)

Product Name : None

Current Version : v1.2.0

License : MIT

[JUCE JAPAN / Outlaw Faction](http://oufac.com/ "Outlaw Faction")

Require : CMake 3.22+ (JUCE is fetched automatically, see Building below)

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
+ CLAP
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

## Keyboard Shortcuts ##

The plugin window is resizable (drag any corner/edge) so you can give the code
editor more room for long shader lines. In addition:

| Shortcut | Effect |
|---|---|
| Ctrl + K | Toggle "Preview Only" (fullscreen GLSL render, hides the code editor) |
| Ctrl + P | Toggle "Editor Only" (fullscreen code editor, hides the GLSL preview) |
| Ctrl + `+` (or `;`) | Increase the code editor's font size |
| Ctrl + `-` | Decrease the code editor's font size |

Note: this is the Ctrl key specifically, even on macOS (not Cmd).

## Building ##

The project uses CMake with JUCE fetched automatically via `FetchContent` — there's no
Projucer project and no separate JUCE download/checkout needed.

### Requirements ###

* CMake 3.22 or newer
* A C++ compiler:
  * Windows: Visual Studio 2019 or newer
  * macOS: Xcode (command line tools)
* An internet connection for the first configure (CMake downloads JUCE 8.0.15 automatically)

### Build Instructions (Windows, macOS) ###

```sh
# Configure (downloads JUCE and clap-juce-extensions into build/_deps on first run)
cmake -B build

# Build the Standalone, VST3, and CLAP targets, Release configuration
cmake --build build --config Release
```

To build a single format instead of everything:

```sh
cmake --build build --target GLSLPlugIn_Standalone --config Release
cmake --build build --target GLSLPlugIn_VST3 --config Release
cmake --build build --target GLSLPlugIn_CLAP --config Release
```

Build products are placed under `build/GLSLPlugIn/GLSLPlugIn_artefacts/Release/`
(`Standalone/GLSLPlugIn.app` or `.exe`, `VST3/GLSLPlugIn.vst3`, and `CLAP/GLSLPlugIn.clap`).

CLAP support comes from the third-party [clap-juce-extensions](https://github.com/free-audio/clap-juce-extensions)
project (JUCE itself has no native CLAP format) - it's fetched automatically alongside JUCE, no extra setup needed.

On Windows, `cmake -B build` uses the Visual Studio generator by default, producing a
solution you can also open and build directly in the IDE. On macOS, pass `-G Xcode`
if you'd rather work from an Xcode project:

```sh
cmake -B build -G Xcode
```

### Testing shaders headlessly ###

The Standalone build accepts a `-i`/`--input <file>` flag that loads a fragment
shader from disk, compiles it exactly as if it had been pasted into the editor,
prints `OK: ...` or `ERROR: ...` (the same message the status bar would show) to
the console, and exits with code 0 on success or 1 on failure. This is useful for
scripting/CI, or for diagnosing driver-specific shader compile issues without
having to drive the GUI:

```sh
build/GLSLPlugIn/GLSLPlugIn_artefacts/Standalone/GLSLPlugIn --input examples/plasma_time.frag
```

### Technologies Used ###
  * C++ for the language
  * JUCE (fetched via CMake `FetchContent`) for the framework/library
  * CMake for the build system
