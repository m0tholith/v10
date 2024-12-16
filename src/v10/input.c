#include "v10/input.h"
#include <cglm/util.h>

#include <cglm/struct.h>
#include <stdio.h>
#include <string.h>

InputEvent *_events;
int _eventCount;

MouseInput *_mouseData = NULL;

void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods);

void inputSetEvents(InputEvent *events, int eventCount) {
    _events = events;
    _eventCount = eventCount;
}
void inputInit(Window *window) {
    glfwSetInputMode(window->glWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window->glWin, inputKeyCallback);

    if (_mouseData == NULL) {
        _mouseData = malloc(sizeof(MouseInput));
        memset(_mouseData, 0, sizeof(MouseInput));
        _mouseData->Sensitivity = GLMS_VEC2_ONE;
    }
    printf("Initialized input\n");
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
void inputMouseUpdate(Window *mainWindow) {
    double xPosition, yPosition;
    glfwGetCursorPos(mainWindow->glWin, &xPosition, &yPosition);
    vec2s newPosition = (vec2s){{xPosition, yPosition}};
    _mouseData->Delta = glms_vec2_sub(newPosition, _mouseData->Position);
    _mouseData->Position = newPosition;
}
MouseInput *inputGetMouseData() { return _mouseData; }
InputEvent *inputGetEvent(char *eventName) {
    for (int i = 0; i < _eventCount; i++) {
        if (strcmp(_events[i].Name, eventName) == 0)
            return &_events[i];
    }
    fprintf(stderr, "Couldn't find input event '%s'\n", eventName);
    return NULL;
}
void inputDeInit() {
    if (_mouseData == NULL)
        return;
    free(_mouseData);
}

void inputKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    printf("Got callback:\n\tkey %d, action %d, mods %d\n", key, action, mods);
    for (int i = 0; i < _eventCount; i++) {
        InputEvent *event = &_events[i];
        for (int keyIdx = 0; keyIdx < event->KeyCount; keyIdx++) {
            struct InputKey *eventKey = &event->Keys[keyIdx];
            bool check;
            if (eventKey->Modifiers == 0)
                check = key == eventKey->Value;
            else
                check = key == eventKey->Value && mods == eventKey->Modifiers;
            if (check) {
                eventKey->Action = action;
                break;
            }
        }
    }
}
