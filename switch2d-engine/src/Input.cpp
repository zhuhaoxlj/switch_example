#include "Switch2D/Input.h"
#include <cstdio>

namespace Switch2D {

InputManager::InputManager() {
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    // 初始化触摸屏
    hidInitializeTouchScreen();
    
    // 初始化陀螺仪
    hidGetSixAxisSensorHandles(&sixaxisHandle, 1, HidNpadIdType_Handheld, HidNpadStyleTag_NpadHandheld);
    hidStartSixAxisSensor(sixaxisHandle);
}

InputManager::~InputManager() {
}

void InputManager::update() {
    // 更新手柄状态
    padUpdate(&pad);
    buttonsDown = padGetButtonsDown(&pad);
    buttonsUp = padGetButtonsUp(&pad);
    buttonsHeld = padGetButtons(&pad);
    
    // 更新触摸状态
    wasTouching = isTouchingNow;
    if (hidGetTouchScreenStates(&touchState, 1)) {
        isTouchingNow = touchState.count > 0;
    } else {
        isTouchingNow = false;
    }
}

HidNpadButton InputManager::mapButton(Button button) const {
    switch (button) {
        case Button::A: return HidNpadButton_A;
        case Button::B: return HidNpadButton_B;
        case Button::X: return HidNpadButton_X;
        case Button::Y: return HidNpadButton_Y;
        case Button::L: return HidNpadButton_L;
        case Button::R: return HidNpadButton_R;
        case Button::ZL: return HidNpadButton_ZL;
        case Button::ZR: return HidNpadButton_ZR;
        case Button::Plus: return HidNpadButton_Plus;
        case Button::Minus: return HidNpadButton_Minus;
        case Button::Left: return HidNpadButton_Left;
        case Button::Right: return HidNpadButton_Right;
        case Button::Up: return HidNpadButton_Up;
        case Button::Down: return HidNpadButton_Down;
        case Button::LeftStick: return HidNpadButton_StickL;
        case Button::RightStick: return HidNpadButton_StickR;
        default: return HidNpadButton_A;
    }
}

bool InputManager::getButton(Button button) const {
    return buttonsHeld & mapButton(button);
}

bool InputManager::getButtonDown(Button button) const {
    return buttonsDown & mapButton(button);
}

bool InputManager::getButtonUp(Button button) const {
    return buttonsUp & mapButton(button);
}

Vector2 InputManager::getLeftStick() const {
    HidAnalogStickState stick = padGetStickPos(&pad, 0);
    return {stick.x / 32767.0f, stick.y / 32767.0f};
}

Vector2 InputManager::getRightStick() const {
    HidAnalogStickState stick = padGetStickPos(&pad, 1);
    return {stick.x / 32767.0f, stick.y / 32767.0f};
}

bool InputManager::isTouching() const {
    return isTouchingNow;
}

Vector2 InputManager::getTouchPosition() const {
    if (isTouchingNow && touchState.count > 0) {
        return {(float)touchState.touches[0].x, (float)touchState.touches[0].y};
    }
    return {0, 0};
}

bool InputManager::getTouchDown() const {
    return !wasTouching && isTouchingNow;
}

bool InputManager::getTouchUp() const {
    return wasTouching && !isTouchingNow;
}

Vector2 InputManager::getGyro() const {
    // 读取陀螺仪数据
    hidGetSixAxisSensorStates(sixaxisHandle, const_cast<HidSixAxisSensorState*>(&sixaxisState), 1);
    return {sixaxisState.angular_velocity.x, sixaxisState.angular_velocity.y};
}

} // namespace Switch2D
