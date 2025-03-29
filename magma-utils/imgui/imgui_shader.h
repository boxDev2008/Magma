#pragma once

#include <stdint.h>

static const uint32_t imgui_shader_spirv_vert[] = {
	0x07230203,0x00010000,0x0008000b,0x0000002f,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x000b000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000d,0x00000012,0x00000028,
	0x00000029,0x0000002b,0x0000002d,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,
	0x6e69616d,0x00000000,0x00060005,0x0000000b,0x505f6c67,0x65567265,0x78657472,0x00000000,
	0x00060006,0x0000000b,0x00000000,0x505f6c67,0x7469736f,0x006e6f69,0x00070006,0x0000000b,
	0x00000001,0x505f6c67,0x746e696f,0x657a6953,0x00000000,0x00070006,0x0000000b,0x00000002,
	0x435f6c67,0x4470696c,0x61747369,0x0065636e,0x00070006,0x0000000b,0x00000003,0x435f6c67,
	0x446c6c75,0x61747369,0x0065636e,0x00030005,0x0000000d,0x00000000,0x00050005,0x00000012,
	0x69736f70,0x6e6f6974,0x00000000,0x00070005,0x00000014,0x66696e55,0x426d726f,0x65666675,
	0x6a624f72,0x00746365,0x00060006,0x00000014,0x00000000,0x70736964,0x7a69735f,0x00000065,
	0x00030005,0x00000016,0x00000000,0x00030005,0x00000028,0x00007675,0x00050005,0x00000029,
	0x63786574,0x64726f6f,0x00000030,0x00040005,0x0000002b,0x6f6c6f63,0x00000072,0x00040005,
	0x0000002d,0x6f6c6f63,0x00003072,0x00050048,0x0000000b,0x00000000,0x0000000b,0x00000000,
	0x00050048,0x0000000b,0x00000001,0x0000000b,0x00000001,0x00050048,0x0000000b,0x00000002,
	0x0000000b,0x00000003,0x00050048,0x0000000b,0x00000003,0x0000000b,0x00000004,0x00030047,
	0x0000000b,0x00000002,0x00040047,0x00000012,0x0000001e,0x00000000,0x00050048,0x00000014,
	0x00000000,0x00000023,0x00000000,0x00030047,0x00000014,0x00000002,0x00040047,0x00000016,
	0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,0x00000000,0x00040047,0x00000028,
	0x0000001e,0x00000000,0x00040047,0x00000029,0x0000001e,0x00000001,0x00040047,0x0000002b,
	0x0000001e,0x00000001,0x00040047,0x0000002d,0x0000001e,0x00000002,0x00020013,0x00000002,
	0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,
	0x00000006,0x00000004,0x00040015,0x00000008,0x00000020,0x00000000,0x0004002b,0x00000008,
	0x00000009,0x00000001,0x0004001c,0x0000000a,0x00000006,0x00000009,0x0006001e,0x0000000b,
	0x00000007,0x00000006,0x0000000a,0x0000000a,0x00040020,0x0000000c,0x00000003,0x0000000b,
	0x0004003b,0x0000000c,0x0000000d,0x00000003,0x00040015,0x0000000e,0x00000020,0x00000001,
	0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040017,0x00000010,0x00000006,0x00000002,
	0x00040020,0x00000011,0x00000001,0x00000010,0x0004003b,0x00000011,0x00000012,0x00000001,
	0x0003001e,0x00000014,0x00000010,0x00040020,0x00000015,0x00000002,0x00000014,0x0004003b,
	0x00000015,0x00000016,0x00000002,0x00040020,0x00000017,0x00000002,0x00000010,0x0004002b,
	0x00000006,0x0000001b,0x3f000000,0x0004002b,0x00000006,0x0000001e,0x40000000,0x0005002c,
	0x00000010,0x0000001f,0x0000001e,0x0000001e,0x0004002b,0x00000006,0x00000021,0x3f800000,
	0x00040020,0x00000025,0x00000003,0x00000007,0x00040020,0x00000027,0x00000003,0x00000010,
	0x0004003b,0x00000027,0x00000028,0x00000003,0x0004003b,0x00000011,0x00000029,0x00000001,
	0x0004003b,0x00000025,0x0000002b,0x00000003,0x00040020,0x0000002c,0x00000001,0x00000007,
	0x0004003b,0x0000002c,0x0000002d,0x00000001,0x00050036,0x00000002,0x00000004,0x00000000,
	0x00000003,0x000200f8,0x00000005,0x0004003d,0x00000010,0x00000013,0x00000012,0x00050041,
	0x00000017,0x00000018,0x00000016,0x0000000f,0x0004003d,0x00000010,0x00000019,0x00000018,
	0x00050088,0x00000010,0x0000001a,0x00000013,0x00000019,0x00050050,0x00000010,0x0000001c,
	0x0000001b,0x0000001b,0x00050083,0x00000010,0x0000001d,0x0000001a,0x0000001c,0x00050085,
	0x00000010,0x00000020,0x0000001d,0x0000001f,0x00050051,0x00000006,0x00000022,0x00000020,
	0x00000000,0x00050051,0x00000006,0x00000023,0x00000020,0x00000001,0x00070050,0x00000007,
	0x00000024,0x00000022,0x00000023,0x0000001b,0x00000021,0x00050041,0x00000025,0x00000026,
	0x0000000d,0x0000000f,0x0003003e,0x00000026,0x00000024,0x0004003d,0x00000010,0x0000002a,
	0x00000029,0x0003003e,0x00000028,0x0000002a,0x0004003d,0x00000007,0x0000002e,0x0000002d,
	0x0003003e,0x0000002b,0x0000002e,0x000100fd,0x00010038
};

static const uint32_t imgui_shader_spirv_frag[] = {
	0x07230203,0x00010000,0x0008000b,0x00000018,0x00000000,0x00020011,0x00000001,0x0006000b,
	0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
	0x0008000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x00000011,0x00000015,
	0x00030010,0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,
	0x6e69616d,0x00000000,0x00050005,0x00000009,0x67617266,0x6c6f635f,0x0000726f,0x00030005,
	0x0000000d,0x00786574,0x00030005,0x00000011,0x00007675,0x00040005,0x00000015,0x6f6c6f63,
	0x00000072,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x00000022,
	0x00000001,0x00040047,0x0000000d,0x00000021,0x00000000,0x00040047,0x00000011,0x0000001e,
	0x00000000,0x00040047,0x00000015,0x0000001e,0x00000001,0x00020013,0x00000002,0x00030021,
	0x00000003,0x00000002,0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,
	0x00000004,0x00040020,0x00000008,0x00000003,0x00000007,0x0004003b,0x00000008,0x00000009,
	0x00000003,0x00090019,0x0000000a,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
	0x00000001,0x00000000,0x0003001b,0x0000000b,0x0000000a,0x00040020,0x0000000c,0x00000000,
	0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000000,0x00040017,0x0000000f,0x00000006,
	0x00000002,0x00040020,0x00000010,0x00000001,0x0000000f,0x0004003b,0x00000010,0x00000011,
	0x00000001,0x00040020,0x00000014,0x00000001,0x00000007,0x0004003b,0x00000014,0x00000015,
	0x00000001,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,
	0x0004003d,0x0000000b,0x0000000e,0x0000000d,0x0004003d,0x0000000f,0x00000012,0x00000011,
	0x00050057,0x00000007,0x00000013,0x0000000e,0x00000012,0x0004003d,0x00000007,0x00000016,
	0x00000015,0x00050085,0x00000007,0x00000017,0x00000013,0x00000016,0x0003003e,0x00000009,
	0x00000017,0x000100fd,0x00010038
};

/*
#version 450

layout(binding = 0, std140) uniform UniformBufferObject
{
    vec2 disp_size;
} _22;

layout(location = 0) in vec2 position;
layout(location = 0) out vec2 uv;
layout(location = 1) in vec2 texcoord0;
layout(location = 1) out vec4 color;
layout(location = 2) in vec4 color0;

void main()
{
    gl_Position = vec4(((position / _22.disp_size) - vec2(0.5)) * vec2(2.0), 0.5, 1.0);
    uv = texcoord0;
    color = color0;
}

*/
static const uint8_t imgui_shader_glsl_vert[] = {
	0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x34,0x35,0x30,0x0a,0x0a,0x6c,0x61,
	0x79,0x6f,0x75,0x74,0x28,0x62,0x69,0x6e,0x64,0x69,0x6e,0x67,0x20,0x3d,0x20,0x30,
	0x2c,0x20,0x73,0x74,0x64,0x31,0x34,0x30,0x29,0x20,0x75,0x6e,0x69,0x66,0x6f,0x72,
	0x6d,0x20,0x55,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x42,0x75,0x66,0x66,0x65,0x72,0x4f,
	0x62,0x6a,0x65,0x63,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x76,0x65,0x63,0x32,
	0x20,0x64,0x69,0x73,0x70,0x5f,0x73,0x69,0x7a,0x65,0x3b,0x0a,0x7d,0x20,0x5f,0x32,
	0x32,0x3b,0x0a,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,
	0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,
	0x20,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,
	0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,
	0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x3b,0x0a,0x6c,0x61,0x79,
	0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x31,
	0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x74,0x65,0x78,0x63,0x6f,0x6f,
	0x72,0x64,0x30,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,
	0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x31,0x29,0x20,0x6f,0x75,0x74,0x20,0x76,0x65,
	0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,
	0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x32,0x29,0x20,0x69,
	0x6e,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,0x0a,0x0a,
	0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,
	0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,
	0x76,0x65,0x63,0x34,0x28,0x28,0x28,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,
	0x2f,0x20,0x5f,0x32,0x32,0x2e,0x64,0x69,0x73,0x70,0x5f,0x73,0x69,0x7a,0x65,0x29,
	0x20,0x2d,0x20,0x76,0x65,0x63,0x32,0x28,0x30,0x2e,0x35,0x29,0x29,0x20,0x2a,0x20,
	0x76,0x65,0x63,0x32,0x28,0x32,0x2e,0x30,0x29,0x2c,0x20,0x30,0x2e,0x35,0x2c,0x20,
	0x31,0x2e,0x30,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x20,0x3d,0x20,0x74,
	0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,
	0x6c,0x6f,0x72,0x20,0x3d,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,0x0a,0x7d,0x0a,
	0x0a
};

/*
#version 450

layout(binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 frag_color;
layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 color;

void main()
{
    frag_color = texture(tex, uv) * color;
}

*/
static const uint8_t imgui_shader_glsl_frag[] = {
	0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x34,0x35,0x30,0x0a,0x0a,0x6c,0x61,
	0x79,0x6f,0x75,0x74,0x28,0x62,0x69,0x6e,0x64,0x69,0x6e,0x67,0x20,0x3d,0x20,0x30,
	0x29,0x20,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,
	0x72,0x32,0x44,0x20,0x74,0x65,0x78,0x3b,0x0a,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,
	0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x6f,
	0x75,0x74,0x20,0x76,0x65,0x63,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,
	0x6f,0x72,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,
	0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,
	0x20,0x75,0x76,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,
	0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x31,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,
	0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,
	0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x72,0x61,0x67,
	0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x74,0x65,0x78,0x74,0x75,0x72,0x65,
	0x28,0x74,0x65,0x78,0x2c,0x20,0x75,0x76,0x29,0x20,0x2a,0x20,0x63,0x6f,0x6c,0x6f,
	0x72,0x3b,0x0a,0x7d,0x0a,0x0a
};

/*
cbuffer UniformBufferObject : register(b0)
{
    float2 _22_disp_size : packoffset(c0);
};


static float4 gl_Position;
static float2 position;
static float2 uv;
static float2 texcoord0;
static float4 color;
static float4 color0;

struct SPIRV_Cross_Input
{
    float2 position : TEXCOORD0;
    float2 texcoord0 : TEXCOORD1;
    float4 color0 : TEXCOORD2;
};

struct SPIRV_Cross_Output
{
    float2 uv : TEXCOORD0;
    float4 color : TEXCOORD1;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    gl_Position = float4(((position / _22_disp_size) - 0.5f.xx) * 2.0f.xx, 0.5f, 1.0f);
    uv = texcoord0;
    color = color0;
    gl_Position.y = -gl_Position.y;
    gl_Position.z = (gl_Position.z + gl_Position.w) * 0.5;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    position = stage_input.position;
    texcoord0 = stage_input.texcoord0;
    color0 = stage_input.color0;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.uv = uv;
    stage_output.color = color;
    return stage_output;
}
*/
static const uint8_t imgui_shader_hlsl_vert[] = {
	0x63,0x62,0x75,0x66,0x66,0x65,0x72,0x20,0x55,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x42,
	0x75,0x66,0x66,0x65,0x72,0x4f,0x62,0x6a,0x65,0x63,0x74,0x20,0x3a,0x20,0x72,0x65,
	0x67,0x69,0x73,0x74,0x65,0x72,0x28,0x62,0x30,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,
	0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x5f,0x32,0x32,0x5f,0x64,0x69,0x73,0x70,
	0x5f,0x73,0x69,0x7a,0x65,0x20,0x3a,0x20,0x70,0x61,0x63,0x6b,0x6f,0x66,0x66,0x73,
	0x65,0x74,0x28,0x63,0x30,0x29,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x0a,0x73,0x74,0x61,
	0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,0x5f,0x50,0x6f,
	0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,
	0x6c,0x6f,0x61,0x74,0x32,0x20,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,
	0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x75,0x76,
	0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,
	0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,
	0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,
	0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,
	0x6c,0x6f,0x72,0x30,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,
	0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,0x74,0x0a,
	0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x70,0x6f,0x73,
	0x69,0x74,0x69,0x6f,0x6e,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,
	0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x74,0x65,
	0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,
	0x52,0x44,0x31,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,
	0x63,0x6f,0x6c,0x6f,0x72,0x30,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,
	0x44,0x32,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,
	0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,
	0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x75,
	0x76,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x30,0x3b,0x0a,0x20,
	0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,
	0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x31,0x3b,0x0a,0x20,0x20,0x20,
	0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,
	0x69,0x6f,0x6e,0x20,0x3a,0x20,0x53,0x56,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,
	0x6e,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x76,0x65,0x72,0x74,
	0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,
	0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x66,0x6c,0x6f,0x61,
	0x74,0x34,0x28,0x28,0x28,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x2f,0x20,
	0x5f,0x32,0x32,0x5f,0x64,0x69,0x73,0x70,0x5f,0x73,0x69,0x7a,0x65,0x29,0x20,0x2d,
	0x20,0x30,0x2e,0x35,0x66,0x2e,0x78,0x78,0x29,0x20,0x2a,0x20,0x32,0x2e,0x30,0x66,
	0x2e,0x78,0x78,0x2c,0x20,0x30,0x2e,0x35,0x66,0x2c,0x20,0x31,0x2e,0x30,0x66,0x29,
	0x3b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x20,0x3d,0x20,0x74,0x65,0x78,0x63,0x6f,
	0x6f,0x72,0x64,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,
	0x3d,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,
	0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2e,0x79,0x20,0x3d,0x20,0x2d,0x67,
	0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2e,0x79,0x3b,0x0a,0x20,0x20,
	0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2e,0x7a,0x20,
	0x3d,0x20,0x28,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2e,0x7a,
	0x20,0x2b,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2e,0x77,
	0x29,0x20,0x2a,0x20,0x30,0x2e,0x35,0x3b,0x0a,0x7d,0x0a,0x0a,0x53,0x50,0x49,0x52,
	0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x6d,
	0x61,0x69,0x6e,0x28,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,
	0x49,0x6e,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,
	0x74,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,
	0x6e,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,
	0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x20,0x20,0x20,0x20,0x74,0x65,
	0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,
	0x69,0x6e,0x70,0x75,0x74,0x2e,0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,
	0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x20,0x3d,0x20,0x73,0x74,
	0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x63,0x6f,0x6c,0x6f,0x72,0x30,
	0x3b,0x0a,0x20,0x20,0x20,0x20,0x76,0x65,0x72,0x74,0x5f,0x6d,0x61,0x69,0x6e,0x28,
	0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,
	0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,
	0x6f,0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,
	0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,
	0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,
	0x6f,0x6e,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,
	0x74,0x70,0x75,0x74,0x2e,0x75,0x76,0x20,0x3d,0x20,0x75,0x76,0x3b,0x0a,0x20,0x20,
	0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x63,
	0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,
	0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,
	0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x7d,0x0a
};

/*
Texture2D<float4> tex : register(t0);
SamplerState _tex_sampler : register(s0);

static float4 frag_color;
static float2 uv;
static float4 color;

struct SPIRV_Cross_Input
{
    float2 uv : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct SPIRV_Cross_Output
{
    float4 frag_color : SV_Target0;
};

void frag_main()
{
    frag_color = tex.Sample(_tex_sampler, uv) * color;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    uv = stage_input.uv;
    color = stage_input.color;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.frag_color = frag_color;
    return stage_output;
}
*/
static const uint8_t imgui_shader_hlsl_frag[] = {
	0x54,0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x44,0x3c,0x66,0x6c,0x6f,0x61,0x74,0x34,
	0x3e,0x20,0x74,0x65,0x78,0x20,0x3a,0x20,0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,
	0x28,0x74,0x30,0x29,0x3b,0x0a,0x53,0x61,0x6d,0x70,0x6c,0x65,0x72,0x53,0x74,0x61,
	0x74,0x65,0x20,0x5f,0x74,0x65,0x78,0x5f,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x20,
	0x3a,0x20,0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,0x28,0x73,0x30,0x29,0x3b,0x0a,
	0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x66,
	0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,
	0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x75,0x76,0x3b,0x0a,0x73,0x74,0x61,
	0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,
	0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,
	0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,
	0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x75,0x76,0x20,0x3a,0x20,0x54,0x45,
	0x58,0x43,0x4f,0x4f,0x52,0x44,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,
	0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,
	0x4f,0x4f,0x52,0x44,0x31,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,
	0x74,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,
	0x74,0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,
	0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3a,0x20,0x53,
	0x56,0x5f,0x54,0x61,0x72,0x67,0x65,0x74,0x30,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x76,
	0x6f,0x69,0x64,0x20,0x66,0x72,0x61,0x67,0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,
	0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,
	0x20,0x3d,0x20,0x74,0x65,0x78,0x2e,0x53,0x61,0x6d,0x70,0x6c,0x65,0x28,0x5f,0x74,
	0x65,0x78,0x5f,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x2c,0x20,0x75,0x76,0x29,0x20,
	0x2a,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x7d,0x0a,0x0a,0x53,0x50,0x49,0x52,
	0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x6d,
	0x61,0x69,0x6e,0x28,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,
	0x49,0x6e,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,
	0x74,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x20,0x3d,0x20,0x73,0x74,
	0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x75,0x76,0x3b,0x0a,0x20,0x20,
	0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,
	0x69,0x6e,0x70,0x75,0x74,0x2e,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,
	0x20,0x66,0x72,0x61,0x67,0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,0x3b,0x0a,0x20,0x20,
	0x20,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,
	0x74,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,
	0x74,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,
	0x70,0x75,0x74,0x2e,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,
	0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,
	0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,
	0x74,0x70,0x75,0x74,0x3b,0x0a,0x7d,0x0a
};

static inline const mg_shader get_imgui_shader(mg_renderer_type type) {
    switch (type) {
        case MG_RENDERER_TYPE_VULKAN:
            return {
                .vertex = {
                    .code = (void*)imgui_shader_spirv_vert,
                    .size = sizeof(imgui_shader_spirv_vert)
                },
                .fragment = {
                    .code = (void*)imgui_shader_spirv_frag,
                    .size = sizeof(imgui_shader_spirv_frag)
                },
            };
        case MG_RENDERER_TYPE_OPENGL:
            return {
                .vertex = {
                    .code = (void*)imgui_shader_glsl_vert,
                    .size = sizeof(imgui_shader_glsl_vert)
                },
                .fragment = {
                    .code = (void*)imgui_shader_glsl_frag,
                    .size = sizeof(imgui_shader_glsl_frag)
                },
            };
        case MG_RENDERER_TYPE_DIRECT3D11:
            return {
                .vertex = {
                    .code = (void*)imgui_shader_hlsl_vert,
                    .size = sizeof(imgui_shader_hlsl_vert)
                },
                .fragment = {
                    .code = (void*)imgui_shader_hlsl_frag,
                    .size = sizeof(imgui_shader_hlsl_frag)
                },
            };
    };
    return {};
}

