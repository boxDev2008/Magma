@stage vertex

in vec2 in_position;
in vec2 in_tex_coord;

out vec2 frag_tex_coord;

void main()
{
    gl_Position = vec4(in_position, 0.0, 1.0);
    frag_tex_coord = in_tex_coord;
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