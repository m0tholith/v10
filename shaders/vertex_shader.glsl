#version 460 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 vertColor;

out vec3 vColor;
out vec2 vTexCoord;

uniform mat4 projectionFromModel;

void main()
{
    vColor = vertColor;
    vTexCoord = vertTexCoord;

    gl_Position = projectionFromModel * vec4(vertPos, 1.0f);
}
