@stage vertex

const vec2 positions[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

const vec2 tex_coords[4] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0)
);

out vec2 frag_tex_coord;

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    frag_tex_coord = tex_coords[gl_VertexIndex];
}

@stage fragment

layout(binding = 0) uniform sampler2D tex_sampler;

layout(binding = 0) uniform PostProcessData
{
    vec4 vignette_data;
    bool enable_vignette;
};

in vec2 frag_tex_coord;
out vec4 out_color;

void main()
{
    vec2 uv = frag_tex_coord.xy;
    vec3 color = texture(tex_sampler, uv).rgb;

    if (enable_vignette)
    {
        vec2 centered_uv = uv - vec2(0.5);
        float dist = length(centered_uv);
        float vignette = smoothstep(0.7, 0.5, dist);
        float factor = mix(1.0, vignette, vignette_data.a);
        color = mix(vignette_data.rgb, color, factor);
    }

    out_color = vec4(color, 1.0);
}