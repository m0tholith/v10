#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/include/cglm/struct.h"

typedef struct Camera {
    vec3s Position;
    vec3s Forward;
    vec3s Right;
    vec3s Up;
    mat4s ViewMatrix;
} Camera;

Camera cameraCreate(vec3s position, vec3s target);
void cameraCalculateViewMatrix(Camera *camera);
void cameraMove(Camera *camera, vec3s newPosition);
void cameraLookAt(Camera *camera, vec3s target);

#endif
