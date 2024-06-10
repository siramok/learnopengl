#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 object_color;
uniform vec3 camera_position;

in vec2 texture_coords;
in vec3 fragment_position;
in vec3 normal;

out vec4 fragment_color;

void main()
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texture_coords));

    // diffuse
    vec3 light_direction = normalize(light.position - fragment_position);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texture_coords));

    // specular
    vec3 camera_direction = normalize(camera_position - fragment_position);
    vec3 reflect_direction = reflect(-light_direction, normal);
    vec3 specular = light.specular * pow(max(dot(camera_direction, reflect_direction), 0.01), material.shininess) * vec3(texture(material.specular, texture_coords));

    fragment_color = vec4(ambient + diffuse + specular, 1.0);
}