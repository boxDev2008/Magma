@stage vertex

layout(binding=0) uniform UniformBufferObject
{
    vec2 disp_size;
};

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord0;
layout(location=2) in vec4 color0;

out vec2 uv;
out vec4 color;

void main()
{
    gl_Position = vec4(((position/disp_size)-0.5)*vec2(2.0,2.0), 0.5, 1.0);
    uv = texcoord0;
    color = color0;
}

@stage fragment

uniform sampler2D tex;

in vec2 uv;
in vec4 color;
out vec4 frag_color;

void main()
{
    frag_color = texture(tex, uv) * color;
}