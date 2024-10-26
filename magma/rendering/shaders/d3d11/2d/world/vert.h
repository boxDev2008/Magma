#pragma once

const char HLSL_WORLD_2D_VERT[] = 
"struct VSInput"
"{"
"    float2 position : INDEX0;"
"    float2 texCoord : INDEX1;"
"};"

"struct VSOutput"
"{"
"    float4 pos : SV_Position;"
"    float2 texCoord : TEXCOORD0;"
"};"

"VSOutput VSMain(VSInput input)"
"{"
"    VSOutput output;"
"    output.pos = float4(input.position, 0.0, 1.0);"
"    output.texCoord = input.texCoord;"
"    return output;"
"}\0";