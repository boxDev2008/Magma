#pragma once

const char *OPENGL_SPRITE_FRAG = "#version 450\n"

"uniform sampler2D tex_sampler;"

"layout(location = 0) in vec2 frag_tex_coord;"
"layout(location = 1) in vec4 frag_color;"

"layout(location = 0) out vec4 out_color;"

"void main()"
"{"
	"out_color = texture(tex_sampler, frag_tex_coord) * frag_color;"
"}\0";