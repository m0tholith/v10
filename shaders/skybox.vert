#version 420 core

out vec3 TexCoords;

layout(location = 0) in vec3 vertPos;

layout(std140, binding = 0) uniform WorldData {
    mat4 _projectionFromWorld;
    vec3 _cameraWorldPosition;
};

void main() {
    TexCoords = vertPos;
    gl_Position = (_projectionFromWorld * vec4(vertPos + _cameraWorldPosition, 1.0)).xyww;
}
