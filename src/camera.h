#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/include/cglm/struct.h"

typedef struct Camera {
    vec3s Position;
    versors Quaternion;
    mat4s ViewMatrix;
} Camera;

Camera cameraCreate(vec3s position, versors quaternion);
void cameraCalculateViewMatrix(Camera *camera);
void cameraSetPosition(Camera *camera, vec3s position);
void cameraLookAt(Camera *camera, vec3s target);
void cameraSetEulerAngles(Camera *camera, vec3s eulerAngles);
void cameraSetQuaternion(Camera *camera, versors quaternion);

#endif // !CAMERA_H
