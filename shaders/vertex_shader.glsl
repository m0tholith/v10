#version 330 core

const int MAX_BONES = 20;
const int MAX_BONE_INFLUENCE = 4;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 vertColor;
layout(location = 4) in float boneIDs[MAX_BONE_INFLUENCE];
layout(location = 5) in float weights[MAX_BONE_INFLUENCE];

out vec3 vColor;
out vec2 vTexCoord;

uniform mat4 boneFromRoot[MAX_BONES];
uniform mat4 projectionFromModel;

void main()
{
    vColor = vertColor;
    vTexCoord = vertTexCoord;

    vec4 totalPosition = vec4(0.0f);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIDs[i] == -1)
        {
            vColor = vec3(1, 0, 0);
            continue;
        }
        if (boneIDs[i] >= MAX_BONES)
        {
            vColor = vec3(0, 0, 1);
            totalPosition = vec4(vertPos, 1.0f);
            break;
        }
        vec4 localPosition = boneFromRoot[int(boneIDs[i])] * vec4(vertPos, 1.0f);
        totalPosition += localPosition * weights[i];
        vColor = vec3(0, 1, 0);
    }

    gl_Position = projectionFromModel * totalPosition;
}
