#version 450

layout(binding = 0, set = 1) uniform sampler2D tex_sampler;

layout(location = 0) in vec3 frag_position;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec2 frag_tex_coord;
layout(location = 3) in vec4 frag_color;

layout(location = 0) out vec4 out_color;

/*vec3 light_color = vec3(
    1.0, 1.0, 1.0
);

vec3 light_position = vec3(
    4.0, 0.0, -4.0
);

vec3 view_position = vec3(
    0.0, 0.0, 1.0
);

vec3 calculate_lighting(vec3 norm, vec3 view_dir)
{
    float ambient_strength = 0.5;
    vec3 ambient = ambient_strength * light_color;

    vec3 light_dir = normalize(light_position - frag_position);
    
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    float specular_strength = 1.0;
    vec3 reflect_dir = reflect(-light_dir, norm);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;  

    return ambient + diffuse + specular;
}*/

void main()
{
    /*vec3 norm = normalize(frag_normal);
    vec3 view_dir = normalize(view_position - frag_position);

    vec3 result = calculate_lighting(norm, view_dir);*/
    // result += calculate_lighting(norm, view_dir); for point lights

    out_color = texture(tex_sampler, frag_tex_coord) /** vec4(result, 1.0)*/ * frag_color;
}