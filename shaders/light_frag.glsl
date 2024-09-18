#version 460 core

in vec3 vColor;

out vec4 FragColor;

uniform vec3 light_color;

void main()
{
    FragColor = vec4(light_color * vColor, 1.0f);
}
