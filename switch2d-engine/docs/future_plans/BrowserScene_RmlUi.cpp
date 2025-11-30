#include "BrowserScene_RmlUi.h"
#include <algorithm>
#include <regex>

// ============================================
// RmlUiBrowserComponent 实现
// ============================================

RmlUiBrowserComponent::RmlUiBrowserComponent()
    : m_context(nullptr)
    , m_document(nullptr)
    , m_initialized(false)
{
    DEBUG_LOG("[RmlUi] Browser component created");
}

RmlUiBrowserComponent::~RmlUiBrowserComponent()
{
    if (m_context)
    {
        m_context->UnloadAllDocuments();
        Rml::RemoveContext(m_context->GetName());
    }
    
    if (m_initialized)
    {
        Rml::Shutdown();
    }
    
    DEBUG_LOG("[RmlUi] Browser component destroyed");
}

bool RmlUiBrowserComponent::initialize()
{
    if (m_initialized)
        return true;

    SDL_Renderer* renderer = Engine::getInstance().getRenderer();
    
    // 创建渲染器和系统接口
    m_renderer = std::make_unique<RmlUi_Renderer_SDL>(renderer, 1280, 720);
    m_system = std::make_unique<RmlUi_Platform_SDL>();
    
    // 设置 RmlUi 接口
    Rml::SetRenderInterface(m_renderer.get());
    Rml::SetSystemInterface(m_system.get());
    
    // 初始化 RmlUi
    if (!Rml::Initialise())
    {
        DEBUG_LOG("[RmlUi] Failed to initialize!");
        return false;
    }
    
    // 创建上下文
    m_context = Rml::CreateContext("main", Rml::Vector2i(1280, 720));
    if (!m_context)
    {
        DEBUG_LOG("[RmlUi] Failed to create context!");
        Rml::Shutdown();
        return false;
    }
    
    // 加载默认字体
    if (!Rml::LoadFontFace("romfs:/fonts/LXGWWenKai-Regular.ttf"))
    {
        DEBUG_LOG("[RmlUi] Warning: Failed to load font, using fallback");
    }
    
    m_initialized = true;
    DEBUG_LOG("[RmlUi] Initialized successfully!");
    
    return true;
}

bool RmlUiBrowserComponent::loadDocument(const std::string& rml_path)
{
    if (!m_initialized || !m_context)
    {
        DEBUG_LOG("[RmlUi] Not initialized!");
        return false;
    }
    
    // 卸载旧文档
    if (m_document)
    {
        m_document->Close();
        m_document = nullptr;
    }
    
    // 加载新文档
    m_document = m_context->LoadDocument(rml_path);
    if (!m_document)
    {
        DEBUG_LOG("[RmlUi] Failed to load document: %s", rml_path.c_str());
        return false;
    }
    
    m_document->Show();
    DEBUG_LOG("[RmlUi] Document loaded: %s", rml_path.c_str());
    
    return true;
}

bool RmlUiBrowserComponent::loadDocumentFromString(const std::string& rml_content)
{
    if (!m_initialized || !m_context)
    {
        DEBUG_LOG("[RmlUi] Not initialized!");
        return false;
    }
    
    // 卸载旧文档
    if (m_document)
    {
        m_document->Close();
        m_document = nullptr;
    }
    
    // 从字符串加载
    m_document = m_context->LoadDocumentFromMemory(rml_content);
    if (!m_document)
    {
        DEBUG_LOG("[RmlUi] Failed to load document from string");
        return false;
    }
    
    m_document->Show();
    DEBUG_LOG("[RmlUi] Document loaded from string (%zu bytes)", rml_content.size());
    
    return true;
}

bool RmlUiBrowserComponent::loadHTMLFromNetwork(const std::string& url)
{
    DEBUG_LOG("[RmlUi] Loading page: %s", url.c_str());
    
    NetworkManager* net = Engine::getInstance().getNetwork();
    
    if (!net->isInitialized())
    {
        DEBUG_LOG("[RmlUi] ERROR: Network not initialized!");
        return false;
    }
    
    HTTPResponse resp = net->get(url);
    
    if (resp.isSuccess())
    {
        std::string html = resp.getBodyAsString();
        DEBUG_LOG("[RmlUi] Downloaded %zu bytes", html.length());
        
        // 简单的 HTML 到 RML 转换
        std::string rml = convertHTMLToRML(html);
        return loadDocumentFromString(rml);
    }
    else
    {
        DEBUG_LOG("[RmlUi] Failed to load page: %s", resp.error.c_str());
        return false;
    }
}

std::string RmlUiBrowserComponent::convertHTMLToRML(const std::string& html)
{
    // 这是一个简化的 HTML 到 RML 转换
    // RmlUi 的 RML 格式与 HTML 非常相似，大部分标签可以直接使用
    
    std::string rml = html;
    
    // 替换 DOCTYPE
    rml = std::regex_replace(rml, std::regex("<!DOCTYPE[^>]*>"), 
        "<rml>\n<head>\n<title>Browser Page</title>\n</head>\n<body>");
    
    // 添加结束标签（如果缺少）
    if (rml.find("</html>") == std::string::npos)
    {
        rml += "\n</body>\n</rml>";
    }
    else
    {
        rml = std::regex_replace(rml, std::regex("</html>"), "</body>\n</rml>");
    }
    
    // 替换 <html> 标签
    rml = std::regex_replace(rml, std::regex("<html[^>]*>"), "");
    
    return rml;
}

void RmlUiBrowserComponent::handleInput()
{
    if (!m_context)
        return;
    
    handleTouchInput();
    handleGamepadInput();
}

void RmlUiBrowserComponent::handleTouchInput()
{
    InputManager* input = Engine::getInstance().getInput();
    
    if (input->isTouching())
    {
        Vector2 touchPos = input->getTouchPosition();
        
        // 发送鼠标移动事件
        m_context->ProcessMouseMove((int)touchPos.x, (int)touchPos.y, 0);
        
        // 发送触摸按下/释放事件
        if (input->getTouchDown())
        {
            m_context->ProcessMouseButtonDown(0, 0);
            DEBUG_LOG("[RmlUi] Touch down at: (%.0f, %.0f)", touchPos.x, touchPos.y);
        }
        else if (input->getTouchUp())
        {
            m_context->ProcessMouseButtonUp(0, 0);
            DEBUG_LOG("[RmlUi] Touch up");
        }
    }
}

void RmlUiBrowserComponent::handleGamepadInput()
{
    InputManager* input = Engine::getInstance().getInput();
    
    // 方向键导航
    if (input->getButtonDown(Button::Up))
    {
        m_context->ProcessKeyDown(Rml::Input::KI_UP, 0);
        m_context->ProcessKeyUp(Rml::Input::KI_UP, 0);
    }
    if (input->getButtonDown(Button::Down))
    {
        m_context->ProcessKeyDown(Rml::Input::KI_DOWN, 0);
        m_context->ProcessKeyUp(Rml::Input::KI_DOWN, 0);
    }
    if (input->getButtonDown(Button::Left))
    {
        m_context->ProcessKeyDown(Rml::Input::KI_LEFT, 0);
        m_context->ProcessKeyUp(Rml::Input::KI_LEFT, 0);
    }
    if (input->getButtonDown(Button::Right))
    {
        m_context->ProcessKeyDown(Rml::Input::KI_RIGHT, 0);
        m_context->ProcessKeyUp(Rml::Input::KI_RIGHT, 0);
    }
    
    // A 键确认（模拟回车）
    if (input->getButtonDown(Button::A))
    {
        m_context->ProcessKeyDown(Rml::Input::KI_RETURN, 0);
        m_context->ProcessKeyUp(Rml::Input::KI_RETURN, 0);
    }
}

void RmlUiBrowserComponent::onUpdate()
{
    if (!m_context)
        return;
    
    handleInput();
    
    // 更新上下文
    m_context->Update();
}

void RmlUiBrowserComponent::onRender()
{
    if (!m_context)
        return;
    
    // 开始渲染帧
    m_renderer->BeginFrame();
    
    // 渲染上下文
    m_context->Render();
    
    // 结束渲染帧
    m_renderer->EndFrame();
}

// ============================================
// BrowserSceneRmlUi 实现
// ============================================

void BrowserSceneRmlUi::onLoad()
{
    DEBUG_LOG("=== Browser Scene (RmlUi Powered) ===");
    DEBUG_LOG("🚀 Modern HTML/CSS rendering with RmlUi");
    DEBUG_LOG("Press B to return to main menu");
    DEBUG_LOG("Use Touch or D-Pad to interact");
    DEBUG_LOG("Press X to load example, Y to load network page");
    
    // 创建摄像机
    GameObject* cameraObj = createGameObject("Camera");
    Camera* camera = cameraObj->addComponent<Camera>();
    camera->backgroundColor = Color(15, 15, 25);
    
    // 创建浏览器对象
    browserObject = createGameObject("RmlUi Browser");
    browser = browserObject->addComponent<RmlUiBrowserComponent>();
    
    // 初始化 RmlUi
    if (!browser->initialize())
    {
        DEBUG_LOG("ERROR: Failed to initialize RmlUi!");
        return;
    }
    
    // 加载示例页面
    loadDemoUI();
    
    DEBUG_LOG("Browser Scene loaded successfully!");
    DEBUG_LOG("");
    DEBUG_LOG("=== Powered by RmlUi ===");
    DEBUG_LOG("✨ CSS3 Animations & Transforms");
    DEBUG_LOG("📐 Flexbox Layout Engine");
    DEBUG_LOG("🎮 Gamepad Navigation");
    DEBUG_LOG("⚡ Optimized for Switch");
}

void BrowserSceneRmlUi::loadDemoUI()
{
    // 创建一个漂亮的 RML 演示页面
    const char* rml = R"(
<rml>
<head>
    <title>RmlUi Demo</title>
    <style>
        body {
            font-family: sans-serif;
            background: #1a1a2e;
            color: #eee;
            padding: 40px;
            margin: 0;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        h1 {
            font-size: 56px;
            text-align: center;
            color: #00d4ff;
            margin-bottom: 20px;
            text-shadow: 0 0 20px rgba(0, 212, 255, 0.5);
        }
        
        .subtitle {
            text-align: center;
            font-size: 24px;
            color: #aaa;
            margin-bottom: 40px;
        }
        
        .feature-grid {
            display: flex;
            flex-wrap: wrap;
            gap: 20px;
            margin: 30px 0;
        }
        
        .feature-card {
            flex: 1 1 calc(50% - 10px);
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.3);
            transition: transform 0.3s ease;
        }
        
        .feature-card:hover {
            transform: translateY(-5px);
        }
        
        .feature-card:focus {
            transform: translateY(-5px);
            box-shadow: 0 0 0 3px #00d4ff;
        }
        
        .feature-card h3 {
            font-size: 28px;
            margin: 0 0 15px 0;
            color: #fff;
        }
        
        .feature-card p {
            font-size: 16px;
            line-height: 1.6;
            color: #eee;
        }
        
        .highlight-box {
            background: rgba(0, 212, 255, 0.1);
            border-left: 4px solid #00d4ff;
            padding: 20px;
            margin: 30px 0;
            border-radius: 5px;
        }
        
        .highlight-box p {
            margin: 0;
            font-size: 18px;
        }
        
        .button-group {
            display: flex;
            gap: 15px;
            margin: 30px 0;
            justify-content: center;
        }
        
        button {
            background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
            color: white;
            border: none;
            padding: 15px 30px;
            font-size: 18px;
            border-radius: 25px;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 5px 15px rgba(245, 87, 108, 0.3);
        }
        
        button:hover {
            transform: scale(1.05);
            box-shadow: 0 8px 25px rgba(245, 87, 108, 0.5);
        }
        
        button:focus {
            transform: scale(1.05);
            box-shadow: 0 0 0 3px #00d4ff;
        }
        
        .footer {
            text-align: center;
            margin-top: 50px;
            padding-top: 30px;
            border-top: 1px solid rgba(255, 255, 255, 0.1);
            color: #888;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🎮 RmlUi on Switch</h1>
        <div class="subtitle">Modern UI Rendering with Full CSS3 Support</div>
        
        <div class="highlight-box">
            <p><strong>🎉 Success!</strong> This page is rendered using RmlUi - a powerful HTML/CSS engine optimized for games!</p>
        </div>
        
        <div class="feature-grid">
            <div class="feature-card" tabindex="0">
                <h3>⚡ High Performance</h3>
                <p>Optimized rendering engine designed specifically for real-time applications and games.</p>
            </div>
            
            <div class="feature-card" tabindex="0">
                <h3>🎨 CSS3 Styling</h3>
                <p>Full support for modern CSS3 including gradients, shadows, transforms, and animations!</p>
            </div>
            
            <div class="feature-card" tabindex="0">
                <h3>📐 Flexbox Layout</h3>
                <p>Responsive layouts with CSS Flexbox - like this grid you're looking at!</p>
            </div>
            
            <div class="feature-card" tabindex="0">
                <h3>🎮 Gamepad Ready</h3>
                <p>Native support for gamepad navigation with focus states and spatial navigation.</p>
            </div>
        </div>
        
        <div class="button-group">
            <button tabindex="0">Load Example</button>
            <button tabindex="0">Network Page</button>
            <button tabindex="0">Settings</button>
        </div>
        
        <div class="highlight-box">
            <p><strong>💡 Controls:</strong> Use D-Pad to navigate • A to select • B to go back</p>
        </div>
        
        <div class="footer">
            <p>Powered by <strong>RmlUi</strong> • Running on <strong>Nintendo Switch</strong></p>
            <p>Built with Switch2D Engine</p>
        </div>
    </div>
</body>
</rml>
)";

    if (browser)
    {
        browser->loadDocumentFromString(rml);
        DEBUG_LOG("[BrowserScene] Demo UI loaded successfully!");
    }
}

void BrowserSceneRmlUi::loadSamplePage()
{
    // 可以加载一个 .rml 文件
    if (browser)
    {
        browser->loadDocument("romfs:/rml/sample.rml");
    }
}

void BrowserSceneRmlUi::loadNetworkPage(const std::string& url)
{
    if (browser)
    {
        browser->loadHTMLFromNetwork(url);
    }
}

void BrowserSceneRmlUi::onUpdate()
{
    InputManager* input = Engine::getInstance().getInput();
    
    // B 键返回主菜单
    if (input->getButtonDown(Button::B))
    {
        DEBUG_LOG("Returning to main menu...");
        extern Scene* createDemoScene();
        Engine::getInstance().loadScene(std::unique_ptr<Switch2D::Scene>(createDemoScene()));
        return;
    }
    
    // X 键 - 重新加载演示页面
    if (input->getButtonDown(Button::X))
    {
        DEBUG_LOG("Reloading demo page...");
        loadDemoUI();
    }
    
    // Y 键 - 加载网络页面（示例）
    if (input->getButtonDown(Button::Y))
    {
        DEBUG_LOG("Loading network page...");
        loadNetworkPage("http://www.example.com");
    }
}
