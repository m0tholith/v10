#include "window.h"

#include "camera.h"
#include "input.h"
#include "mesh.h"
#include "rendering.h"
#include "shader.h"

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

    Camera camera = cameraCreate((vec3s){{0.0f, 0.0f, 0.0f}},
                                 (versors){{0.0f, 0.0f, 0.0f, 0.0f}});

    // texture init
    Texture texture = textureCreate("textures/crate.jpg", TEXTURETYPE_RGB);

    // shader init
    unsigned int shader = shaderCreate("shaders/vertex_shader.vert",
                                       "shaders/fragment_shader.frag");

    // rectangle
    Vertex vertices[] = {
        // position, tex coords
        (Vertex){(vec3s){{-1.0f, -1.0f, 1.0f}},
                 (vec2s){{0.0f, 0.0f}}}, // 0, 17, 21
        (Vertex){(vec3s){{-1.0f, 1.0f, 1.0f}},
                 (vec2s){{0.0f, 1.0f}}}, // 1, 5, 22
        (Vertex){(vec3s){{1.0f, 1.0f, 1.0f}},
                 (vec2s){{1.0f, 1.0f}}}, // 2, 6, 10
        (Vertex){(vec3s){{1.0f, -1.0f, 1.0f}},
                 (vec2s){{1.0f, 0.0f}}}, // 3, 9, 18

        (Vertex){(vec3s){{-1.0f, 1.0f, -1.0f}},
                 (vec2s){{0.0f, 0.0f}}}, // 4, 13, 23
        (Vertex){(vec3s){{-1.0f, 1.0f, 1.0f}},
                 (vec2s){{0.0f, 1.0f}}}, // 1, 5, 22
        (Vertex){(vec3s){{1.0f, 1.0f, 1.0f}},
                 (vec2s){{1.0f, 1.0f}}}, // 2, 6, 10
        (Vertex){(vec3s){{1.0f, 1.0f, -1.0f}},
                 (vec2s){{1.0f, 0.0f}}}, // 7, 11, 14

        (Vertex){(vec3s){{1.0f, -1.0f, -1.0f}},
                 (vec2s){{0.0f, 0.0f}}}, // 8, 15, 19
        (Vertex){(vec3s){{1.0f, -1.0f, 1.0f}},
                 (vec2s){{0.0f, 1.0f}}}, // 3, 9, 18
        (Vertex){(vec3s){{1.0f, 1.0f, 1.0f}},
                 (vec2s){{1.0f, 1.0f}}}, // 2, 6, 10
        (Vertex){(vec3s){{1.0f, 1.0f, -1.0f}},
                 (vec2s){{1.0f, 0.0f}}}, // 7, 11, 14

        (Vertex){(vec3s){{-1.0f, -1.0f, -1.0f}},
                 (vec2s){{0.0f, 0.0f}}}, // 12, 16, 20
        (Vertex){(vec3s){{-1.0f, 1.0f, -1.0f}},
                 (vec2s){{0.0f, 1.0f}}}, // 4, 13, 23
        (Vertex){(vec3s){{1.0f, 1.0f, -1.0f}},
                 (vec2s){{1.0f, 1.0f}}}, // 7, 11, 14
        (Vertex){(vec3s){{1.0f, -1.0f, -1.0f}},
                 (vec2s){{1.0f, 0.0f}}}, // 8, 15, 19

        (Vertex){(vec3s){{-1.0f, -1.0f, -1.0f}},
                 (vec2s){{0.0f, 0.0f}}}, // 12, 16, 20
        (Vertex){(vec3s){{-1.0f, -1.0f, 1.0f}},
                 (vec2s){{0.0f, 1.0f}}}, // 0, 17, 21
        (Vertex){(vec3s){{1.0f, -1.0f, 1.0f}},
                 (vec2s){{1.0f, 1.0f}}}, // 3, 9, 18
        (Vertex){(vec3s){{1.0f, -1.0f, -1.0f}},
                 (vec2s){{1.0f, 0.0f}}}, // 8, 15, 19

        (Vertex){(vec3s){{-1.0f, -1.0f, -1.0f}},
                 (vec2s){{0.0f, 0.0f}}}, // 12, 16, 20
        (Vertex){(vec3s){{-1.0f, -1.0f, 1.0f}},
                 (vec2s){{0.0f, 1.0f}}}, // 0, 17, 21
        (Vertex){(vec3s){{-1.0f, 1.0f, 1.0f}},
                 (vec2s){{1.0f, 1.0f}}}, // 1, 5, 22
        (Vertex){(vec3s){{-1.0f, 1.0f, -1.0f}},
                 (vec2s){{1.0f, 0.0f}}}, // 4, 13, 23
    };
    unsigned int indices[] = {
        0,  1,  2, // triangle 1
        0,  2,  3, // triangle 2

        4,  5,  6, // triangle 3
        4,  6,  7, // triangle 4

        8,  9,  10, // triangle 5
        8,  10, 11, // triangle 6

        12, 13, 14, // triangle 7
        12, 14, 15, // triangle 8

        16, 17, 18, // triangle 9
        16, 18, 19, // triangle 10

        20, 21, 22, // triangle 11
        20, 22, 23, // triangle 12
    };
    Mesh *mesh1 =
        meshCreate(vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
                   sizeof(indices) / sizeof(indices[0]), texture.Id);
    mesh1->Transform =
        glms_translate(mesh1->Transform, (vec3s){{-4.0f, -2.0f, 0.0f}});
    Mesh *mesh2 =
        meshCreate(vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
                   sizeof(indices) / sizeof(indices[0]), texture.Id);
    mesh2->Transform =
        glms_translate(mesh2->Transform, (vec3s){{4.0f, 2.0f, 0.0f}});
    Mesh *mesh3 =
        meshCreate(vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
                   sizeof(indices) / sizeof(indices[0]), texture.Id);
    mesh3->Transform =
        glms_translate(mesh3->Transform, (vec3s){{0.0f, 0.0f, -4.5f}});
    Mesh *mesh4 =
        meshCreate(vertices, indices, sizeof(vertices) / sizeof(vertices[0]),
                   sizeof(indices) / sizeof(indices[0]), texture.Id);
    mesh4->Transform =
        glms_translate(mesh4->Transform, (vec3s){{0.0f, 0.0f, 4.5f}});

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

        meshRender(mesh1, shader);
        meshRender(mesh2, shader);
        meshRender(mesh3, shader);
        meshRender(mesh4, shader);

        windowDraw(window);
    }

    meshDelete(mesh1);
    meshDelete(mesh2);
    meshDelete(mesh3);
    meshDelete(mesh4);

    windowClose();
    return 0;
}
