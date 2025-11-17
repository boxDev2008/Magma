@stage compute

layout(std430, binding = 0) buffer DataBuffer {
    uint data[];
};

layout(local_size_x = 16) in;

void main()
{
    uint idx = gl_GlobalInvocationID.x;
    data[idx] = idx;
}