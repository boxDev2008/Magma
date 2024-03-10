#version 450

layout(binding = 0, set = 1) uniform sampler2D tex_sampler;

layout(location = 0) in vec2 frag_tex_coord;
layout(location = 1) in vec4 frag_color;

layout(location = 0) out vec4 out_color;

void main()
{
    vec4 tex_color = texture(tex_sampler, frag_tex_coord);
    if (tex_color.a > 0)
        tex_color = vec4(1.0, 1.0, 1.0, tex_color.a);

    out_color = tex_color * frag_color;
}