#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertTexCoord;

out vec2 texCoordinates;

uniform mat4 transform;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    // (right to left)
    gl_Position = projectionMatrix * viewMatrix * transform * vec4(vertPos, 1.0f);
    texCoordinates = vertTexCoord;
}
