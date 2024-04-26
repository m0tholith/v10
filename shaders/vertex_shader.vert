#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 0) in vec3 vertNormal;
layout (location = 2) in vec2 vertTexCoord;

out vec3 normal;
out vec2 texCoordinates;

uniform mat4 transform;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    // (right to left)
    gl_Position = projectionMatrix * viewMatrix * transform * vec4(vertPos, 1.0f);
    normal = vertNormal;
    texCoordinates = vertTexCoord;
}
