#version 420 core

layout(location = 0) in vec3 vertPos;

layout(std140, binding = 0) uniform WorldData {
    mat4 _projectionFromWorld;
    vec3 _cameraWorldPosition;
};

uniform mat4 _worldFromModel;
uniform mat4 _lightSpaceProjectionFromWorld;

void main() {
    gl_Position =
        _lightSpaceProjectionFromWorld * _worldFromModel * vec4(vertPos, 1.0);
}
