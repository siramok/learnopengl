#version 330 core

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 camera_position;

in vec3 fragment_position;
in vec3 normal;

out vec4 fragment_color;

void main()
{
    // ambient
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;

    // diffuse
    vec3 light_direction = normalize(light_position - fragment_position);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    // specular
    float specular_strength = 0.5;
    vec3 camera_direction = normalize(camera_position - fragment_position);
    vec3 reflect_direction = reflect(-light_direction, normal);
    vec3 specular = specular_strength * pow(max(dot(camera_direction, reflect_direction), 0.0), 32) * light_color;

    vec3 result = (ambient + diffuse + specular) * object_color;
    fragment_color = vec4(result, 1.0);
}