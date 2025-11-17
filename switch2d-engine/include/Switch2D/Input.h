#pragma once
#include <switch.h>
#include <SDL.h>
#include <unordered_map>
#include "Math.h"

namespace Switch2D {

// ============================================
// 按钮枚举
// ============================================
enum class Button {
    A, B, X, Y,
    L, R, ZL, ZR,
    Plus, Minus,
    Left, Right, Up, Down,
    LeftStick, RightStick
};

// ============================================
// InputManager - 输入管理器
// ============================================
class InputManager {
public:
    InputManager();
    ~InputManager();
    
    void update();
    
    // 按钮输入
    bool getButton(Button button) const;
    bool getButtonDown(Button button) const;
    bool getButtonUp(Button button) const;
    
    // 摇杆输入
    Vector2 getLeftStick() const;
    Vector2 getRightStick() const;
    
    // 触摸输入
    bool isTouching() const;
    Vector2 getTouchPosition() const;
    bool getTouchDown() const;
    bool getTouchUp() const;
    
    // 陀螺仪
    Vector2 getGyro() const;
    
private:
    PadState pad;
    
    u64 buttonsDown = 0;
    u64 buttonsUp = 0;
    u64 buttonsHeld = 0;
    
    HidTouchScreenState touchState;
    bool wasTouching = false;
    bool isTouchingNow = false;
    
    HidSixAxisSensorHandle sixaxisHandle;
    HidSixAxisSensorState sixaxisState;
    
    HidNpadButton mapButton(Button button) const;
};

} // namespace Switch2D
