// spectrum_bars.frag
//
// Audio-reactive frequency spectrum analyzer, drawn as vertical bars.
// Feed the plugin some audio (route a track/bus into it) to see it move.
//
// Uniforms used:
//   uniform vec2  resolution
//   uniform float spectrum[256]   (0..1, per-band magnitude, low to high frequency)

uniform vec2 resolution;
uniform float spectrum[256];

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;

    int band = int(uv.x * 256.0);
    float magnitude = 0.0;

    // GLSL requires a constant loop bound, so scan every band and keep
    // the one that matches this pixel's column.
    for (int i = 0; i < 256; i++)
    {
        if (i == band)
            magnitude = spectrum[i];
    }

    float bar = step(1.0 - uv.y, magnitude);
    vec3 col = mix(vec3(0.05), vec3(uv.x, magnitude, 1.0 - uv.x), bar);

    gl_FragColor = vec4(col, 1.0);
}
