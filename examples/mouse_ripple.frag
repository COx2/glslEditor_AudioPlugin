// mouse_ripple.frag
//
// Ripples that emanate from the mouse position while dragging in the
// rendering window.
//
// Uniforms used:
//   uniform float time
//   uniform vec2  mouse       (0..1, updated while dragging)
//   uniform vec2  resolution

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec2 centre = mouse;

    float dist = distance(uv, centre);
    float ripple = sin(dist * 40.0 - time * 6.0) * 0.5 + 0.5;
    ripple *= smoothstep(0.75, 0.0, dist);

    vec3 col = mix(vec3(0.0, 0.05, 0.15), vec3(0.2, 0.8, 1.0), ripple);

    gl_FragColor = vec4(col, 1.0);
}
