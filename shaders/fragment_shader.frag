#version 330 core

in vec3 normal;

out vec4 FragColor;

void main()
{
    FragColor.rgb = (normal + 1) / 2;
    FragColor.a = 1;
}
