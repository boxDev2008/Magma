#pragma once

const char HLSL_SPRITE_FRAG[] = 
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
"    return tex_sampler.Sample(samp, input.texCoord) * input.color;"
"}\0";