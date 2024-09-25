#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 vertColor;
layout(location = 4) in ivec4 vertBoneIDs;
layout(location = 5) in vec4 vertBoneWeights;

out vec3 vColor;
out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vPos;

uniform mat4 _projectionFromModel;
uniform mat4 _worldFromModel;
uniform mat3 _worldNormalFromModel;

#define MAX_BONES 100

uniform mat4 _boneTransformations[MAX_BONES];

const float strength = 5;
vec4 vertex_warp(vec4 pos) {
    pos.xy = (pos.xy + vec2(1.0)) * vec2(320.0 / strength, 240.0 / strength) * 0.5;
    pos.xy = round(pos.xy);
    pos.xy = pos.xy * 2 / vec2(320.0 / strength, 240.0 / strength) - vec2(1.0, 1.0);
    return pos;
}

void main() {
    vec4 position = vec4(0.0f);
    vec3 normal = vec3(0.0f);
    for (int i = 0; i < 4; i++) {
        if (vertBoneIDs[i] == -1)
            break;
        position += (_boneTransformations[vertBoneIDs[i]] * vec4(vertPos, 1.0f)) * vertBoneWeights[i];
        normal += (mat3(_boneTransformations[vertBoneIDs[i]]) * vertNormal) * vertBoneWeights[i];
    }

    vColor = vertColor;
    vTexCoord = vertTexCoord;
    vNormal = normalize(_worldNormalFromModel * normal);
    vPos = vec3(_worldFromModel * position);

    gl_Position = _projectionFromModel * position;
}
