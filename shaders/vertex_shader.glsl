#version 460 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 vertColor;

out vec3 vColor;
out vec2 vTexCoord;
out vec3 vNormal;

uniform mat4 projectionFromModel;

const float strength = 20;
vec4 vertex_warp(vec4 pos) {
    pos.xy = (pos.xy + vec2(1.0)) * vec2(320.0 / strength, 240.0 / strength) * 0.5;
    pos.xy = round(pos.xy);
    pos.xy = pos.xy * 2 / vec2(320.0 / strength, 240.0 / strength) - vec2(1.0, 1.0);
    return pos;
}

void main() {
    vColor = vertColor;
    vTexCoord = vertTexCoord;
    vNormal = vertNormal;

    gl_Position = vertex_warp(projectionFromModel * vec4(vertPos, 1.0f));
}
