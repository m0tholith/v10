#version 330 core

out vec4 FragColor;

uniform vec3 tint;

void main()
{
    FragColor.rgb = tint;
    FragColor.a = 1;
}
