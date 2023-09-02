#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UniformBufferObject {
    float time;
} ubo;

void main() {
    gl_Position = vec4(inPosition.x + ubo.time, inPosition.y, 0.0, 1.0);
    fragColor = vec3(0.6 + ubo.time, 1.0, 0.6);
}