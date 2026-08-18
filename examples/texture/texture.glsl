@name texture

@stage vertex

in vec2 pos;
in vec2 uv;

out vec2 frag_uv;

void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
    frag_uv = uv;
}

@stage fragment

layout(binding = 0) uniform sampler2D tex0;

in vec2 frag_uv;
out vec4 out_color;

void main()
{
    out_color = texture(tex0, frag_uv);
}