#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texture_coords;

void main()
{
    texture_coords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
