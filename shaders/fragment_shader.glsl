#version 460 core

in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPos;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0f);
}
