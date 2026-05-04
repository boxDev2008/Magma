@stage vertex

in vec2 pos;
in vec3 color;

out vec3 frag_color;

void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
    frag_color = color;
}

@stage fragment

in vec3 frag_color;
out vec4 out_color;

void main()
{
    out_color = vec4(frag_color, 1.0);
}