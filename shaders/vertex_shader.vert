#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 vertTexCoord;

out vec3 normal;
out vec2 texCoordinates;

uniform mat4 mvpMatrix;

void main()
{
    // (right to left)
    gl_Position = mvpMatrix * vec4(vertPos, 1.0f);
    normal = vertNormal;
    texCoordinates = vertTexCoord;
}
