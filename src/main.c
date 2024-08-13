#include "window.h"

#include "camera.h"
#include "input.h"
#include "rendering.h"

#include "animation.h"
#include "material.h"
#include "model.h"
#include "model_presets.h"
#include "shader.h"

#include <stdio.h>

#define MOVE_SPEED 10.0f

GLFWwindow *window;

vec3s movementInput;
vec2s mousePosition;
vec2s mouseDelta;
vec2s mouseSensitivity;

int main(void) {
    window = windowCreate();
    windowSetSkybox(0.117f, 0.117f, 0.18f);
    inputInit(window);

    Camera camera =
        cameraCreate((vec3s){{0.0f, 1.0f, 1.0f}}, GLMS_QUAT_IDENTITY);
    cameraSetProjectionMatrixPersp(&camera, 60, 0.1f, 100.0f);
    cameraLookAt(&camera, GLMS_VEC3_ZERO);

    Model *model =
        modelPresetTexturedAll("SM_Deccer_Cubes_Textured_Complex.gltf",
                               "vertex_shader.glsl", "fragment_shader.glsl");

    float lastTime = 0, currentTime = 0, deltaTime = 0;
    vec3s eulerAngles = GLMS_VEC3_ZERO;
    vec3s positionDelta = GLMS_VEC3_ZERO;
    GLenum err;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

        for (int i = 0; i < model->AnimationCount; i++) {
            animationStep(model->Animations[i], deltaTime);
        }

        inputMouseUpdate(window);
        eulerAngles = glms_vec3_add(
            eulerAngles,
            (vec3s){{-mouseDelta.y * mouseSensitivity.x * deltaTime,
                     -mouseDelta.x * mouseSensitivity.y * deltaTime, 0}});
        cameraSetEulerAngles(&camera, eulerAngles);

        positionDelta = (vec3s){{movementInput.x, 0, -movementInput.y}};
        positionDelta = glms_quat_rotatev(camera.Quaternion, positionDelta);
        positionDelta =
            glms_vec3_add(positionDelta, (vec3s){{0, -movementInput.z, 0}});
        camera.Position = glms_vec3_add(
            camera.Position,
            glms_vec3_scale(positionDelta, MOVE_SPEED * deltaTime));
        cameraCalculateViewMatrix(&camera);

        lastTime = currentTime;

        modelRender(model);

        windowDraw(window);

        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr, "gl error 0x%04X\n", err);
        }
    }

    modelFree(model);

    windowClose();
    return 0;
}
