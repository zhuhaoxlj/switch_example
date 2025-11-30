#pragma once
#include "Switch2D/Switch2D.h"
#include "RmlUi_Renderer_SDL.h"
#include "RmlUi_Platform_SDL.h"
#include <RmlUi/Core.h>
#include <string>
#include <memory>

using namespace Switch2D;

// ============================================
// RmlUi 浏览器组件
// ============================================
// 使用 RmlUi 引擎渲染 RML/RCSS
// 支持完整的 CSS3 标准和游戏优化
// ============================================

class RmlUiBrowserComponent : public Component
{
public:
    RmlUiBrowserComponent();
    ~RmlUiBrowserComponent();
    
    // 初始化 RmlUi
    bool initialize();
    
    // 加载 RML 文档
    bool loadDocument(const std::string& rml_path);
    bool loadDocumentFromString(const std::string& rml_content);
    
    // 从网络加载 HTML 并转换为 RML（简化版）
    bool loadHTMLFromNetwork(const std::string& url);
    
    // 获取上下文
    Rml::Context* getContext() { return m_context; }
    
    // 组件生命周期
    void onUpdate() override;
    void onRender() override;
    
private:
    // RmlUi 核心
    std::unique_ptr<RmlUi_Renderer_SDL> m_renderer;
    std::unique_ptr<RmlUi_Platform_SDL> m_system;
    Rml::Context* m_context;
    Rml::ElementDocument* m_document;
    
    bool m_initialized;
    
    // 输入处理
    void handleInput();
    void handleTouchInput();
    void handleGamepadInput();
    
    // HTML 到 RML 的简单转换
    std::string convertHTMLToRML(const std::string& html);
};

// ============================================
// 浏览器场景 (RmlUi 版本)
// ============================================
class BrowserSceneRmlUi : public Scene
{
public:
    BrowserSceneRmlUi() : Scene("Browser Scene - RmlUi Powered") {}
    
    void onLoad() override;
    void onUpdate() override;
    
private:
    GameObject* browserObject = nullptr;
    RmlUiBrowserComponent* browser = nullptr;
    
    // 加载不同的页面
    void loadSamplePage();
    void loadNetworkPage(const std::string& url);
    void loadDemoUI();
};
