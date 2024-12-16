#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/struct.h>

struct camera_data {
    vec3s position;
    versors quaternion;
    mat4s projectionFromViewMatrix;
    mat4s viewFromWorldMatrix;
};
typedef struct {
    struct camera_data _prev;
    vec3s Position;
    versors Quaternion;
    mat4s ProjectionFromViewMatrix;
    mat4s ViewFromWorldMatrix;
    uint32_t MatricesUBO;
} Camera;

Camera *cameraCreate(vec3s position, versors quaternion);
void cameraSetProjectionMatrixPersp(Camera *camera, float fov, float ratio,
                                    float nearPlane, float farPlane);
/// @param float x: the width of the viewport in world units
/// @param float y: the height of the viewport in world units
void cameraSetProjectionMatrixOrtho(Camera *camera, float x, float y,
                                    float nearPlane, float farPlane);
void cameraCalculateViewMatrix(Camera *camera);
void cameraSetPosition(Camera *camera, vec3s position);
void cameraLookAt(Camera *camera, vec3s target);
void cameraSetEulerAngles(Camera *camera, vec3s eulerAngles);
void cameraSetQuaternion(Camera *camera, versors quaternion);
void cameraPreRender(Camera *camera);
void cameraFree(Camera *camera);

#endif // !CAMERA_H
