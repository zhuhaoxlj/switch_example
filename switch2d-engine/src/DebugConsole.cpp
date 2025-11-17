#include "Switch2D/DebugConsole.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <algorithm>

namespace Switch2D {

// 简单的 3x5 像素字体（位图表示）
// 每个字符用一个 5 元素数组表示，每个元素是一行的位图（3位）
static const unsigned char FONT_3x5[][5] = {
    {0b111, 0b101, 0b101, 0b101, 0b111}, // 0
    {0b010, 0b110, 0b010, 0b010, 0b111}, // 1
    {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
    {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
    {0b101, 0b101, 0b111, 0b001, 0b001}, // 4
    {0b111, 0b100, 0b111, 0b001, 0b111}, // 5
    {0b111, 0b100, 0b111, 0b101, 0b111}, // 6
    {0b111, 0b001, 0b010, 0b010, 0b010}, // 7
    {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
    {0b111, 0b101, 0b111, 0b001, 0b111}, // 9
    {0b010, 0b101, 0b111, 0b101, 0b101}, // A
    {0b110, 0b101, 0b110, 0b101, 0b110}, // B
    {0b111, 0b100, 0b100, 0b100, 0b111}, // C
    {0b110, 0b101, 0b101, 0b101, 0b110}, // D
    {0b111, 0b100, 0b111, 0b100, 0b111}, // E
    {0b111, 0b100, 0b111, 0b100, 0b100}, // F
    {0b111, 0b100, 0b101, 0b101, 0b111}, // G
    {0b101, 0b101, 0b111, 0b101, 0b101}, // H
    {0b111, 0b010, 0b010, 0b010, 0b111}, // I
    {0b111, 0b001, 0b001, 0b101, 0b111}, // J
    {0b101, 0b110, 0b100, 0b110, 0b101}, // K
    {0b100, 0b100, 0b100, 0b100, 0b111}, // L
    {0b101, 0b111, 0b111, 0b101, 0b101}, // M
    {0b101, 0b111, 0b111, 0b111, 0b101}, // N
    {0b111, 0b101, 0b101, 0b101, 0b111}, // O
    {0b111, 0b101, 0b111, 0b100, 0b100}, // P
    {0b111, 0b101, 0b101, 0b111, 0b001}, // Q
    {0b111, 0b101, 0b110, 0b101, 0b101}, // R
    {0b111, 0b100, 0b111, 0b001, 0b111}, // S
    {0b111, 0b010, 0b010, 0b010, 0b010}, // T
    {0b101, 0b101, 0b101, 0b101, 0b111}, // U
    {0b101, 0b101, 0b101, 0b101, 0b010}, // V
    {0b101, 0b101, 0b111, 0b111, 0b101}, // W
    {0b101, 0b101, 0b010, 0b101, 0b101}, // X
    {0b101, 0b101, 0b010, 0b010, 0b010}, // Y
    {0b111, 0b001, 0b010, 0b100, 0b111}, // Z
};

DebugConsole& DebugConsole::getInstance() {
    static DebugConsole instance;
    return instance;
}

void DebugConsole::initialize(SDL_Renderer* r, const char* fontPath) {
    renderer = r;
    logLines.clear();
    scrollOffset = 0;
    
#ifdef HAS_SDL_TTF
    // 初始化 SDL_ttf
    if (TTF_Init() == -1) {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        useTTF = false;
        log("[ERROR] TTF_Init failed - using pixel font");
        return;
    }
    
    // 尝试加载字体
    log("=== FONT INIT ===");
    log("Note: Need romfsInit() first!");
    
    if (fontPath) {
        // 尝试多个可能的路径格式
        const char* tryPaths[] = {
            fontPath,  // 原始路径
            "/fonts/LXGWWenKai-Regular.ttf"
        };
        
        bool loaded = false;
        for (int i = 0; i < 4; i++) {
            logf("Try #%d: %s", i+1, tryPaths[i]);
            
            if (loadFont(tryPaths[i], fontSize)) {
                printf("✅ Loaded TTF font: %s\n", tryPaths[i]);
                log("SUCCESS!");
                logf("Size: %d pt", fontSize);
                loaded = true;
                break;
            } else {
                log("Failed");
            }
        }
        
        if (!loaded) {
            log("ALL ATTEMPTS FAILED");
            log("Using pixel font");
        }
    } else {
        log("No font path provided");
    }
#else
    printf("SDL_ttf not available, using fallback pixel font\n");
    useTTF = false;
    log("[INFO] HAS_SDL_TTF not defined - using pixel font");
#endif
}

void DebugConsole::shutdown() {
#ifdef HAS_SDL_TTF
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();
#endif
    logLines.clear();
}

void DebugConsole::log(const std::string& message) {
    // 添加时间戳
    time_t now = time(nullptr);
    struct tm* tm_info = localtime(&now);
    char timeStr[16];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm_info);
    
    std::string fullMsg = std::string("[") + timeStr + "] " + message;
    
    // 分割多行
    size_t start = 0;
    size_t end = fullMsg.find('\n');
    
    while (end != std::string::npos) {
        logLines.push_back(fullMsg.substr(start, end - start));
        start = end + 1;
        end = fullMsg.find('\n', start);
    }
    
    if (start < fullMsg.length()) {
        logLines.push_back(fullMsg.substr(start));
    }
    
    // 限制最大行数
    while (logLines.size() > static_cast<size_t>(maxLines * 2)) {
        logLines.pop_front();
    }
    
    // 同时输出到标准输出
    printf("%s\n", message.c_str());
}

void DebugConsole::logf(const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    log(buffer);
}

void DebugConsole::clear() {
    logLines.clear();
    scrollOffset = 0;
}

void DebugConsole::toggle() {
    visible = !visible;
}

void DebugConsole::show() {
    visible = true;
}

void DebugConsole::hide() {
    visible = false;
}

void DebugConsole::update() {
    // 可以在这里添加滚动逻辑
}

bool DebugConsole::loadFont(const char* fontPath, int size) {
#ifdef HAS_SDL_TTF
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    
    // 先检查文件是否存在
    FILE* testFile = fopen(fontPath, "rb");
    if (!testFile) {
        printf("Font file not found: %s\n", fontPath);
        useTTF = false;
        return false;
    }
    fclose(testFile);
    printf("Font file exists: %s\n", fontPath);
    
    font = TTF_OpenFont(fontPath, size);
    if (!font) {
        const char* error = TTF_GetError();
        printf("Failed to load font %s: %s\n", fontPath, error);
        useTTF = false;
        return false;
    }
    
    useTTF = true;
    fontSize = size;
    lineHeight = TTF_FontLineSkip(font);
    printf("Font loaded successfully, line height: %d\n", lineHeight);
    return true;
#else
    printf("SDL_ttf not available\n");
    return false;
#endif
}

void DebugConsole::setFontSize(int size) {
    fontSize = size;
#ifdef HAS_SDL_TTF
    if (useTTF && font) {
        // 需要重新加载字体以更改大小
        // 这里保存当前字体路径会更好，但暂时跳过
        lineHeight = size + 4;
    }
#endif
}

void DebugConsole::renderChar(char c, int x, int y, SDL_Color color) {
    if (!renderer) return;
    
    int index = -1;
    
    // 映射字符到字体索引
    if (c >= '0' && c <= '9') {
        index = c - '0';
    } else if (c >= 'A' && c <= 'Z') {
        index = 10 + (c - 'A');
    } else if (c >= 'a' && c <= 'z') {
        index = 10 + (c - 'a');
    } else if (c == ' ') {
        return; // 空格不绘制
    } else {
        // 其他字符显示为点
        SDL_Rect rect = {x + 1, y + 2, 1, 1};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
        return;
    }
    
    if (index < 0 || index >= 36) return;
    
    // 渲染字符（3x5像素）
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (int row = 0; row < 5; row++) {
        unsigned char line = FONT_3x5[index][row];
        for (int col = 0; col < 3; col++) {
            if (line & (1 << (2 - col))) {
                SDL_Rect pixel = {x + col, y + row, 1, 1};
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
}

void DebugConsole::renderTextFallback(const std::string& text, int x, int y, SDL_Color color) {
    int curX = x;
    int curY = y;
    
    for (char c : text) {
        if (c == '\n') {
            curY += 7;
            curX = x;
            continue;
        }
        
        renderChar(c, curX, curY, color);
        curX += 4; // 字符宽度 + 间距
        
        // 换行处理
        if (curX > 1270) {
            curY += 7;
            curX = x;
        }
    }
}

void DebugConsole::renderText(const std::string& text, int x, int y, SDL_Color color) {
    if (!renderer) return;
    
    // 只在第一次渲染时记录状态
    static bool firstRender = true;
    if (firstRender) {
#ifdef HAS_SDL_TTF
        printf("🎨 First render - useTTF=%s, font=%p\n", useTTF ? "true" : "false", (void*)font);
        if (useTTF && font) {
            log("=== RENDERING: TTF ===");
        } else {
            log("=== RENDERING: PIXEL ===");
        }
#else
        printf("🎨 First render - HAS_SDL_TTF not defined\n");
        log("=== NO SDL_TTF ===");
#endif
        firstRender = false;
    }
    
#ifdef HAS_SDL_TTF
    // 使用 TTF 字体
    if (useTTF && font) {
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect dstRect = {x, y, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    } else
#endif
    {
        // 回退到像素字体
        renderTextFallback(text, x, y, color);
    }
}

void DebugConsole::render() {
    if (!visible || !renderer) return;
    
    int actualLineHeight = useTTF ? lineHeight : 7;
    int headerHeight = useTTF ? lineHeight + 10 : 22;
    
    // 绘制半透明背景
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(opacity * 255));
    SDL_Rect bg = {5, 5, 1270, headerHeight + maxLines * actualLineHeight + 10};
    SDL_RenderFillRect(renderer, &bg);
    
    // 绘制边框
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
    SDL_RenderDrawRect(renderer, &bg);
    
    // 绘制标题
    SDL_Color titleColor = {100, 200, 255, 255};
    renderText("DEBUG CONSOLE - Press A+B to toggle", 10, 10, titleColor);
    
    // 绘制分隔线
    int separatorY = useTTF ? 10 + lineHeight + 2 : 20;
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 128);
    SDL_RenderDrawLine(renderer, 10, separatorY, 1270, separatorY);
    
    // 绘制日志
    SDL_Color textColor = {255, 255, 255, 255};
    int startLine = (logLines.size() > static_cast<size_t>(maxLines)) 
                    ? logLines.size() - maxLines : 0;
    
    int yPos = separatorY + 5;
    for (size_t i = startLine; i < logLines.size(); i++) {
        renderText(logLines[i], 10, yPos, textColor);
        yPos += actualLineHeight;
    }
    
    // 如果有更多日志，显示指示器
    if (logLines.size() > static_cast<size_t>(maxLines)) {
        SDL_Color indicatorColor = {255, 255, 0, 255};
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "(%zu more lines above...)", 
                 logLines.size() - maxLines);
        renderText(buffer, 10, yPos, indicatorColor);
    }
}

} // namespace Switch2D
