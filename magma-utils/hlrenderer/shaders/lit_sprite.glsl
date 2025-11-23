@stage vertex

in vec2 in_position;
in vec2 in_tex_coord;
in vec4 in_color;
in float in_tex_id;
in float in_grayscale;

out vec2 frag_position;
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
    frag_position = in_position;
    frag_tex_coord = in_tex_coord;
    frag_color = in_color;
    frag_tex_id = in_tex_id;
    frag_grayscale = in_grayscale;
}

@stage fragment

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;
layout(binding = 2) uniform sampler2D texture2;
layout(binding = 3) uniform sampler2D texture3;

in vec2 frag_position;
in vec2 frag_tex_coord;
in vec4 frag_color;
in float frag_tex_id;
in float frag_grayscale;

out vec4 out_color;

#define MAX_LIGHT_COUNT 128
layout(binding = 1) uniform LightData
{
    vec4 light_transform[MAX_LIGHT_COUNT];
    vec4 light_colors[MAX_LIGHT_COUNT];
    vec4 global_light_data;
    vec4 resolution;
    int light_count;
};

vec3 calculate_lights(vec2 uv)
{
    vec3 final_color = vec3(0.0);
    for (int i = 0; i < light_count && i < MAX_LIGHT_COUNT; i++)
    {
        vec2 norm = vec2(resolution.x / resolution.y, 1.0);
        float distance = distance(light_transform[i].xy, uv);
        float attenuation = 1.0 / (1.0 + 10.0 * (distance / light_transform[i].z) * (distance / light_transform[i].z));
        final_color += light_transform[i].w * light_colors[i].rgb * attenuation;
    }

    vec4 global_light = global_light_data * global_light_data.w;

    if (final_color.r < global_light.r) final_color.r = global_light.r;
    if (final_color.g < global_light.g) final_color.g = global_light.g;
    if (final_color.b < global_light.b) final_color.b = global_light.b;

    return final_color;
}

void main()
{
    const vec2 uv = frag_tex_coord;
    vec3 lit_color = calculate_lights(frag_position);

    switch (int(frag_tex_id))
    {
    case 0: out_color = texture(texture0, uv); break;
    case 1: out_color = texture(texture1, uv); break;
    case 2: out_color = texture(texture2, uv); break;
    case 3: out_color = texture(texture3, uv); break;
    default: out_color = frag_color * vec4(lit_color, 1.0); return;
    }

    if (frag_grayscale > 0.0)
    {
        const float gray = dot(out_color.rgb, vec3(0.299, 0.587, 0.114));
        out_color.rgb = mix(out_color.rgb, vec3(gray), frag_grayscale);
    }

    out_color *= frag_color * vec4(lit_color, 1.0);
}