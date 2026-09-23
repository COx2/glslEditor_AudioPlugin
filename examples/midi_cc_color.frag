// midi_cc_color.frag
//
// MIDI-controlled color field. Send MIDI CC events from your DAW/controller
// to change the look in real time.
//
//   CC 1  (mod wheel) -> hue
//   CC 7  (volume)    -> brightness
//   CC 74 (filter cutoff, common on many controllers) -> pattern scale
//
// Uniforms used:
//   uniform float time
//   uniform vec2  resolution
//   uniform float midiCC[128]   (roughly 0..127, depending on the DAW)

uniform float time;
uniform vec2 resolution;
uniform float midiCC[128];

vec3 hsv2rgb(vec3 c)
{
    vec4 k = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + k.xyz) * 6.0 - k.www);
    return c.z * mix(k.xxx, clamp(p - k.xxx, 0.0, 1.0), c.y);
}

void main()
{
    vec2 uv = (gl_FragCoord.xy / resolution.xy) - 0.5;

    float hue = midiCC[1] / 127.0;
    float brightness = 0.3 + 0.7 * (midiCC[7] / 127.0);
    float scale = 4.0 + (midiCC[74] / 127.0) * 16.0;

    float pattern = sin(uv.x * scale + time) * cos(uv.y * scale - time);
    float v = brightness * (0.5 + 0.5 * pattern);

    vec3 col = hsv2rgb(vec3(hue, 0.85, v));

    gl_FragColor = vec4(col, 1.0);
}
