#version 420 core

layout(location = 0) in vec3 vertPos;
layout(location = 4) in ivec4 vertBoneIDs;
layout(location = 5) in vec4 vertBoneWeights;

layout(std140, binding = 0) uniform WorldData {
    mat4 _projectionFromWorld;
    vec3 _cameraWorldPosition;
};

uniform mat4 _worldFromModel;
uniform mat4 _lightSpaceProjectionFromWorld;

#define MAX_BONES 100
uniform mat4 _boneTransformations[MAX_BONES];

void main() {
    vec4 position = vec4(0.0);
    for (int i = 0; i < 4; i++) {
        if (vertBoneIDs[i] == -1)
            break;
        position +=
            (_boneTransformations[vertBoneIDs[i]] * vec4(vertPos, 1.0)) *
            vertBoneWeights[i];
    }

    gl_Position = _lightSpaceProjectionFromWorld * _worldFromModel * position;
}
