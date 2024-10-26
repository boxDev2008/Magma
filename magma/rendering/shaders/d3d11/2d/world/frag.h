#pragma once

const char HLSL_WORLD_2D_FRAG[] = 
"#define MAX_LIGHT_COUNT 1024\n"

"Texture2D tex_sampler : register(t0);"
"SamplerState samp : register(s0);"

"struct PSInput"
"{"
"    float4 pos : SV_Position;"
"    float2 texCoord : TEXCOORD0;"
"};"

"cbuffer UniformBufferObject : register(b0)"
"{"
"    float4 light_transform[MAX_LIGHT_COUNT];"
"    float4 light_colors[MAX_LIGHT_COUNT];"
"    float4 global_light_data;"
"    float4 shadow_data;"
"    float2 resolution;"
"    int light_count;"
"    bool flag_enable_vignette;"
"    bool flag_enable_lights;"
"    bool flag_enable_shadows;"
"};"

"float3 calculate_lights(float2 uv)"
"{"
"    float3 final_color = float3(0.0, 0.0, 0.0);"
"    for (int i = 0; i < light_count && i < MAX_LIGHT_COUNT; i++)"
"    {"
"        float2 norm = float2(resolution.x / resolution.y, 1.0);"
"        float distance = distance(light_transform[i].xy * norm, uv * norm);"
"        float attenuation = 1.0 / (1.0 + 10.0 * (distance / light_transform[i].z) * (distance / light_transform[i].z));"
"        final_color += light_transform[i].w * light_colors[i].rgb * attenuation;"
"    }"

"    float4 global_light = global_light_data * global_light_data.w;"

"    final_color.r = max(final_color.r, global_light.r);"
"    final_color.g = max(final_color.g, global_light.g);"
"    final_color.b = max(final_color.b, global_light.b);"

"    return final_color;"
"}"

"float4 calculate_shadows(float2 uv)"
"{"
"    float4 final_color = float4(0, 0, 0, 0);"
"    if (tex_sampler.Sample(samp, uv).a > 0)"
"    {"
"        float2 check_uv = uv;"

"        for (int i = 0; i < 32; i++)"
"        {"
"            check_uv.x += shadow_data.x;"
"            check_uv.y -= shadow_data.y;"
"            final_color.a += (tex_sampler.Sample(samp, check_uv).a / (13 + pow(2, i / 3)));"
"        }"
"    }"
"    return final_color;"
"}"

"float4 PSMain(PSInput input) : SV_Target"
"{"
"    float2 uv = input.texCoord;"

"    float4 out_color = tex_sampler.Sample(samp, uv);"

"    float3 lit_color = float3(1.0, 1.0, 1.0);"

"    if (flag_enable_shadows)"
"    {"
"        float4 shadow_color = calculate_shadows(uv) * shadow_data.z;"
"        lit_color += (float3(0.8, 0.9, 1.0) - float3(shadow_color.a, shadow_color.a, shadow_color.a));"
"    }"

"    if (flag_enable_lights)"
"    {"
"        lit_color *= calculate_lights(uv);"
"    }"

"    out_color *= float4(lit_color, 1.0);"

"    if (flag_enable_vignette)"
"    {"
"        uv *=  1.0 - uv.yx;"
"        float vig = uv.x * uv.y * 15.0;"
"        vig = pow(vig, 0.25);"
"        out_color += float4(0.0, 0.0, 0.0, 1.0 - vig);"
"    }"
"    return out_color;"
"}\0";