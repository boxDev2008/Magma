#version 450

#extension GL_EXT_scalar_block_layout : enable

layout(binding = 0, set = 1) uniform sampler2D tex_sampler;

layout(location = 0) in vec2 frag_tex_coord;

layout(location = 0) out vec4 out_color;

#define MAX_LIGHT_COUNT 16

layout(std430, binding = 0) uniform UniformBufferObject
{
    vec4 light_transform[MAX_LIGHT_COUNT];
    vec4 light_colors[MAX_LIGHT_COUNT];
    vec4 global_light_data;
    vec4 shadow_data;
    vec2 resolution;
    int light_count;
    bool flag_enable_vignette;
    bool flag_enable_lights;
    bool flag_enable_shadows;
}
ubo;

vec3 calculate_lights(vec2 uv)
{
    vec3 final_color = vec3(0.0);
    for (int i = 0; i < ubo.light_count && i < MAX_LIGHT_COUNT; i++)
    {
        vec2 norm = vec2(ubo.resolution.x / ubo.resolution.y, 1.0);
        float distance = distance(ubo.light_transform[i].xy * norm, uv * norm);
        float attenuation = 1.0 / (1.0 + 10.0 * (distance / ubo.light_transform[i].z) * (distance / ubo.light_transform[i].z));
        final_color += ubo.light_transform[i].w * ubo.light_colors[i].rgb * attenuation;
    }

    vec4 global_light = ubo.global_light_data * ubo.global_light_data.w;

    if (final_color.r < global_light.r) final_color.r = global_light.r;
    if (final_color.g < global_light.g) final_color.g = global_light.g;
    if (final_color.b < global_light.b) final_color.b = global_light.b;

    return final_color;
}

vec4 calculate_shadows(vec2 uv)
{
    vec4 final_color;
    if (texture(tex_sampler, uv).a > 0)
    {
        final_color = vec4(0, 0, 0, 0);
        vec2 check_uv = uv;

        for (int i = 0; i < 32; i++)
        {
            check_uv.x += ubo.shadow_data.x;
            check_uv.y -= ubo.shadow_data.y;
            final_color.a += (texture(tex_sampler, check_uv).a / (13 + pow(2, i / 3)));
        }
    }
    else
        return vec4(0, 0, 0, 0);
    return final_color;
}

void main()
{
    vec2 uv = frag_tex_coord.xy;

    out_color = texture(tex_sampler, uv);

    vec3 lit_color = vec3(1.0);

    if (ubo.flag_enable_shadows)
    {
        vec4 shadow_color = calculate_shadows(uv) * ubo.shadow_data.z;
        lit_color += (vec3(0.8, 0.9, 1.0) - vec3(shadow_color.a, shadow_color.a, shadow_color.a));
    }

    if (ubo.flag_enable_lights)
    {
        lit_color *= calculate_lights(uv);
    }

    out_color *= vec4(lit_color, 1.0);

    if (ubo.flag_enable_vignette)
    {
        uv *=  1.0 - uv.yx;
        float vig = uv.x*uv.y * 15.0;
        vig = pow(vig, 0.25);
        // out_color += vec4(1.0 - vig, 0.0, 0.0, 1.0 - vig); NOTE: subtract vig when having color
        out_color += vec4(0.0, 0.0, 0.0, 1.0 - vig);
    }
}