#pragma once

const char HLSL_WORLD_3D_FRAG[] = 
"Texture2D tex_sampler : register(t0);"
"SamplerState samp : register(s0);"

"struct PSInput"
"{"
"    float4 pos : SV_Position;"
"    float2 texCoord : TEXCOORD0;"
"};"

"float4 PSMain(PSInput input) : SV_Target"
"{"
"    float2 uv = input.texCoord;"
"    return tex_sampler.Sample(samp, uv);"
"}\0";