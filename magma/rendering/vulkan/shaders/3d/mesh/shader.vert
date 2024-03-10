#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coord;

layout(location = 0) out vec3 frag_position;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out vec2 frag_tex_coord;
layout(location = 3) out vec4 frag_color;

layout(binding = 0) uniform UniformBufferObject
{
    mat4 view_projection;
}
ubo;

layout(push_constant) uniform PushConstants
{
	mat4 model;
    vec4 color;
}
push;

void main()
{
    gl_Position = ubo.view_projection * push.model * vec4(in_position, 1.0);
    frag_position = vec3(push.model * vec4(in_position, 1.0));
    frag_normal = vec3(mat3(transpose(inverse(push.model))) * in_normal);
    frag_tex_coord = in_tex_coord;
    frag_color = push.color;
}