#version 420 core

in vec3 vColor;

out vec4 FragColor;

uniform vec3 diffuse;

void main()
{
    FragColor = vec4(diffuse * vColor, 1.0f);
}
