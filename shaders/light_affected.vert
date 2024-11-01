#version 420 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 vertColor;

out VS_OUT {
    vec3 Color;
    vec2 TexCoord;
    vec3 Normal;
    vec3 Pos;
} vs_out;

layout(std140, binding = 0) uniform WorldData {
    mat4 _projectionFromWorld;
    vec3 _cameraWorldPosition;
};
uniform mat4 _worldFromModel;

const float strength = 5;
vec4 vertex_warp(vec4 pos) {
    pos.xy = (pos.xy + vec2(1.0)) * vec2(320.0 / strength, 240.0 / strength) * 0.5;
    pos.xy = round(pos.xy);
    pos.xy = pos.xy * 2 / vec2(320.0 / strength, 240.0 / strength) - vec2(1.0, 1.0);
    return pos;
}

void main() {
    mat3 _worldNormalFromModel = transpose(inverse(mat3(_worldFromModel)));

    vs_out.Color = vertColor;
    vs_out.TexCoord = vertTexCoord;
    vs_out.Normal = normalize(_worldNormalFromModel * vertNormal);
    vs_out.Pos = vec3(_worldFromModel * vec4(vertPos, 1.0f));

    mat4 _projectionFromModel = _projectionFromWorld * _worldFromModel;
    gl_Position = _projectionFromModel * vec4(vertPos, 1.0f);
}
