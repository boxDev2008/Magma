#pragma once

const char *OPENGL_WORLD_3D_FRAG = "#version 450\n"
"uniform sampler2D tex_sampler;"
"layout(location = 0) in vec2 frag_tex_coord;"
"layout(location = 0) out vec4 out_color;"

"void main()"
"{"
	"vec2 uv = frag_tex_coord.xy;"
	"out_color = texture(tex_sampler, uv);"
"}\0";