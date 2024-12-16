#include "v10/camera.h"

#include "v10/rendering.h"
#include "v10/window.h"
#include <cglm/struct/affine-mat.h>
#include <cglm/struct/quat.h>
#include <cglm/struct/vec3-ext.h>
#include <cglm/struct/vec4.h>
#include <stdio.h>
#include <string.h>

#define QUAT_EQ(a, b)                                                          \
    (glms_vec4_eqv((vec4s){{a.x, a.y, a.z, a.w}},                              \
                   (vec4s){{b.x, b.y, b.z, b.w}}))
#define MAT4_EQ(a, b)                                                          \
    (a.m00 == b.m00 && a.m01 == b.m01 && a.m02 == b.m02 && a.m03 == b.m03 &&   \
     a.m10 == b.m10 && a.m11 == b.m11 && a.m12 == b.m12 && a.m13 == b.m13 &&   \
     a.m20 == b.m20 && a.m21 == b.m21 && a.m22 == b.m22 && a.m23 == b.m23 &&   \
     a.m30 == b.m30 && a.m31 == b.m31 && a.m32 == b.m32 && a.m33 == b.m33)

bool CAM_EQ(Camera *camera) {
    bool positionEqual =
        glms_vec3_eqv(camera->Position, camera->_prev.position);
    bool rotationEqual = QUAT_EQ(camera->Quaternion, camera->_prev.quaternion);
    bool projectionEqual = MAT4_EQ(camera->ProjectionFromViewMatrix,
                                   camera->_prev.projectionFromViewMatrix);
    bool viewEqual =
        MAT4_EQ(camera->ViewFromWorldMatrix, camera->_prev.viewFromWorldMatrix);

    return positionEqual && rotationEqual && projectionEqual && viewEqual;
}

Camera *cameraCreate(vec3s position, versors quaternion) {
    Camera *camera = malloc(sizeof(Camera));
    memset(camera, 0, sizeof(Camera));
    *camera = (Camera){
        .Position = position,
        .Quaternion = quaternion,
    };

    glGenBuffers(1, &camera->MatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, camera->MatricesUBO);
    // mat4 _projectionFromWorld -> 64 => 0
    // vec3 _cameraWorldPosition -> 16 => 64
    glBufferData(GL_UNIFORM_BUFFER, sizeof(vec4s) + sizeof(mat4s), NULL,
                 GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera->MatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    cameraCalculateViewMatrix(camera);
    return camera;
}
void cameraSetProjectionMatrixPersp(Camera *camera, float fov, float ratio,
                                    float nearPlane, float farPlane) {
    camera->ProjectionFromViewMatrix =
        glms_perspective(glm_rad(fov), ratio, nearPlane, farPlane);
}
void cameraSetProjectionMatrixOrtho(Camera *camera, float x, float y,
                                    float nearPlane, float farPlane) {
    camera->ProjectionFromViewMatrix =
        glms_ortho(-x, x, y, -y, nearPlane, farPlane);
}
void cameraCalculateViewMatrix(Camera *camera) {
    camera->ViewFromWorldMatrix =
        glms_quat_look(camera->Position, camera->Quaternion);
}
void cameraSetPosition(Camera *camera, vec3s position) {
    camera->Position = position;
}
void cameraLookAt(Camera *camera, vec3s target) {
    camera->Quaternion = glms_quat_forp(camera->Position, target, GLMS_YUP);
}
// creates a new quaternion by multiplying per-axis quaternions together in a
// specific order (YXZ)
void cameraSetEulerAngles(Camera *camera, vec3s eulerAngles) {
    versors quatX = glms_quatv(eulerAngles.x, GLMS_XUP);
    versors quatY = glms_quatv(eulerAngles.y, GLMS_YUP);
    versors quatZ = glms_quatv(eulerAngles.z, GLMS_ZUP);
    camera->Quaternion = glms_quat_mul(glms_quat_mul(quatY, quatX), quatZ);
}
void cameraSetQuaternion(Camera *camera, versors quaternion) {
    camera->Quaternion = quaternion;
}
void cameraPreRender(Camera *camera) {
    if (CAM_EQ(camera) == 1)
        return;

    ProjectionFromViewMatrix = camera->ProjectionFromViewMatrix;
    ViewFromWorldMatrix = camera->ViewFromWorldMatrix;
    CameraPosition = camera->Position;

    glBindBuffer(GL_UNIFORM_BUFFER, camera->MatricesUBO);
    mat4s projectionFromWorld =
        glms_mul(ProjectionFromViewMatrix, ViewFromWorldMatrix);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &projectionFromWorld);
    glBufferSubData(GL_UNIFORM_BUFFER, 64, 16, CameraPosition.raw);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    memcpy(&camera->_prev,
           (void *)((unsigned long)camera + sizeof(struct camera_data)),
           sizeof(struct camera_data));
}
void cameraFree(Camera *camera) {
    glDeleteBuffers(1, &camera->MatricesUBO);
    free(camera);
}
