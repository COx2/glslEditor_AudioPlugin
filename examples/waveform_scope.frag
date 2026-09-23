// waveform_scope.frag
//
// Audio-reactive oscilloscope, drawing the raw waveform captured from the
// DAW's mixer as a scrolling line. Feed the plugin some audio to see it move.
//
// Uniforms used:
//   uniform vec2  resolution
//   uniform float wave[256]   (-1..1 approx, per-sample amplitude)

uniform vec2 resolution;
uniform float wave[256];

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;

    int sampleIndex = int(uv.x * 256.0);
    float amplitude = 0.0;

    // GLSL requires a constant loop bound, so scan every sample and keep
    // the one that matches this pixel's column.
    for (int i = 0; i < 256; i++)
    {
        if (i == sampleIndex)
            amplitude = wave[i];
    }

    float y = 0.5 + amplitude * 0.5;
    float line = smoothstep(0.02, 0.0, abs(uv.y - y));

    vec3 col = mix(vec3(0.0, 0.02, 0.05), vec3(0.1, 1.0, 0.4), line);

    gl_FragColor = vec4(col, 1.0);
}
