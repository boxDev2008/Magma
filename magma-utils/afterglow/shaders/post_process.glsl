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
layout(binding = 0) uniform sampler2D world_sampler;
layout(binding = 1) uniform sampler2D background_sampler;

#define MAX_LIGHT_COUNT 128
#define MAX_REFLECTIVE_RECT_COUNT 8

struct ReflectiveRect
{
    vec4 rect;
    vec4 tint;
    vec4 data;
};

struct PointLight
{
    vec4 transform; // x, y, radius, reserved
    vec4 color; // r, g, b, intensity
};

layout(binding = 0) uniform PostProcessData
{
    ReflectiveRect reflective_rects[MAX_REFLECTIVE_RECT_COUNT];
    PointLight point_lights[MAX_LIGHT_COUNT];

    vec4 resolution;
    vec4 vignette_data;
    vec4 godrays_data;
    vec4 global_light_data;
    ivec4 flags;
    ivec4 data0;
    float time;
};

in vec2 frag_tex_coord;
out vec4 out_color;

vec3 calculate_lights(vec2 uv)
{
    vec3 final_color = vec3(0.0);
    
    const int light_count = data0.y;
    for (int i = 0; i < light_count && i < MAX_LIGHT_COUNT; i++)
    {
        vec2 light_pos = point_lights[i].transform.xy;
        vec2 light_scale = point_lights[i].transform.zw;
        float intensity = point_lights[i].color.a;
        
        vec2 delta = uv - light_pos;
        float normalized_dist = length(delta / light_scale);
        
        float attenuation = 1.0 / (1.0 + 10.0 * normalized_dist * normalized_dist);
        final_color += intensity * point_lights[i].color.rgb * attenuation;
    }
    
    vec4 global_light = global_light_data * global_light_data.w;
    if (final_color.r < global_light.r) final_color.r = global_light.r;
    if (final_color.g < global_light.g) final_color.g = global_light.g;
    if (final_color.b < global_light.b) final_color.b = global_light.b;
    
    return final_color;
}

float calculate_godrays(vec2 uv)
{
    float final_color;
    if (texture(world_sampler, uv).a > 0)
    {
        final_color = 0.0;
        vec2 check_uv = uv;
        for (int i = 0; i < 32; i++)
        {
            check_uv.x -= godrays_data.x;
            check_uv.y -= godrays_data.y;
            final_color += (texture(world_sampler, check_uv).a / (13 + pow(2, i / 3)));
        }
    }
    else
        return 0.0;
    return final_color;
}

vec4 apply_godrays_to_color(vec4 color, vec2 uv)
{
    if (flags.y == 1)
    {
        float shadow_color = calculate_godrays(uv) * godrays_data.z;
        vec3 lit_color = vec3(1.0);
        lit_color += (vec3(0.8, 0.9, 1.0) - vec3(shadow_color));
        color.rgb *= lit_color;
    }
    return color;
}

void main()
{
    vec2 uv = frag_tex_coord.xy;

    vec4 background = texture(background_sampler, uv);
    vec4 color = texture(world_sampler, uv);
    
    color = apply_godrays_to_color(color, uv);

    vec3 lighting = calculate_lights(uv);
    color.rgb *= lighting;

    color.rgb = mix(background.rgb, color.rgb, color.a);

    
    const int reflective_rect_count = data0.x;
    for (int i = 0; i < reflective_rect_count && i < MAX_REFLECTIVE_RECT_COUNT; i++)
    {
        vec4 rect = reflective_rects[i].rect;
        vec4 tint = reflective_rects[i].tint;
        
        if (uv.x >= rect.x && uv.x <= rect.x + rect.z && 
            uv.y >= rect.y && uv.y <= rect.y + rect.w)
        {
            float rect_top = rect.y;
            float local_y = uv.y - rect_top;
            float mirrored_y = rect_top - local_y;
            
            float wave = sin(uv.x * 800.0 + uv.y * 600.0 + time * 10.0f) * reflective_rects[i].data.x;
            vec2 mirror_uv = vec2(uv.x + wave, mirrored_y);
            
            vec2 texel_size = vec2(1.0) / textureSize(world_sampler, 0);
            float blur_radius = reflective_rects[i].data.y;
            
            vec4 reflection = vec4(0.0);
            vec4 reflection_bg = vec4(0.0);
            float total_weight = 0.0;
            
            for (int y = -1; y <= 1; y++)
            {
                for (int x = -1; x <= 1; x++)
                {
                    vec2 offset = vec2(float(x), float(y)) * texel_size * blur_radius;
                    reflection += texture(world_sampler, mirror_uv + offset);
                    reflection_bg += texture(background_sampler, mirror_uv + offset);
                    total_weight += 1.0;
                }
            }
            
            reflection /= total_weight;
            reflection_bg /= total_weight;
            
            reflection = apply_godrays_to_color(reflection, mirror_uv);
            vec3 mirror_lighting = calculate_lights(mirror_uv);
            reflection.rgb *= mirror_lighting;
            reflection.a *= 1.0 - (local_y / rect.w);
            
            reflection.rgb = mix(reflection_bg.rgb, reflection.rgb, reflection.a);
            
            color = mix(color, reflection, 1.0);
            
            float gradient = pow(1.0 - (local_y / rect.w), 64.0);
            color.rgb += 0.08 * gradient;
            color.rgb *= tint.rgb;
        }
    }
    
    if (flags.x == 1)
    {
        vec2 centered_uv = uv - vec2(0.5);
        float dist = length(centered_uv);
        float vignette = smoothstep(0.8, 0.2, dist);
        float factor = mix(1.0, vignette, vignette_data.a);
        color.rgb = mix(vignette_data.rgb, color.rgb, factor);
        color.a = max(color.a, (1.0 - factor) * vignette_data.a);
    }
    
    out_color = vec4(color.rgb, 1.0);
}
