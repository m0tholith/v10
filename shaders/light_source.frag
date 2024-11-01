#version 420 core

in VS_OUT { vec3 Color; }
fs_in;

out vec4 FragColor;

uniform vec3 diffuse;

void main() { FragColor = vec4(diffuse * fs_in.Color, 1.0); }
