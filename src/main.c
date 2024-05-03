#include "window.h"

#include "camera.h"
#include "input.h"
#include "material.h"
#include "model.h"
#include "rendering.h"
#include "shader.h"
#include "texture.h"

#define MOVE_SPEED 10.0f

GLFWwindow *window;

mat4s ViewMatrix, ProjectionMatrix;
vec3s movementInput;
vec2s mousePosition;
vec2s mouseDelta;
vec2s mouseSensitivity;

int main(void) {
    window = windowCreate();
    inputInit(window);

    Camera camera =
        cameraCreate((vec3s){{0.0f, 1.0f, -1.0f}}, GLMS_QUAT_IDENTITY);
    cameraLookAt(&camera, GLMS_VEC3_ZERO);

    // shader init
    unsigned int shader = shaderCreate("shaders/vertex_shader.vert",
                                       "shaders/fragment_shader.frag");

    // texture init
    Texture light =
        textureCreate("textures/prototype/light.png", TEXTURETYPE_RGB);
    Texture orange =
        textureCreate("textures/prototype/orange.png", TEXTURETYPE_RGB);
    Texture dark =
        textureCreate("textures/prototype/dark.png", TEXTURETYPE_RGB);

    // material init

    Material *lightMaterial = materialCreate(shader, 1);
    lightMaterial->Properties[0] =
        materialPropertyCreate("albedo", MATTYPE_TEXTURE2D, (void *)&light.Id);
    materialApplyProperties(lightMaterial);
    Material *orangeMaterial = materialCopy(lightMaterial);
    materialChangeProperty(orangeMaterial, "albedo", (void *)&orange.Id);
    Material *darkMaterial = materialCopy(lightMaterial);
    materialChangeProperty(darkMaterial, "albedo", (void *)&dark.Id);

    Model *model1 = modelLoad("models/nodes_test.glb");
    model1->Materials[0] = lightMaterial;
    model1->Materials[1] = orangeMaterial;
    model1->Materials[2] = darkMaterial;

    ProjectionMatrix = glms_perspective(
        glm_rad(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f,
        100.0f);

    float lastTime = 0, currentTime = 0, deltaTime = 0;
    vec3s eulerAngles = GLMS_VEC3_ZERO;
    vec3s positionDelta = GLMS_VEC3_ZERO;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

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
        ViewMatrix = camera.ViewMatrix;

        lastTime = currentTime;

        // HINT: drawing goes here
        // glClearColor(0.12f, 0.12f, 0.18f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        modelRender(model1);

        windowDraw(window);
    }

    modelDelete(model1);
    materialFree(darkMaterial);

    windowClose();
    return 0;
}
