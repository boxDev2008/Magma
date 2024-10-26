#pragma once

const char HLSL_QUAD_VERT[] = 
"cbuffer UniformBufferObject : register(b0)"
"{"
"    float4x4 view_projection;"
"};"

"cbuffer PushConstants : register(b1)"
"{"
"    float4x4 model;"
"    float4 color;"
"};"

"struct VSInput"
"{"
"    float2 position : INDEX0;"
"};"

"struct VSOutput"
"{"
"    float4 pos : SV_Position;"
"    float4 color : COLOR;"
"};"

"VSOutput VSMain(VSInput input)"
"{"
"    VSOutput output;"
"    output.pos = mul(view_projection, mul(model, float4(input.position, 0.0f, 1.0f)));"
"    output.color = color;"
"    return output;"
"}\0";