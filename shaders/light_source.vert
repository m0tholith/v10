#version 420 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 vertColor;

out VS_OUT {
    vec3 Color;
} vs_out;

layout(std140, binding = 0) uniform WorldData {
    mat4 _projectionFromWorld;
    vec3 _cameraWorldPosition;
};
uniform mat4 _worldFromModel;

void main() {
    vs_out.Color = vertColor;

    mat4 _projectionFromModel = _projectionFromWorld * _worldFromModel;
    gl_Position = _projectionFromModel * vec4(vertPos, 1.0f);
}
