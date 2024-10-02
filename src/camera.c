#include "camera.h"

#include "rendering.h"
#include "window.h"
#include <cglm/struct/affine-mat.h>
#include <cglm/struct/quat.h>
#include <cglm/struct/vec3-ext.h>
#include <cglm/struct/vec4.h>

#define X_AXIS ((vec3s){{1.0f, 0.0f, 0.0f}})
#define Y_AXIS ((vec3s){{0.0f, 1.0f, 0.0f}})
#define Z_AXIS ((vec3s){{0.0f, 0.0f, 1.0f}})

Camera *cameraCreate(vec3s position, versors quaternion) {
    Camera *camera = malloc(sizeof(Camera));
    *camera = (Camera){
        .Position = position,
        .Quaternion = quaternion,
    };

    glGenBuffers(1, &camera->MatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, camera->MatricesUBO);
    // mat4 _projectionFromWorld -> 64 => 0
    // mat4 _lightSpaceProjectionFromWorld -> 64 => 0
    // vec3 _cameraWorldPosition -> 16 => 64
    glBufferData(GL_UNIFORM_BUFFER, 64 + 64 + 16, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera->MatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    cameraCalculateViewMatrix(camera);
    return camera;
}
void cameraSetProjectionMatrixPersp(Camera *camera, float fov, float nearPlane,
                                    float farPlane) {
    ProjectionFromViewMatrix = glms_perspective(
        glm_rad(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, nearPlane,
        farPlane);
}
void cameraSetProjectionMatrixOrtho(Camera *camera, float x, float y,
                                    float nearPlane, float farPlane) {
#define INITIAL_SCALE 40.0f
    ProjectionFromViewMatrix = glms_ortho(-x, x, y, -y, nearPlane, farPlane);
}
void cameraCalculateViewMatrix(Camera *camera) {
    camera->ViewFromWorldMatrix =
        glms_quat_look(camera->Position, camera->Quaternion);
    ViewFromWorldMatrix = camera->ViewFromWorldMatrix;
}
void cameraSetPosition(Camera *camera, vec3s position) {
    camera->Position = position;
}
void cameraLookAt(Camera *camera, vec3s target) {
    camera->Quaternion = glms_quat_forp(camera->Position, target, Y_AXIS);
}
// creates a new quaternion by multiplying per-axis quaternions together in a
// specific order (YXZ)
void cameraSetEulerAngles(Camera *camera, vec3s eulerAngles) {
    versors quatX = glms_quatv(eulerAngles.x, X_AXIS);
    versors quatY = glms_quatv(eulerAngles.y, Y_AXIS);
    versors quatZ = glms_quatv(eulerAngles.z, Z_AXIS);
    camera->Quaternion = glms_quat_mul(glms_quat_mul(quatY, quatX), quatZ);
}
void cameraSetQuaternion(Camera *camera, versors quaternion) {
    camera->Quaternion = quaternion;
}
vec3s camPosPrev = GLMS_VEC3_ZERO;
versors camRotPrev = GLMS_QUAT_IDENTITY;
void cameraPreRender(Camera *camera) {
    if (glms_vec3_eqv(camera->Position, camPosPrev) &&
        glms_vec4_eqv(
            (vec4s){{camera->Quaternion.x, camera->Quaternion.y,
                     camera->Quaternion.z, camera->Quaternion.w}},
            (vec4s){{camRotPrev.x, camRotPrev.y, camRotPrev.z, camRotPrev.w}}))
        return;

    CameraPosition = camPosPrev = camera->Position;
    camRotPrev = camera->Quaternion;

    glBindBuffer(GL_UNIFORM_BUFFER, camera->MatricesUBO);
    mat4s projectionFromWorld =
        glms_mul(ProjectionFromViewMatrix, ViewFromWorldMatrix);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &projectionFromWorld);
    glBufferSubData(GL_UNIFORM_BUFFER, 64 * 2, 16, CameraPosition.raw);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void cameraFree(Camera *camera) {
    glDeleteBuffers(1, &camera->MatricesUBO);
    free(camera);
}
