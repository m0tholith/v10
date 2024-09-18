#include "armature.h"
#include "cglm/struct/affine-pre.h"
#include "cglm/struct/affine.h"
#include "window.h"

#include "camera.h"
#include "input.h"
#include "rendering.h"

#include "animation.h"
#include "error.h"
#include "material.h"
#include "model.h"
#include "model_presets.h"
#include "shader.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MOVE_SPEED 10.0f
#define EVENT_COUNT 2

GLFWwindow *window;

vec2s mousePosition;
vec2s mouseDelta;
vec2s mouseSensitivity;

// just to make the main function more easily accessible
InputEvent *getInputEventArray();

int main(void) {
    window = windowCreate();
    windowSetSkybox(0.117f, 0.117f, 0.18f);
    InputEvent *events = getInputEventArray();
    inputSetEvents(events, 2);
    inputInit(window);
    errorInit();

    Camera camera =
        cameraCreate((vec3s){{0.0f, 1.0f, 1.0f}}, GLMS_QUAT_IDENTITY);
    cameraSetProjectionMatrixPersp(&camera, 60, 0.1f, 100.0f);
    cameraLookAt(&camera, GLMS_VEC3_ZERO);

    uint32_t lightShader = shaderCreate("light_vert.glsl", "light_frag.glsl");
    Model *light = modelLoad("light.glb");
    light->Materials[0] = materialCreate(lightShader, 0);
    vec3s lightPos = (vec3s){{-2.2f, 1.2f, -0.6f}};
    vec3s lightColor = GLMS_VEC3_ONE;
    light->WorldFromModel = glms_translate(GLMS_MAT4_IDENTITY, lightPos);

    uint32_t shader =
        shaderCreate("vertex_shader.glsl", "fragment_shader.glsl");
    Model *model = modelLoad("BrainStem.glb");
    model->Materials[0] =
        materialCreate(shader, 2,
                       materialPropertyCreate("light_position", MATTYPE_VEC3,
                                              (void *)&lightPos),
                       materialPropertyCreate("light_color", MATTYPE_VEC3,
                                              (void *)&lightColor));
    modelSetDefaultMaterial(model, model->Materials[0]);

    model->WorldFromModel = glms_rotate_x(glms_rotate_z(GLMS_MAT4_IDENTITY, glm_rad(90)), glm_rad(180));
    Armature *armature = armatureCreate(model);

    glEnable(GL_CULL_FACE);

    float lastTime = 0, currentTime = 0, deltaTime = 0;
    vec3s eulerAngles = GLMS_VEC3_ZERO;
    vec3s movementInput = GLMS_VEC3_ZERO;
    vec3s positionDelta = GLMS_VEC3_ZERO;

    InputEvent *movementEvent = inputGetEvent("movement");
    InputEvent *exitEvent = inputGetEvent("exit");
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

        for (int i = 0; i < model->AnimationCount; i++) {
            animationStep(model->Animations[i], deltaTime);
        }

        inputUpdate();
        if (exitEvent->State > 0)
            glfwSetWindowShouldClose(window, 1);
        inputMouseUpdate();
        eulerAngles = glms_vec3_add(
            eulerAngles,
            (vec3s){{-mouseDelta.y * mouseSensitivity.x * deltaTime,
                     -mouseDelta.x * mouseSensitivity.y * deltaTime, 0}});
        cameraSetEulerAngles(&camera, eulerAngles);

        movementInput = (vec3s){{
            glm_clamp(movementEvent->State & (1 << 0), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 1), -1, 1),
            glm_clamp(movementEvent->State & (1 << 2), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 3), -1, 1),
            glm_clamp(movementEvent->State & (1 << 4), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 5), -1, 1),
        }};
        positionDelta = (vec3s){{movementInput.x, 0, -movementInput.z}};
        positionDelta = glms_quat_rotatev(camera.Quaternion, positionDelta);
        positionDelta =
            glms_vec3_add(positionDelta, (vec3s){{0, movementInput.y, 0}});
        camera.Position = glms_vec3_add(
            camera.Position,
            glms_vec3_scale(positionDelta, MOVE_SPEED * deltaTime));
        cameraCalculateViewMatrix(&camera);

        lastTime = currentTime;

        lightColor.x = (sinf(currentTime * M_PI / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        lightColor.y = (sinf(currentTime * 0.7f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        lightColor.z = (sinf(currentTime * 1.3f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        light->WorldFromModel = glms_translate(GLMS_MAT4_IDENTITY, lightPos);
        modelSetNodeWorldMatrices(light);
        modelRender(light);

        modelSetNodeWorldMatrices(model);
        armatureApplyTransformations(armature);
        modelRender(model);

        windowDraw(window);
    }

    materialFree(model->Materials[0]);
    materialFree(light->Materials[0]);
    modelFree(model);
    modelFree(light);
    armatureFree(armature);
    shaderFreeCache();

    windowClose();

    free(events);
    return 0;
}

InputEvent *getInputEventArray() {
    InputEvent *events = malloc(EVENT_COUNT * sizeof(InputEvent));

    events[0] = (InputEvent){
        .Name = "movement",
        .Type = INPUTEVENT_AXIS,
        .KeyCount = 6,
    };
    events[0].Keys = malloc(6 * sizeof(struct InputKey));
    // done in this particular order so every two values are the positive and
    // negative ends of x,y,z axis
    events[0].Keys[0] = (struct InputKey){
        .Value = GLFW_KEY_D,
    };
    events[0].Keys[1] = (struct InputKey){
        .Value = GLFW_KEY_A,
    };
    events[0].Keys[2] = (struct InputKey){
        .Value = GLFW_KEY_E,
    };
    events[0].Keys[3] = (struct InputKey){
        .Value = GLFW_KEY_Q,
    };
    events[0].Keys[4] = (struct InputKey){
        .Value = GLFW_KEY_W,
    };
    events[0].Keys[5] = (struct InputKey){
        .Value = GLFW_KEY_S,
    };

    events[1] = (InputEvent){
        .Name = "exit",
        .Type = INPUTEVENT_BUTTON,
        .KeyCount = 1,
    };
    events[1].Keys = malloc(1 * sizeof(struct InputKey));
    events[1].Keys[0] = (struct InputKey){
        .Value = GLFW_KEY_ESCAPE,
    };

    return events;
}
