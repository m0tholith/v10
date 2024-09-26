#version 330 core

in vec3 vColor;

out vec4 FragColor;

uniform vec3 diffuse;

void main()
{
    FragColor = vec4(diffuse, 1.0f);
}
