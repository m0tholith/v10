#include "input.h"
#include "cglm/util.h"

#include <cglm/struct.h>
#include <stdio.h>
#include <string.h>

GLFWwindow *_window;

InputEvent *_events;
int _eventCount;

extern vec2s mousePosition;
extern vec2s mouseDelta;
extern vec2s mouseSensitivity;

void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods);

void inputSetEvents(InputEvent *events, int eventCount) {
    _events = events;
    _eventCount = eventCount;
}
void inputInit(GLFWwindow *window) {
    _window = window;

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(_window, inputKeyCallback);
    mouseSensitivity = (vec2s){{0.25f, 0.25f}};
}
void inputUpdate() {
    for (int i = 0; i < _eventCount; i++) {
        InputEvent *event = &_events[i];
        switch (event->Type) {

        case INPUTEVENT_BUTTON:
            event->State = 0;
            for (int keyIdx = 0; keyIdx < event->KeyCount; keyIdx++) {
                struct InputKey *key = &event->Keys[keyIdx];
                event->State = glm_max(event->State, key->Action);
            }
            break;

        case INPUTEVENT_AXIS:
            for (int keyIdx = 0; keyIdx < event->KeyCount; keyIdx++) {
                struct InputKey *key = &event->Keys[keyIdx];

                if (key->Action & (GLFW_PRESS | GLFW_REPEAT))
                    event->State |= (1 << keyIdx);
                else
                    event->State &= ~(1 << keyIdx);
            }
            break;

        default:
            break;
        }
    }
}
void inputMouseUpdate() {
    double xPosition, yPosition;
    glfwGetCursorPos(_window, &xPosition, &yPosition);
    vec2s newPosition = (vec2s){{xPosition, yPosition}};
    mouseDelta = glms_vec2_sub(newPosition, mousePosition);
    mousePosition = newPosition;
}
InputEvent *inputGetEvent(char *eventName) {
    for (int i = 0; i < _eventCount; i++) {
        if (strcmp(_events[i].Name, eventName) == 0)
            return &_events[i];
    }
    fprintf(stderr, "ERROR: couldn't find input event '%s'\n", eventName);
    return NULL;
}

void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    /* printf("got callback:\n"
           "	key %d, action %d, mods %d\n",
           key, action, mods); */
    for (int i = 0; i < _eventCount; i++) {
        InputEvent *event = &_events[i];
        for (int keyIdx = 0; keyIdx < event->KeyCount; keyIdx++) {
            struct InputKey *eventKey = &event->Keys[keyIdx];
            if (key == eventKey->Value && mods == eventKey->Modifiers) {
                eventKey->Action = action;
                break;
            }
        }
    }
}
