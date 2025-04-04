@stage vertex

in vec2 in_position;
in vec2 in_tex_coord;
in vec4 in_color;
in float in_tex_id;

out vec2 frag_tex_coord;
out vec4 frag_color;
out float frag_tex_id;

uniform UniformBufferObject
{
    mat4 view_projection;
};

void main()
{
    gl_Position = view_projection * vec4(in_position, 0.0, 1.0);
    frag_tex_coord = in_tex_coord;
    frag_color = in_color;
    frag_tex_id = in_tex_id;
}

@stage fragment

uniform sampler2D textures[8];

in vec2 frag_tex_coord;
in vec4 frag_color;
in float frag_tex_id;

out vec4 out_color;

void main()
{
    const vec2 uv = frag_tex_coord;
    switch (int(frag_tex_id))
    {
    case 0: out_color = texture(textures[0], uv); break;
    case 1: out_color = texture(textures[1], uv); break;
    case 2: out_color = texture(textures[2], uv); break;
    case 3: out_color = texture(textures[3], uv); break;
    case 4: out_color = texture(textures[4], uv); break;
    case 5: out_color = texture(textures[5], uv); break;
    case 6: out_color = texture(textures[6], uv); break;
    case 7: out_color = texture(textures[7], uv); break;
    default: out_color = frag_color; return;
    }
    out_color *= frag_color;
}