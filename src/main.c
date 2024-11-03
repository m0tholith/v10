#include "window.h"

#ifdef ENABLE_ERRORCHECKING
#include "error.h"
#endif
#include "animation.h"
#include "armature.h"
#include "camera.h"
#include "input.h"
#include "light.h"
#include "material.h"
#include "model.h"
#include "render_texture.h"
#include "scene_object.h"
#include "shader.h"
#include "texture.h"

#include <cglm/struct/affine-pre.h>
#include <cglm/struct/affine.h>
#include <cglm/struct/vec3.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MOVE_SPEED 10.0f
#define EVENT_COUNT 2

GLFWwindow *window;

vec2s mousePosition;
vec2s mouseDelta;
vec2s mouseSensitivity;

// just to make the main function more easily accessible
InputEvent *getInputEventArray();
void freeInputEventArray(InputEvent *events);

int main(void) {
    window = windowCreate();
    windowSetSkybox(0.117f, 0.117f, 0.18f);
    InputEvent *events = getInputEventArray();
    inputSetEvents(events, EVENT_COUNT);
    inputInit(window);
#ifdef ENABLE_ERRORCHECKING
    errorInit();
#endif

    ShaderCache *shaderCache = shaderCacheCreate();
    shaderSetCache(shaderCache);

    Camera *camera = cameraCreate(GLMS_VEC3_ZERO, GLMS_QUAT_IDENTITY);
    cameraSetProjectionMatrixPersp(camera, 60, 0.1f, 100.0f);
    cameraLookAt(camera, GLMS_VEC3_ZERO);

    DirLight *dirLights = malloc(DIRLIGHTS_MAX * sizeof(DirLight));
    memset(dirLights, 0, DIRLIGHTS_MAX * sizeof(DirLight));
    dirLights[0] = dirLightCreate(
        glms_vec3_normalize(glms_vec3_negate((vec3s){{-3, -4, 7}})),
        (vec3s){{0.0f, 0.0f, 0.0f}}, (vec3s){{0.694f, 0.688f, 0.578f}},
        (vec3s){{0.7f, 0.7f, 0.7f}});

    PointLight *pointLights = malloc(POINTLIGHTS_MAX * sizeof(PointLight));
    memset(pointLights, 0, POINTLIGHTS_MAX * sizeof(PointLight));
    pointLights[0] = pointLightCreate(
        (vec3s){{-2.2f, 1.2f, -0.6f}}, (vec3s){{0.2f, 0.2f, 0.2f}},
        (vec3s){{1.0f, 1.0f, 1.0f}}, 1.0f, 10.0f, 1.5f);

    SpotLight *spotLights = malloc(SPOTLIGHTS_MAX * sizeof(SpotLight));
    memset(spotLights, 0, SPOTLIGHTS_MAX * sizeof(SpotLight));
    spotLights[0] = spotLightCreate(
        (vec3s){{5.2f, -3.2f, -5.6f}},
        glms_vec3_normalize(glms_vec3_negate((vec3s){{-3, 2, 1}})),
        (vec3s){{1.0f, 0.2f, 1.0f}}, (vec3s){{1.0f, 1.0f, 1.0f}}, 5.0f, 15.0f,
        1.5f, 12.5f, 17.5f);

    LightScene *lightScene =
        lightSceneCreate(dirLights, pointLights, spotLights);

    int SCENE_OBJECT_COUNT = 0;

    Shader *lightShader =
        shaderCreate("light_source.vert", "light_source.frag");
    Model *pointLightModel = modelLoad("light.glb", 0);
    pointLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&pointLights[0].Diffuse));
    pointLightModel->WorldFromModel =
        glms_translate(GLMS_MAT4_IDENTITY, pointLights[0].Position);
    SCENE_OBJECT_COUNT++;

    Model *dirLightModel = modelLoad("arrow.glb", 0);
    dirLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&dirLights[0].Diffuse));
    vec3s startAxis = (vec3s){{1, 0, 0}};
    float angle = acos(glms_vec3_dot(startAxis, dirLights[0].Direction));
    vec3s axis = glms_vec3_cross(startAxis, dirLights[0].Direction);
    dirLightModel->WorldFromModel =
        glms_rotate(GLMS_MAT4_IDENTITY, angle, axis);
    SCENE_OBJECT_COUNT++;

    Model *spotLightModel = modelLoad("flashlight.glb", 0);
    spotLightModel->Materials[0] =
        materialCreate(lightShader, 1,
                       materialPropertyCreate("diffuse", MATTYPE_VEC3,
                                              (void *)&spotLights[0].Diffuse));
    angle = acos(glms_vec3_dot(startAxis, spotLights[0].Direction));
    axis = glms_vec3_cross(startAxis, spotLights[0].Direction);
    spotLightModel->WorldFromModel =
        glms_rotate(glms_translate(GLMS_MAT4_IDENTITY, spotLights[0].Position),
                    angle, axis);
    SCENE_OBJECT_COUNT++;

    Shader *homeShader =
        shaderCreate("light_affected.vert", "light_affected.frag");
    Model *homeModel = modelLoad("home.glb", 0);
    for (int i = 0; i < homeModel->MaterialCount; i++) {
        homeModel->Materials[i]->Shader = homeShader;
    }
    SCENE_OBJECT_COUNT++;

    Shader *skinningShader =
        shaderCreate("skinning.vert", "light_affected.frag");
    Model *skinningModel = modelLoad("BrainStem.glb", 0);
    skinningModel->DepthShader =
        shaderCreate("depth_skinning.vert", "depth.frag");
    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        skinningModel->Materials[i]->Shader = skinningShader;
    }
    skinningModel->WorldFromModel = glms_translate(
        glms_scale(glms_rotate_x(glms_rotate_z(GLMS_MAT4_IDENTITY, glm_rad(90)),
                                 glm_rad(180)),
                   (vec3s){{1.5f, 1.5f, 1.5f}}),
        (vec3s){{-3.0f, 0.0f, -1.4f}});
    Armature *armature = armatureCreate(skinningModel);
    SCENE_OBJECT_COUNT++;

    const int TexturedBoxCount = 7;
    Model **texturedBoxes = malloc(TexturedBoxCount * sizeof(Model *));
    Shader *texturedShader =
        shaderCreate("light_affected.vert", "light_affected.frag");
    for (int i = 0; i < TexturedBoxCount; i++) {
        texturedBoxes[i] = modelLoad("BoxTextured.glb", 0);
        for (int j = 0; j < texturedBoxes[i]->MaterialCount; j++) {
            texturedBoxes[i]->Materials[j]->Shader = texturedShader;
        }
        texturedBoxes[i]->WorldFromModel = glms_translate(
            glms_rotate_y(GLMS_MAT4_IDENTITY, glm_rad(23)),
            (vec3s){{-1.7f + (float)i / ((TexturedBoxCount - 1) * 2), -4.0f + i,
                     -4.4f}});
        SCENE_OBJECT_COUNT++;
    }

    SceneObject **sceneObjects =
        malloc(SCENE_OBJECT_COUNT * sizeof(SceneObject *));
    sceneObjects[0] = sceneObjectCreate(pointLightModel, NULL);
    sceneObjects[1] = sceneObjectCreate(dirLightModel, NULL);
    sceneObjects[2] = sceneObjectCreate(spotLightModel, NULL);
    sceneObjects[3] = sceneObjectCreate(homeModel, NULL);
    sceneObjects[4] = sceneObjectCreate(skinningModel, armature);
    for (int i = 5; i < SCENE_OBJECT_COUNT; i++) {
        sceneObjects[i] = sceneObjectCreate(texturedBoxes[i - 5], NULL);
    }

    //
    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    maxTextureSize /= 4;
    const int SHADOW_WIDTH = maxTextureSize, SHADOW_HEIGHT = maxTextureSize;
    RenderTexture *dirLightRenderTex =
        renderTextureCreate(SHADOW_WIDTH, SHADOW_HEIGHT, RENDERTEX_DEPTH);
    //

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

        for (int i = 0; i < skinningModel->AnimationCount; i++) {
            animationStep(skinningModel->Animations[i], deltaTime);
        }
        for (int i = 0; i < homeModel->AnimationCount; i++) {
            animationStep(homeModel->Animations[i], deltaTime);
        }

        inputUpdate();
        if (exitEvent->State > 0)
            glfwSetWindowShouldClose(window, 1);
        inputMouseUpdate();
        eulerAngles = glms_vec3_add(
            eulerAngles,
            (vec3s){{-mouseDelta.y * mouseSensitivity.x * deltaTime,
                     -mouseDelta.x * mouseSensitivity.y * deltaTime, 0}});
        cameraSetEulerAngles(camera, eulerAngles);

        movementInput = (vec3s){{
            glm_clamp(movementEvent->State & (1 << 0), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 1), -1, 1),
            glm_clamp(movementEvent->State & (1 << 2), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 3), -1, 1),
            glm_clamp(movementEvent->State & (1 << 4), -1, 1) -
                glm_clamp(movementEvent->State & (1 << 5), -1, 1),
        }};
        positionDelta = (vec3s){{movementInput.x, 0, -movementInput.z}};
        positionDelta = glms_quat_rotatev(camera->Quaternion, positionDelta);
        positionDelta =
            glms_vec3_add(positionDelta, (vec3s){{0, movementInput.y, 0}});
        camera->Position = glms_vec3_add(
            camera->Position,
            glms_vec3_scale(positionDelta, MOVE_SPEED * deltaTime));
        cameraCalculateViewMatrix(camera);

        pointLights[0].Position.x = sinf(currentTime * M_PI) * 2.3f + 0.7f;
        pointLights[0].Position.y =
            sinf(currentTime * M_PI * 0.8f) * 1.7f + 0.7f;
        pointLights[0].Position.z =
            sinf(currentTime * M_PI * 1.3f) * 2.8f + -1.2f;
        pointLights[0].Diffuse.x =
            (sinf(currentTime * M_PI / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLights[0].Diffuse.y =
            (sinf(currentTime * 0.7f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLights[0].Diffuse.z =
            (sinf(currentTime * 1.3f / 4) * 0.5 + 0.5) * 0.9f + 0.6f;
        pointLightModel->WorldFromModel =
            glms_translate(GLMS_MAT4_IDENTITY, pointLights[0].Position);

        vec3s startAxis = (vec3s){{1, 0, 0}};
        float angle = acos(glms_vec3_dot(startAxis, dirLights[0].Direction));
        vec3s axis = glms_vec3_cross(startAxis, dirLights[0].Direction);
        dirLightModel->WorldFromModel =
            glms_rotate(GLMS_MAT4_IDENTITY, angle, axis);

        lastTime = currentTime;

        lightScenePreRender(lightScene);
        cameraPreRender(camera);

        //
        renderTextureBind(dirLightRenderTex);
        glCullFace(GL_FRONT);

        ///
#define sendLightMatrix(__model)                                               \
    glUseProgram(__model->DepthShader->ID);                                    \
    glUniformMatrix4fv(glGetUniformLocation(__model->DepthShader->ID,          \
                                            "_lightSpaceProjectionFromWorld"), \
                       1, GL_FALSE,                                            \
                       (void *)&dirLights[0].ProjectionFromWorld);

        for (int i = 0; i < SCENE_OBJECT_COUNT; i++) {
            sendLightMatrix(sceneObjects[i]->Model);
            sceneObjectRender(sceneObjects[i], true);
        }
        ///
        glCullFace(GL_BACK);
        renderTextureResetBind();
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        for (int i = 0; i < shaderCache->Used; i++) {
            glUseProgram(shaderCache->Array[i].value->ID);
            glActiveTexture(GL_TEXTURE10);
            glBindTexture(GL_TEXTURE_2D, dirLightRenderTex->Texture);
        }
        meshOverrideShaders(NULL);
        //

        modelRender(pointLightModel);
        modelRender(dirLightModel);
        modelRender(spotLightModel);
        modelRender(skinningModel);
        modelRender(homeModel);
        for (int i = 0; i < TexturedBoxCount; i++) {
            modelRender(texturedBoxes[i]);
        }

        windowDraw(window);
    }

    for (int i = 0; i < skinningModel->MaterialCount; i++) {
        Material *material = skinningModel->Materials[i];
        for (int j = 0; j < material->PropertyCount; j++) {
            MaterialProperty *property = material->Properties[j];
            if (strncmp(property->Name, "_material.", 9) == 0) {
                materialPropertyDelete(property);
            }
        }
        materialFree(material);
    }
    materialFree(homeModel->Materials[0]);
    materialPropertyFree(pointLightModel->Materials[0]->Properties[0]);
    materialFree(pointLightModel->Materials[0]);
    materialPropertyFree(dirLightModel->Materials[0]->Properties[0]);
    materialFree(dirLightModel->Materials[0]);
    modelFree(skinningModel);
    modelFree(pointLightModel);
    modelFree(dirLightModel);
    modelFree(homeModel);
    for (int i = 0; i < TexturedBoxCount; i++) {
        materialFree(texturedBoxes[i]->Materials[0]);
        modelFree(texturedBoxes[i]);
    }
    free(texturedBoxes);
    armatureFree(armature);

    for (int i = 0; i < SCENE_OBJECT_COUNT; i++) {
        sceneObjectFree(sceneObjects[i]);
    }
    free(sceneObjects);

    free(pointLights);
    free(dirLights);
    lightSceneFree(lightScene);

    textureFreeCache();
    shaderCacheFree(shaderCache);

    renderTextureFree(dirLightRenderTex);

    windowClose();

    cameraFree(camera);

    freeInputEventArray(events);
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
void freeInputEventArray(InputEvent *events) {
    for (int i = 0; i < EVENT_COUNT; i++) {
        free(events[i].Keys);
    }
    free(events);
}
