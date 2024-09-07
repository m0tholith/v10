#version 460 core

in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;
out vec4 FragColor;

void main()
{
    FragColor = vec4((vNormal + vec3(1.0)) / 2.0, 1.0);
}
