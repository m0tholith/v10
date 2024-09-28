#version 420 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 vertColor;

out vec3 vColor;

layout(std140, binding = 0) uniform WorldData {
    mat4 _projectionFromWorld;
    vec3 _cameraWorldPosition;
};
uniform mat4 _worldFromModel;
uniform mat3 _worldNormalFromModel;

void main() {
    vColor = vertColor;

    mat4 _projectionFromModel = _projectionFromWorld * _worldFromModel;
    gl_Position = _projectionFromModel * vec4(vertPos, 1.0f);
}
