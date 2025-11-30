#pragma once

#include <RmlUi/Core.h>
#include <SDL.h>

// ============================================
// RmlUi SDL2 系统接口
// ============================================
// 实现 RmlUi::SystemInterface 用于 SDL2
// 提供时间、剪贴板等系统功能
// ============================================

class RmlUi_Platform_SDL : public Rml::SystemInterface
{
public:
    RmlUi_Platform_SDL();
    virtual ~RmlUi_Platform_SDL();

    // ========================================
    // SystemInterface 必需方法
    // ========================================
    
    // 获取系统时间（秒）
    double GetElapsedTime() override;
    
    // 日志输出
    bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;
    
    // 设置鼠标光标（Switch 上不需要）
    void SetMouseCursor(const Rml::String& cursor_name) override;
    
    // 剪贴板（可选）
    void SetClipboardText(const Rml::String& text) override;
    void GetClipboardText(Rml::String& text) override;
    
    // 激活键盘（Switch 软键盘）
    void ActivateKeyboard(Rml::Vector2f caret_position, float line_height) override;
    void DeactivateKeyboard() override;

    // ========================================
    // 辅助方法
    // ========================================
    
    // 输入转换
    static Rml::Input::KeyIdentifier ConvertKey(SDL_Keycode sdl_key);
    static int GetKeyModifiers(SDL_Keymod sdl_mods);

private:
    Uint64 m_start_time;
};
