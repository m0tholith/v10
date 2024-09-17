#version 460 core

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
    mat4 transformation = mat4(1.0f);
    for (int i = 0; i < 4; i++) {
        if (vertBoneIDs[i] == -1)
            break;
        if (vertBoneIDs[i] >= MAX_BONES)
            vColor = vec3(1.0f, 0.0f, 0.862f);
        transformation += _boneTransformations[vertBoneIDs[i]] * vertBoneWeights[i];
    }

    vec4 position = transformation * vec4(vertPos, 1.0f);
    vec3 normal = vec3(transformation * vec4(vertNormal, 1.0f));

    vColor = vertColor;
    vTexCoord = vertTexCoord;
    vNormal = normalize(_worldNormalFromModel * normal);
    vPos = vec3(_worldFromModel * position);

    gl_Position = vertex_warp(_projectionFromModel * position);
}
