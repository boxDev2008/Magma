@name present

@stage vertex

out vec2 frag_uv;

const vec2 positions[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0)
);

const vec2 uvs[4] = vec2[](
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    frag_uv = uvs[gl_VertexIndex];
}

@stage fragment

layout(binding = 0) uniform sampler2D tex0;

in vec2 frag_uv;
out vec4 out_color;

void main()
{
    out_color = texture(tex0, frag_uv);
}