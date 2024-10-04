#version 420 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 vertColor;
layout(location = 4) in ivec4 vertBoneIDs;
layout(location = 5) in vec4 vertBoneWeights;

out VS_OUT {
    vec3 Color;
    vec2 TexCoord;
    vec3 Normal;
    vec3 Pos;
    vec4 LightSpacePos;
} vs_out;

layout(std140, binding = 0) uniform WorldData {
    mat4 _projectionFromWorld;
    mat4 _lightSpaceProjectionFromWorld;
    vec3 _cameraWorldPosition;
};
uniform mat4 _worldFromModel;

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
    mat4 _projectionFromModel = _projectionFromWorld * _worldFromModel;
    mat3 _worldNormalFromModel = transpose(inverse(mat3(_worldFromModel)));
    vec4 position = vec4(0.0f);
    vec3 normal = vec3(0.0f);
    for (int i = 0; i < 4; i++) {
        if (vertBoneIDs[i] == -1)
            break;
        position += (_boneTransformations[vertBoneIDs[i]] * vec4(vertPos, 1.0f)) * vertBoneWeights[i];
        normal += (mat3(_boneTransformations[vertBoneIDs[i]]) * vertNormal) * vertBoneWeights[i];
    }

    vs_out.Color = vertColor;
    vs_out.TexCoord = vertTexCoord;
    vs_out.Normal = normalize(_worldNormalFromModel * normal);
    vs_out.Pos = vec3(_worldFromModel * position);
    vs_out.LightSpacePos = _lightSpaceProjectionFromWorld * vec4(vs_out.Pos, 1.0f);

    gl_Position = _projectionFromModel * position;
}
