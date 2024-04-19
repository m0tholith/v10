#include "window.h"

#include "camera.h"
#include "input.h"
#include "mesh.h"
#include "rendering.h"
#include "shader.h"

GLFWwindow *window;

mat4s ViewMatrix, ProjectionMatrix;
vec3s wsadqe;

int main(void) {
    window = windowCreate();

    glfwSetKeyCallback(window, inputKeyCallback);

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
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;

        vec3s deltaEulerAngles = glms_vec3_scale(wsadqe, deltaTime * 3);
        deltaEulerAngles = (vec3s){{
            deltaEulerAngles.y,
            -deltaEulerAngles.x,
            deltaEulerAngles.z,
        }};
        eulerAngles = glms_vec3_add(eulerAngles, deltaEulerAngles);
        cameraSetEulerAngles(&camera, eulerAngles);

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
