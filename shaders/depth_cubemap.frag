#version 420 core

in vec4 FragPos;

uniform vec3 _lightPos;
uniform float _farPlane;

void main() {
    float lightDistance = length(FragPos.xyz - _lightPos);
    lightDistance = lightDistance / _farPlane;
    gl_FragDepth = lightDistance;
}
