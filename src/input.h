#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

void inputInit(GLFWwindow *window);
void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods);
void inputMouseUpdate(GLFWwindow *window);

#endif // !INPUT_H
