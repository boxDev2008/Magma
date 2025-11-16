@stage vertex

in vec2 in_position;
in vec2 in_tex_coord;
in vec4 in_color;
in float in_tex_id;
in float in_grayscale;

out vec2 frag_tex_coord;
out vec4 frag_color;
out float frag_tex_id;
out float frag_grayscale;

layout(binding = 0) uniform ViewData
{
    mat4 view_projection;
};

void main()
{
    gl_Position = view_projection * vec4(in_position, 0.0, 1.0);
    frag_tex_coord = in_tex_coord;
    frag_color = in_color;
    frag_tex_id = in_tex_id;
    frag_grayscale = in_grayscale;
}

@stage fragment

layout(set = 1, binding = 0) uniform sampler2D texture0;
layout(set = 1, binding = 1) uniform sampler2D texture1;
layout(set = 1, binding = 2) uniform sampler2D texture2;
layout(set = 1, binding = 3) uniform sampler2D texture3;

in vec2 frag_tex_coord;
in vec4 frag_color;
in float frag_tex_id;
in float frag_grayscale;

out vec4 out_color;

void main()
{
    const vec2 uv = frag_tex_coord;
    switch (int(frag_tex_id))
    {
    case 0: out_color = texture(texture0, uv); break;
    case 1: out_color = texture(texture1, uv); break;
    case 2: out_color = texture(texture2, uv); break;
    case 3: out_color = texture(texture3, uv); break;
    default: out_color = frag_color; return;
    }

    if (frag_grayscale > 0.0)
    {
        const float gray = dot(out_color.rgb, vec3(0.299, 0.587, 0.114));
        out_color.rgb = mix(out_color.rgb, vec3(gray), frag_grayscale);
    }

    out_color *= frag_color;
}