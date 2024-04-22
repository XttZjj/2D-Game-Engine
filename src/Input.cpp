#include "Input.h"

void Input::Init() {
    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++) {
        keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
    }
    for (int i = 1; i < 4; i++) {
        mouse_button_states[i] = INPUT_STATE_UP;
    }
    for (int code = SDL_CONTROLLER_BUTTON_INVALID; code < SDL_CONTROLLER_BUTTON_MAX; code++) {
        gamebutton_state[static_cast<SDL_GameControllerButton>(code)] = INPUT_STATE_UP;
    }
    mouse_position = glm::vec2(0, 0);
}

void Input::ProcessEvent(const SDL_Event& e) {
    if (e.type != SDL_KEYDOWN) {
        justPressedKey = SDL_SCANCODE_UNKNOWN;
    }
    if (e.type == SDL_KEYDOWN) {
        keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_scancodes.push_back(e.key.keysym.scancode);
        justPressedKey = e.key.keysym.scancode;
    } 
    else if (e.type == SDL_KEYUP) {
        keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_scancodes.push_back(e.key.keysym.scancode);
    }
    else if (e.type == SDL_MOUSEMOTION) {
        mouse_position = glm::vec2(e.motion.x, e.motion.y);
    }
    else if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            mouse_button_states[1] = INPUT_STATE_JUST_BECAME_DOWN;
            just_become_down_buttons.push_back(1);
        }
        else if (e.button.button == SDL_BUTTON_MIDDLE) {
            mouse_button_states[2] = INPUT_STATE_JUST_BECAME_DOWN;
            just_become_down_buttons.push_back(2);
        }
        else if (e.button.button == SDL_BUTTON_RIGHT) {
            mouse_button_states[3] = INPUT_STATE_JUST_BECAME_DOWN;
            just_become_down_buttons.push_back(3);
        }
    }
    else if (e.type == SDL_MOUSEBUTTONUP) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            mouse_button_states[1] = INPUT_STATE_JUST_BECAME_UP;
            just_become_up_buttons.push_back(1);
        }
        else if (e.button.button == SDL_BUTTON_MIDDLE) {
            mouse_button_states[2] = INPUT_STATE_JUST_BECAME_UP;
            just_become_up_buttons.push_back(2);
        }
        else if (e.button.button == SDL_BUTTON_RIGHT) {
            mouse_button_states[3] = INPUT_STATE_JUST_BECAME_UP;
            just_become_up_buttons.push_back(3);
        }
    }
    else if (e.type == SDL_MOUSEWHEEL) {
        mouse_scroll_this_frame = e.wheel.preciseY;
    }
    else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
        gamebutton_state[static_cast<SDL_GameControllerButton>(e.cbutton.button)] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_gamebutton.push_back(static_cast<SDL_GameControllerButton>(e.cbutton.button));
    }
    else if (e.type == SDL_CONTROLLERBUTTONUP) {
        gamebutton_state[static_cast<SDL_GameControllerButton>(e.cbutton.button)] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_gamebutton.push_back(static_cast<SDL_GameControllerButton>(e.cbutton.button));
    }
    else if (e.type == SDL_CONTROLLERAXISMOTION) {
        if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
            int value = e.caxis.value;
            std::cout << "Left trigger value: " << value << std::endl;
        }
        else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
            int value = e.caxis.value;
            std::cout << "Left trigger value: " << value << std::endl;
        }
        else if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
            left_x_axis = e.caxis.value;
        }
        else if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
            left_y_axis = e.caxis.value;
        }
        else if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
            right_x_axis = e.caxis.value;
        }
        else if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
            right_y_axis = e.caxis.value;
        }
    }
}

int Input::GetLeftXAxis() {
    return left_x_axis;
}

int Input::GetLeftYAxis() {
    return left_y_axis;
}

int Input::GetRightXAxis() {
    return right_x_axis;
}

int Input::GetRightYAxis() {
    return right_y_axis;
}

void Input::LateUpdate() {
    for (const SDL_Scancode& code : just_became_down_scancodes) {
        keyboard_states[code] = INPUT_STATE_DOWN;
    }
    just_became_down_scancodes.clear();

    for (const SDL_Scancode& code : just_became_up_scancodes) {
        keyboard_states[code] = INPUT_STATE_UP;
    }
    just_became_up_scancodes.clear();

    for (const auto& num : just_become_down_buttons) {
        mouse_button_states[num] = INPUT_STATE_DOWN;
    }
    just_become_down_buttons.clear();

    for (const auto& num : just_become_up_buttons) {
        mouse_button_states[num] = INPUT_STATE_UP;
    }
    just_become_up_buttons.clear();

    for (const SDL_GameControllerButton& code : just_became_down_gamebutton) {
        gamebutton_state[code] = INPUT_STATE_DOWN;
    }
    just_became_down_gamebutton.clear();

    for (const SDL_GameControllerButton& code : just_became_up_gamebutton) {
        gamebutton_state[code] = INPUT_STATE_UP;
    }
    just_became_up_gamebutton.clear();

    mouse_scroll_this_frame = 0;
}

bool Input::GetKey(std::string keycode) {
    SDL_Scancode key = keycode_to_scancode[keycode];
    return keyboard_states[key] == INPUT_STATE_DOWN || keyboard_states[key] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(std::string keycode) {
    SDL_Scancode key = keycode_to_scancode[keycode];
    return keyboard_states[key] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(std::string keycode) {
    SDL_Scancode key = keycode_to_scancode[keycode];
    return keyboard_states[key] == INPUT_STATE_JUST_BECAME_UP;
}

bool Input::GetControllerButton(std::string keycode) {
    SDL_GameControllerButton key = keycode_to_buttoncode[keycode];
    return gamebutton_state[key] == INPUT_STATE_DOWN || gamebutton_state[key] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetControllerButtonDown(std::string keycode) {
    SDL_GameControllerButton key = keycode_to_buttoncode[keycode];
    return gamebutton_state[key] == INPUT_STATE_JUST_BECAME_DOWN;
}
bool Input::GetControllerButtonUp(std::string keycode) {
    SDL_GameControllerButton key = keycode_to_buttoncode[keycode];
    return gamebutton_state[key] == INPUT_STATE_JUST_BECAME_UP;
}

glm::vec2 Input::GetMousePosition() {
    return mouse_position;
}

bool Input::GetMouseButton(int button_num) {
    return mouse_button_states[button_num] == INPUT_STATE_DOWN || mouse_button_states[button_num] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonDown(int button_num) {
    return mouse_button_states[button_num] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonUp(int button_num) {
    return mouse_button_states[button_num] == INPUT_STATE_JUST_BECAME_UP;
}

float Input::GetMouseScrollDelta() {
    return mouse_scroll_this_frame;
}

void Input::KeyPress(std::string key) {
    keyboard_states[keycode_to_scancode[key]] = INPUT_STATE_JUST_BECAME_DOWN;
    just_became_down_scancodes.push_back(keycode_to_scancode[key]);
}

void Input::KeyPressEnd(std::string key) {
    keyboard_states[keycode_to_scancode[key]] = INPUT_STATE_JUST_BECAME_UP;
    just_became_up_scancodes.push_back(keycode_to_scancode[key]);
}

std::string Input::GetKeyName() {
    if (justPressedKey != SDL_SCANCODE_UNKNOWN) {
        for (auto it : keycode_to_scancode) {
            if (it.second == justPressedKey) {
                return it.first;
            }
        }
    }
    return "";
}
