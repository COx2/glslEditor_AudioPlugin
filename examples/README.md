# Shader examples

Fragment shaders demonstrating the uniform variables this plugin provides in
addition to the standard "GLSL Sandbox" ones (`time`, `mouse`, `resolution`).
The vertex shader is fixed by the plugin, so these files only cover the
fragment stage — the part you actually edit in the plugin's code editor.

To try one out: open the shader in a text editor, copy its contents, paste it
into the plugin's fragment shader editor pane, and it will compile and apply
live.

| File | Uniforms used | What it needs |
|---|---|---|
| `plasma_time.frag` | `time`, `resolution` | Nothing — animates on its own |
| `mouse_ripple.frag` | `time`, `mouse`, `resolution` | Drag the mouse over the rendering window |
| `spectrum_bars.frag` | `resolution`, `spectrum[256]` | Audio routed into the plugin |
| `waveform_scope.frag` | `resolution`, `wave[256]` | Audio routed into the plugin |
| `midi_cc_color.frag` | `time`, `resolution`, `midiCC[128]` | MIDI CC 1, 7, and 74 from your controller/DAW |

## Available uniforms

| Uniform | Type | Description |
|---|---|---|
| `time` | `float` | Seconds-ish counter, advances every frame |
| `mouse` | `vec2` | Mouse position (0..1) while dragging in the rendering window |
| `resolution` | `vec2` | Size in pixels of the rendering window |
| `midiCC[128]` | `float[128]` | Latest value of each MIDI CC number received from the host, roughly 0..127 |
| `wave[256]` | `float[256]` | 256-sample snapshot of the audio waveform from the DAW's mixer |
| `spectrum[256]` | `float[256]` | 256-band frequency spectrum computed from that same audio, low to high frequency |
