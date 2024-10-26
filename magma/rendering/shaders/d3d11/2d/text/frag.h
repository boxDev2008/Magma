#pragma once

const char HLSL_TEXT_FRAG[] = 
"Texture2D tex_sampler : register(t0);"
"SamplerState samp : register(s0);"

"struct PSInput"
"{"
"    float4 pos : SV_Position;"
"    float2 texCoord : TEXCOORD0;"
"    float4 color : COLOR;"
"};"

"float4 PSMain(PSInput input) : SV_Target"
"{"
"    float4 tex_color = tex_sampler.Sample(samp, input.texCoord);"
"    if (tex_color.a > 0)"
"    {"
"        tex_color = float4(1.0, 1.0, 1.0, tex_color.a);"
"    }"
"    return tex_color * input.color;"
"}\0";