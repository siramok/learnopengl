#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cut_off;
    float outer_cut_off;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define N_POINT_LIGHTS 4

uniform vec3 camera_position;
uniform DirectionalLight directional_light;
uniform PointLight point_lights[N_POINT_LIGHTS];
uniform SpotLight spot_light;
uniform Material material;

in vec2 texture_coords;
in vec3 fragment_position;
in vec3 normal;

out vec4 fragment_color;

// functions
vec3 calculate_directional(DirectionalLight light, vec3 camera_direction);
vec3 calculate_point(PointLight light, vec3 camera_direction);
vec3 calculate_spot(SpotLight light, vec3 camera_direction);

void main()
{
    vec3 camera_direction = normalize(camera_position - fragment_position);

    // directional lights
    vec3 result = calculate_directional(directional_light, camera_direction);

    // point lights
    for (int i = 0; i < N_POINT_LIGHTS; i++)
    {
        result += calculate_point(point_lights[i], camera_direction);
    }

    // spot light
    result += calculate_spot(spot_light, camera_direction);

    fragment_color = vec4(result, 1.0);
}

vec3 calculate_directional(DirectionalLight light, vec3 camera_direction)
{
    vec3 light_direction = normalize(-light.direction);
    // diffuse
    float diff = max(dot(normal, light_direction), 0.0);
    // specular
    vec3 reflection_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(camera_position, reflection_direction), 0.0), material.shininess);
    // combine
    vec3 ambient = light.ambient * texture(material.diffuse, texture_coords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texture_coords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, texture_coords).rgb;
    return ambient + diffuse + specular;
}

vec3 calculate_point(PointLight light, vec3 camera_direction)
{
    vec3 light_direction = normalize(light.position - fragment_position);
    // diffuse
    float diff = max(dot(normal, light_direction), 0.0);
    // specular
    vec3 reflection_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragment_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine
    vec3 ambient = light.ambient * texture(material.diffuse, texture_coords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texture_coords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, texture_coords).rgb;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

vec3 calculate_spot(SpotLight light, vec3 camera_direction)
{
    vec3 light_direction = normalize(light.position - fragment_position);
    // diffuse
    float diff = max(dot(normal, light_direction), 0.0);
    // specular
    vec3 reflection_direction = reflect(-light_direction, normal);
    float spec = pow(max(dot(camera_direction, reflection_direction), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragment_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight
    float theta = dot(light_direction, normalize(-light.direction)); 
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);
    // combine
    vec3 ambient = light.ambient * texture(material.diffuse, texture_coords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texture_coords).rgb;
    vec3 specular = light.specular * spec * texture(material.specular, texture_coords).rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return ambient + diffuse + specular;
}
