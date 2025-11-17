#pragma once
#include <SDL.h>
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
    
    void initialize(SDL_Renderer* renderer);
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
    void setFontSize(int size) { fontSize = size; }
    void setOpacity(float alpha) { opacity = alpha; }
    
private:
    DebugConsole() = default;
    ~DebugConsole() = default;
    
    // 渲染文本（简单的像素字体）
    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void renderChar(char c, int x, int y, SDL_Color color);
    
    SDL_Renderer* renderer = nullptr;
    std::deque<std::string> logLines;
    
    bool visible = false;
    int maxLines = 20;
    int fontSize = 8;
    float opacity = 0.85f;
    
    // 滚动
    int scrollOffset = 0;
};

// 全局日志宏
#define DEBUG_LOG(...) Switch2D::DebugConsole::getInstance().logf(__VA_ARGS__)
#define DEBUG_CLEAR() Switch2D::DebugConsole::getInstance().clear()

} // namespace Switch2D
