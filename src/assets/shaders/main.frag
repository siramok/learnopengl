#version 330 core

uniform sampler2D texture_diffuse1;

in vec2 texture_coords;

out vec4 fragment_color;

void main()
{
    fragment_color = texture(texture_diffuse1, texture_coords);
}
