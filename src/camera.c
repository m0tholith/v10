#include "camera.h"

#define X_AXIS ((vec3s){{1.0f, 0.0f, 0.0f}})
#define Y_AXIS ((vec3s){{0.0f, 1.0f, 0.0f}})
#define Z_AXIS ((vec3s){{0.0f, 0.0f, 1.0f}})

Camera cameraCreate(vec3s position, versors quaternion) {
    Camera camera = (Camera){
        .Position = position,
        .Quaternion = quaternion,
    };
    cameraCalculateViewMatrix(&camera);
    return camera;
}
void cameraCalculateViewMatrix(Camera *camera) {
    camera->ViewMatrix = glms_quat_look(camera->Position, camera->Quaternion);
}
void cameraSetPosition(Camera *camera, vec3s position) {
    camera->Position = position;
}
void cameraLookAt(Camera *camera, vec3s target) {
    camera->Quaternion = glms_quat_forp(camera->Position, target, Y_AXIS);
}
void cameraSetEulerAngles(Camera *camera, vec3s eulerAngles) {
    versors quatX = glms_quatv(eulerAngles.x, X_AXIS);
    versors quatY = glms_quatv(eulerAngles.y, Y_AXIS);
    versors quatZ = glms_quatv(eulerAngles.z, Z_AXIS);
    camera->Quaternion = glms_quat_mul(glms_quat_mul(quatY, quatX), quatZ);
}
void cameraSetQuaternion(Camera *camera, versors quaternion) {
    camera->Quaternion = quaternion;
}
