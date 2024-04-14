#include "camera.h"

#define Y_AXIS ((vec3s){{0.0f, 1.0f, 0.0f}})

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

}
void cameraSetQuaternion(Camera *camera, versors quaternion) {
    camera->Quaternion = quaternion;
}
