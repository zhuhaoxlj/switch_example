#pragma once
#include <SDL.h>

// SDL_ttf 支持（已安装）
#define HAS_SDL_TTF 1
#include <SDL_ttf.h>

#include <string>
#include <vector>
#include <deque>
#include "Math.h"

namespace Switch2D {

// ============================================
// DebugConsole - 游戏内调试控制台
// ============================================
class DebugConsole {
public:
    static DebugConsole& getInstance();
    
    void initialize(SDL_Renderer* renderer, const char* fontPath = nullptr);
    void shutdown();
    
    // 日志功能
    void log(const std::string& message);
    void logf(const char* format, ...);
    void clear();
    
    // 显示控制
    void toggle();
    void show();
    void hide();
    bool isVisible() const { return visible; }
    
    // 更新和渲染
    void update();
    void render();
    
    // 配置
    void setMaxLines(int max) { maxLines = max; }
    void setFontSize(int size);
    void setOpacity(float alpha) { opacity = alpha; }
    bool loadFont(const char* fontPath, int size);
    
private:
    DebugConsole() = default;
    ~DebugConsole() = default;
    
    // 渲染文本（TTF 字体）
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void renderTextFallback(const std::string& text, int x, int y, SDL_Color color);
    void renderChar(char c, int x, int y, SDL_Color color);
    
    SDL_Renderer* renderer = nullptr;
#ifdef HAS_SDL_TTF
    TTF_Font* font = nullptr;
#endif
    std::deque<std::string> logLines;
    
    bool visible = false;
    bool useTTF = false;
    int maxLines = 15;
    int fontSize = 18;
    int lineHeight = 22;
    float opacity = 0.85f;
    
    // 滚动
    int scrollOffset = 0;
};

// 全局日志宏
#define DEBUG_LOG(...) Switch2D::DebugConsole::getInstance().logf(__VA_ARGS__)
#define DEBUG_CLEAR() Switch2D::DebugConsole::getInstance().clear()

} // namespace Switch2D
