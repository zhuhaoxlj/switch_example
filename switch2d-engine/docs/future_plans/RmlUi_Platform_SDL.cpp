#include "RmlUi_Platform_SDL.h"
#include <cstdio>

RmlUi_Platform_SDL::RmlUi_Platform_SDL()
{
    m_start_time = SDL_GetPerformanceCounter();
}

RmlUi_Platform_SDL::~RmlUi_Platform_SDL()
{
}

double RmlUi_Platform_SDL::GetElapsedTime()
{
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 freq = SDL_GetPerformanceFrequency();
    return (double)(now - m_start_time) / (double)freq;
}

bool RmlUi_Platform_SDL::LogMessage(Rml::Log::Type type, const Rml::String& message)
{
    const char* type_str = "";
    switch (type)
    {
        case Rml::Log::LT_ALWAYS:
            type_str = "[RmlUi]";
            break;
        case Rml::Log::LT_ERROR:
            type_str = "[RmlUi ERROR]";
            break;
        case Rml::Log::LT_ASSERT:
            type_str = "[RmlUi ASSERT]";
            break;
        case Rml::Log::LT_WARNING:
            type_str = "[RmlUi WARNING]";
            break;
        case Rml::Log::LT_INFO:
            type_str = "[RmlUi INFO]";
            break;
        case Rml::Log::LT_DEBUG:
            type_str = "[RmlUi DEBUG]";
            break;
        default:
            type_str = "[RmlUi]";
            break;
    }

    printf("%s %s\n", type_str, message.c_str());
    return true;
}

void RmlUi_Platform_SDL::SetMouseCursor(const Rml::String& cursor_name)
{
    // Switch 使用触摸屏，不需要鼠标光标
    // 可以在这里添加自定义光标显示逻辑
}

void RmlUi_Platform_SDL::SetClipboardText(const Rml::String& text)
{
    SDL_SetClipboardText(text.c_str());
}

void RmlUi_Platform_SDL::GetClipboardText(Rml::String& text)
{
    if (SDL_HasClipboardText())
    {
        char* clipboard = SDL_GetClipboardText();
        if (clipboard)
        {
            text = clipboard;
            SDL_free(clipboard);
        }
    }
}

void RmlUi_Platform_SDL::ActivateKeyboard(Rml::Vector2f caret_position, float line_height)
{
    // 在 Switch 上可以激活软键盘
    // 这里可以添加 Switch 软键盘的调用
    // 例如使用 swkbd applet
}

void RmlUi_Platform_SDL::DeactivateKeyboard()
{
    // 关闭软键盘
}

// 键盘按键转换
Rml::Input::KeyIdentifier RmlUi_Platform_SDL::ConvertKey(SDL_Keycode sdl_key)
{
    switch (sdl_key)
    {
        case SDLK_UNKNOWN: return Rml::Input::KI_UNKNOWN;
        case SDLK_ESCAPE: return Rml::Input::KI_ESCAPE;
        case SDLK_SPACE: return Rml::Input::KI_SPACE;
        case SDLK_0: return Rml::Input::KI_0;
        case SDLK_1: return Rml::Input::KI_1;
        case SDLK_2: return Rml::Input::KI_2;
        case SDLK_3: return Rml::Input::KI_3;
        case SDLK_4: return Rml::Input::KI_4;
        case SDLK_5: return Rml::Input::KI_5;
        case SDLK_6: return Rml::Input::KI_6;
        case SDLK_7: return Rml::Input::KI_7;
        case SDLK_8: return Rml::Input::KI_8;
        case SDLK_9: return Rml::Input::KI_9;
        case SDLK_a: return Rml::Input::KI_A;
        case SDLK_b: return Rml::Input::KI_B;
        case SDLK_c: return Rml::Input::KI_C;
        case SDLK_d: return Rml::Input::KI_D;
        case SDLK_e: return Rml::Input::KI_E;
        case SDLK_f: return Rml::Input::KI_F;
        case SDLK_g: return Rml::Input::KI_G;
        case SDLK_h: return Rml::Input::KI_H;
        case SDLK_i: return Rml::Input::KI_I;
        case SDLK_j: return Rml::Input::KI_J;
        case SDLK_k: return Rml::Input::KI_K;
        case SDLK_l: return Rml::Input::KI_L;
        case SDLK_m: return Rml::Input::KI_M;
        case SDLK_n: return Rml::Input::KI_N;
        case SDLK_o: return Rml::Input::KI_O;
        case SDLK_p: return Rml::Input::KI_P;
        case SDLK_q: return Rml::Input::KI_Q;
        case SDLK_r: return Rml::Input::KI_R;
        case SDLK_s: return Rml::Input::KI_S;
        case SDLK_t: return Rml::Input::KI_T;
        case SDLK_u: return Rml::Input::KI_U;
        case SDLK_v: return Rml::Input::KI_V;
        case SDLK_w: return Rml::Input::KI_W;
        case SDLK_x: return Rml::Input::KI_X;
        case SDLK_y: return Rml::Input::KI_Y;
        case SDLK_z: return Rml::Input::KI_Z;
        case SDLK_BACKSPACE: return Rml::Input::KI_BACK;
        case SDLK_TAB: return Rml::Input::KI_TAB;
        case SDLK_RETURN: return Rml::Input::KI_RETURN;
        case SDLK_PAUSE: return Rml::Input::KI_PAUSE;
        case SDLK_CAPSLOCK: return Rml::Input::KI_CAPITAL;
        case SDLK_PAGEUP: return Rml::Input::KI_PRIOR;
        case SDLK_PAGEDOWN: return Rml::Input::KI_NEXT;
        case SDLK_END: return Rml::Input::KI_END;
        case SDLK_HOME: return Rml::Input::KI_HOME;
        case SDLK_LEFT: return Rml::Input::KI_LEFT;
        case SDLK_UP: return Rml::Input::KI_UP;
        case SDLK_RIGHT: return Rml::Input::KI_RIGHT;
        case SDLK_DOWN: return Rml::Input::KI_DOWN;
        case SDLK_INSERT: return Rml::Input::KI_INSERT;
        case SDLK_DELETE: return Rml::Input::KI_DELETE;
        case SDLK_F1: return Rml::Input::KI_F1;
        case SDLK_F2: return Rml::Input::KI_F2;
        case SDLK_F3: return Rml::Input::KI_F3;
        case SDLK_F4: return Rml::Input::KI_F4;
        case SDLK_F5: return Rml::Input::KI_F5;
        case SDLK_F6: return Rml::Input::KI_F6;
        case SDLK_F7: return Rml::Input::KI_F7;
        case SDLK_F8: return Rml::Input::KI_F8;
        case SDLK_F9: return Rml::Input::KI_F9;
        case SDLK_F10: return Rml::Input::KI_F10;
        case SDLK_F11: return Rml::Input::KI_F11;
        case SDLK_F12: return Rml::Input::KI_F12;
        case SDLK_NUMLOCKCLEAR: return Rml::Input::KI_NUMLOCK;
        case SDLK_SCROLLLOCK: return Rml::Input::KI_SCROLL;
        case SDLK_LSHIFT: return Rml::Input::KI_LSHIFT;
        case SDLK_RSHIFT: return Rml::Input::KI_RSHIFT;
        case SDLK_LCTRL: return Rml::Input::KI_LCONTROL;
        case SDLK_RCTRL: return Rml::Input::KI_RCONTROL;
        case SDLK_LALT: return Rml::Input::KI_LMENU;
        case SDLK_RALT: return Rml::Input::KI_RMENU;
        default: return Rml::Input::KI_UNKNOWN;
    }
}

int RmlUi_Platform_SDL::GetKeyModifiers(SDL_Keymod sdl_mods)
{
    int modifiers = 0;

    if (sdl_mods & KMOD_CTRL)
        modifiers |= Rml::Input::KM_CTRL;
    if (sdl_mods & KMOD_SHIFT)
        modifiers |= Rml::Input::KM_SHIFT;
    if (sdl_mods & KMOD_ALT)
        modifiers |= Rml::Input::KM_ALT;
    if (sdl_mods & KMOD_GUI)
        modifiers |= Rml::Input::KM_META;
    if (sdl_mods & KMOD_NUM)
        modifiers |= Rml::Input::KM_NUMLOCK;
    if (sdl_mods & KMOD_CAPS)
        modifiers |= Rml::Input::KM_CAPSLOCK;

    return modifiers;
}
