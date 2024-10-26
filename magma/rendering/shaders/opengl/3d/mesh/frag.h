#pragma once

const char *OPENGL_MESH_FRAG = "#version 450\n"

"layout(binding = 0) uniform sampler2D tex_sampler;"

"layout(location = 0) in vec3 frag_position;"
"layout(location = 1) in vec3 frag_normal;"
"layout(location = 2) in vec2 frag_tex_coord;"
"layout(location = 3) in vec4 frag_color;"

"layout(location = 0) out vec4 out_color;"

"void main()"
"{"
	"out_color = texture(tex_sampler, frag_tex_coord) * frag_color;"
"}\0";