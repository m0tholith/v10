#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTexCoord;
layout (location = 3) in vec3 vertColor;

out vec3 color;

uniform mat4 mvpMatrix;

void main()
{
    // (right to left)
    gl_Position = mvpMatrix * vec4(vertPos, 1.0f);

    color = vertColor;
}
