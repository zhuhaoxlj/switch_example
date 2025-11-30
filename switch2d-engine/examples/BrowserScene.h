#pragma once
#include "Switch2D/Switch2D.h"
#include "litehtml_container_sdl.h"
#include "litehtml/master_css.h"
#include <string>
#include <memory>

using namespace Switch2D;

// ============================================
// Litehtml 浏览器组件
// ============================================
// 使用真正的 litehtml 引擎渲染 HTML/CSS
// 支持完整的 HTML5/CSS3 标准
// ============================================

class LitehtmlBrowserComponent : public Component
{
public:
    LitehtmlBrowserComponent();
    ~LitehtmlBrowserComponent();
    
    // 加载 HTML 内容
    bool loadHTML(const char* html);
    bool loadHTMLFromNetwork(const std::string& url);
    
    // 渲染和绘制
    void render(int max_width);
    void draw(int x, int y);
    
    // 获取文档尺寸
    int getWidth() const { return m_doc_width; }
    int getHeight() const { return m_doc_height; }
    
    // 滚动控制
    void setScrollOffset(float offset) { m_scroll_offset = offset; }
    float getScrollOffset() const { return m_scroll_offset; }
    float getMaxScrollOffset() const { return m_max_scroll_offset; }
    
    void onUpdate() override;
    void onRender() override;
    
private:
    litehtml_container_sdl* m_container;
    litehtml::document::ptr m_document;
    
    int m_doc_width = 0;
    int m_doc_height = 0;
    int m_best_width = 0;
    float m_scroll_offset = 0;
    float m_max_scroll_offset = 0;
    
    void handleInput();
};

// ============================================
// 浏览器场景
// ============================================
class BrowserScene : public Scene
{
public:
    BrowserScene() : Scene("Browser Scene - Litehtml Powered") {}
    
    void onLoad() override;
    void onUpdate() override;
    
private:
    GameObject *browserObject = nullptr;
    LitehtmlBrowserComponent *browser = nullptr;
    
    // 示例 HTML 内容
    void loadSamplePage();
    void loadNetworkPage(const std::string &url);
};
