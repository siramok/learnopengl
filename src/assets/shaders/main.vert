#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texture_coords;
out vec3 fragment_position;
out vec3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texture_coords = aTexCoords;
    fragment_position = vec3(model * vec4(aPos, 1.0));
    normal = normalize(mat3(transpose(inverse(view * model))) * aNormal);
}