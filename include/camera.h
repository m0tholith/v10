#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/struct.h>

typedef struct {
    vec3s Position;
    versors Quaternion;
    mat4s ViewFromWorldMatrix;
} Camera;

Camera cameraCreate(vec3s position, versors quaternion);
void cameraSetProjectionMatrixPersp(Camera *camera, float fov, float nearPlane,
                                    float farPlane);
/// @param float x: the width of the viewport in world units
/// @param float y: the height of the viewport in world units
void cameraSetProjectionMatrixOrtho(Camera *camera, float x, float y,
                                    float nearPlane, float farPlane);
void cameraCalculateViewMatrix(Camera *camera);
void cameraSetPosition(Camera *camera, vec3s position);
void cameraLookAt(Camera *camera, vec3s target);
void cameraSetEulerAngles(Camera *camera, vec3s eulerAngles);
void cameraSetQuaternion(Camera *camera, versors quaternion);

#endif // !CAMERA_H
