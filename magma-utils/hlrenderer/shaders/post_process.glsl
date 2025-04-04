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

uniform sampler2D tex_sampler;

in vec2 frag_tex_coord;
out vec4 out_color;

void main()
{
    vec2 uv = frag_tex_coord.xy;

    out_color = texture(tex_sampler, uv);
}