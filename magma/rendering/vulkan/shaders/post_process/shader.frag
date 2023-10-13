#version 450

layout(binding = 0, set = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {

    vec2 uv = fragTexCoord.xy;
    outColor = texture(texSampler, uv);

    /*uv *=  1.0 - uv.yx;
    float vig = uv.x*uv.y * 15.0;
    vig = pow(vig, 0.25);
    outColor *= vec4(vig);*/
    
    /*vec3 hdrColor = texture(texSampler, fragTexCoord).rgb;
    const float gamma = 0.6;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * 5.0);
    mapped = pow(mapped, vec3(1.0 / gamma));
    outColor = vec4(mapped, 1.0) * vec4(vig);
    outColor = texture(texSampler, fragTexCoord) * vec4(vig);*/
}