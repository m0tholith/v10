#version 420 core

layout(location = 0) in vec3 vertPos;

uniform mat4 _worldFromModel;

void main() { gl_Position = _worldFromModel * vec4(vertPos, 1.0); }
