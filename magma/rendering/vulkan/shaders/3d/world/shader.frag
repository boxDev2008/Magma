#version 450

layout(binding = 0, set = 1) uniform sampler2D tex_sampler;

layout(location = 0) in vec2 frag_tex_coord;

layout(location = 0) out vec4 out_color;

void main()
{
    vec2 uv = frag_tex_coord.xy;
    out_color = texture(tex_sampler, uv);
}