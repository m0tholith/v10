#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/struct.h>

typedef struct {
    vec3s Position;
    versors Quaternion;
    mat4s ViewFromWorldMatrix;
} Camera;

Camera cameraCreate(vec3s position, versors quaternion);
void cameraCalculateViewMatrix(Camera *camera);
void cameraSetPosition(Camera *camera, vec3s position);
void cameraLookAt(Camera *camera, vec3s target);
void cameraSetEulerAngles(Camera *camera, vec3s eulerAngles);
void cameraSetQuaternion(Camera *camera, versors quaternion);

#endif // !CAMERA_H
