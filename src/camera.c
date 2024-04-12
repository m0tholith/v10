#include "camera.h"

#define SCENE_UP ((vec3s){0.0f, 1.0f, 0.0f})

Camera cameraCreate(vec3s position, vec3s target) {
    Camera camera = (Camera){
        .Position = position,
    };
    cameraLookAt(&camera, target);
    cameraCalculateViewMatrix(&camera);
    return camera;
}
void cameraCalculateViewMatrix(Camera *camera) {
    camera->ViewMatrix =
        glms_lookat(camera->Position,
                    glms_vec3_add(camera->Position, camera->Forward), SCENE_UP);
}
void cameraMove(Camera *camera, vec3s newPosition) {
    camera->Position = newPosition;
}
void cameraLookAt(Camera *camera, vec3s target) {
    camera->Forward = glms_normalize(glms_vec3_sub(target, camera->Position));
    camera->Right =
        glms_normalize(glms_cross(SCENE_UP, glms_vec3_negate(camera->Forward)));
    camera->Up = glms_cross(glms_vec3_negate(camera->Forward), camera->Right);
}
