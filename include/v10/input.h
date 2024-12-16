#ifndef INPUT_H
#define INPUT_H

#include "v10/window.h"
#include <GLFW/glfw3.h>
#include <cglm/types-struct.h>
#include <stdint.h>

struct InputKey {
    /// same as `GLFW_KEY_A` or any other key
    int Value;
    /// same as `GLFW_MOD_SHIFT` or any other modifier (set to `0` for none)
    uint8_t Modifiers;
    /// same as `GLFW_RELEASE` or the other two actions (set to `0` in the
    /// `inputSetEvents` function)
    uint8_t Action;
};

/// BUTTON: sets `event.State` to highest possible action value out of all keys
/// (if key1 is released, key2 is pressed and key3 is repeat, `event.State` will
/// be 2)
///
/// AXIS: sets `event.State` to an int with x (`KeyCount`) bits used to show
/// what's pressed and what's not
enum InputEventType {
    INPUTEVENT_BUTTON = 0,
    INPUTEVENT_AXIS = (1 << 0),
};
typedef struct {
    enum InputEventType Type;
    struct InputKey *Keys;
    uint8_t KeyCount; /// max value: 32
    int State;
    char *Name;
} InputEvent;

typedef struct {
    vec2s Position;
    vec2s Delta;
    vec2s Sensitivity;
} MouseInput;

void inputInit(Window *window);
void inputSetEvents(InputEvent *events, int eventCount);
void inputUpdate();
void inputMouseUpdate(Window *mainWindow);
/**
 * @return: Pointer to mouse input struct; usable until input deinit
 */
MouseInput *inputGetMouseData();
InputEvent *inputGetEvent(char *eventName);
void inputDeInit();

#endif // !INPUT_H
