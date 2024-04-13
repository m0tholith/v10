#include "camera.h"

#define SCENE_UP ((vec3s){{0.0f, 1.0f, 0.0f}})

Camera cameraCreate(vec3s position, vec3s target) {
    Camera camera = (Camera){
        .Position = position,
    };
    cameraLookAt(&camera, target);
    cameraCalculateViewMatrix(&camera);
    return camera;
}
void cameraCalculateViewMatrix(Camera *camera) {
    camera->ViewMatrix = glms_mul(
        (mat4s){{
            {camera->Right.x, camera->Right.y, camera->Right.z, 0},
            {camera->Up.x, camera->Up.y, camera->Up.z, 0},
            {camera->Forward.x, camera->Forward.y, camera->Forward.z, 0},
            {0, 0, 0, 1},
        }},
        glms_translate(GLMS_MAT4_IDENTITY, glms_vec3_negate(camera->Position)));
}
void cameraMove(Camera *camera, vec3s newPosition) {
    camera->Position = newPosition;
}
void cameraCalculateDirectionalVectors(Camera *camera) {
    camera->Right = glms_normalize(glms_cross(SCENE_UP, camera->Forward));
    camera->Up = glms_cross(camera->Forward, camera->Right);
}
void cameraSetEulerAngles(Camera *camera, vec3s eulerAngles) {
    mat4s rotationMatrix = GLMS_MAT4_IDENTITY;
    rotationMatrix = glms_rotate_z(rotationMatrix, -eulerAngles.z);
    rotationMatrix = glms_rotate_x(rotationMatrix, -eulerAngles.y);
    rotationMatrix = glms_rotate_y(rotationMatrix, eulerAngles.x);

    camera->Forward = (vec3s){{0.0f, 0.0f, -1.0f}};
    cameraCalculateDirectionalVectors(camera);
    camera->Forward = glms_mat4_mulv3(rotationMatrix, camera->Forward, 1);
    camera->Up = glms_mat4_mulv3(rotationMatrix, camera->Up, 1);
    camera->Right = glms_mat4_mulv3(rotationMatrix, camera->Right, 1);
}
void cameraLookAt(Camera *camera, vec3s target) {
    camera->Forward = glms_normalize(glms_vec3_sub(target, camera->Position));
    cameraCalculateDirectionalVectors(camera);
}
