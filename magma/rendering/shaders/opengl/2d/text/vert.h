#pragma once

const char *OPENGL_TEXT_VERT = "#version 450\n"

"layout(location = 0) in vec2 in_position;"
"layout(location = 1) in vec2 in_tex_coord;"

"layout(location = 0) out vec2 frag_tex_coord;"
"layout(location = 1) out vec4 frag_color;"

"layout(std140, binding = 0) uniform UniformBufferObject"
"{"
	"mat4 view_projection;"
"}"
"ubo;"

"layout(std140, binding = 1) uniform PushConstants"
"{"
	"mat4 model;"
	"vec4 color;"
"}"
"push;"

"void main()"
"{"
	"gl_Position = ubo.view_projection * push.model * vec4(in_position, 0.0, 1.0);"
	"frag_tex_coord = in_tex_coord;"
	"frag_color = push.color;"
"}\0";