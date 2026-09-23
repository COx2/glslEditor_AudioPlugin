// plasma_time.frag
//
// Classic "GLSL Sandbox" style plasma effect.
// Uses only `time` and `resolution`, so it works even with no MIDI/audio input.
//
// Uniforms used:
//   uniform float time
//   uniform vec2  resolution

uniform float time;
uniform vec2 resolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;

    float v = 0.0;
    v += sin((uv.x * 10.0) + time);
    v += sin((uv.y * 10.0) + time * 1.3);
    v += sin((uv.x + uv.y) * 10.0 + time * 0.7);
    v += sin(sqrt(uv.x * uv.x + uv.y * uv.y) * 20.0 - time * 2.0);
    v *= 0.25;

    vec3 col = vec3(sin(v * 3.14159), sin(v * 3.14159 + 2.094), sin(v * 3.14159 + 4.188));
    col = col * 0.5 + 0.5;

    gl_FragColor = vec4(col, 1.0);
}
