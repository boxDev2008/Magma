#pragma once

const char HLSL_MESH_VERT[] = 
/*"struct VSInput"
"{"
"    float3 position : INDEX0;"
"    float3 normal : INDEX1;"
"    float2 texCoord : INDEX2;"
"};"

"struct VSOutput"
"{"
"    float4 pos : SV_Position;"
"    float3 frag_position : POSITION;"
"    float3 frag_normal : NORMAL;"
"    float2 frag_tex_coord : TEXCOORD0;"
"    float4 frag_color : COLOR;"
"};"

"cbuffer UniformBufferObject : register(b0)"
"{"
"    float4x4 view_projection;"
"};"

"cbuffer PushConstants : register(b1)"
"{"
"    float4x4 model;"
"    float4 color;"
"};"

"VSOutput VSMain(VSInput input)"
"{"
"    VSOutput output;"
"    output.frag_tex_coord = input.texCoord;"
"    output.frag_color = color;"
"    return output;"
"}\0"*/
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
"    float3 position : INDEX0;"
"    float3 normal : INDEX1;"
"    float2 texCoord : INDEX2;"
"};"

"struct VSOutput"
"{"
"    float4 position : SV_Position;"
"    float3 normal : NORMAL;"
"    float2 tex_coord : TEXCOORD0;"
"    float4 color : COLOR;"
"};"

"VSOutput VSMain(VSInput input)"
"{"
"    VSOutput output;"
"    output.position = mul(view_projection, mul(model, float4(input.position, 1.0f)));"
"    output.tex_coord = input.texCoord;"
"    output.normal = input.normal;"
"    output.color = color;"
"    return output;"
"}\0";