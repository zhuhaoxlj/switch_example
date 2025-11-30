#include "BrowserScene.h"
#include <algorithm>

// ============================================
// LitehtmlBrowserComponent 实现
// ============================================

LitehtmlBrowserComponent::LitehtmlBrowserComponent()
{
    SDL_Renderer* renderer = Engine::getInstance().getRenderer();
    m_container = new litehtml_container_sdl(renderer);
    m_container->set_font_path("romfs:/fonts/LXGWWenKai-Regular.ttf");
    
    DEBUG_LOG("[Litehtml] Browser component initialized");
}

LitehtmlBrowserComponent::~LitehtmlBrowserComponent()
{
    if (m_container)
    {
        delete m_container;
        m_container = nullptr;
    }
}

bool LitehtmlBrowserComponent::loadHTML(const char* html)
{
    if (!html || !m_container)
        return false;
    
    // 创建文档
    litehtml::estring html_str(html, litehtml::encoding::utf_8);
    
    m_document = litehtml::document::createFromString(
        html_str,
        m_container,
        litehtml::master_css
    );
    
    if (!m_document)
    {
        DEBUG_LOG("[Litehtml] Failed to create document");
        return false;
    }
    
    DEBUG_LOG("[Litehtml] Document loaded successfully");
    return true;
}

bool LitehtmlBrowserComponent::loadHTMLFromNetwork(const std::string& url)
{
    DEBUG_LOG("[Litehtml] Loading page: %s", url.c_str());
    
    NetworkManager* net = Engine::getInstance().getNetwork();
    
    if (!net->isInitialized())
    {
        DEBUG_LOG("[Litehtml] ERROR: Network not initialized!");
        return false;
    }
    
    HTTPResponse resp = net->get(url);
    
    if (resp.isSuccess())
    {
        std::string html = resp.getBodyAsString();
        DEBUG_LOG("[Litehtml] Downloaded %zu bytes", html.length());
        return loadHTML(html.c_str());
    }
    else
    {
        DEBUG_LOG("[Litehtml] Failed to load page: %s", resp.error.c_str());
        return false;
    }
}

void LitehtmlBrowserComponent::render(int max_width)
{
    if (!m_document)
        return;
    
    m_best_width = m_document->render(max_width);
    m_doc_width = m_document->width();
    m_doc_height = m_document->height();
    
    // 计算最大滚动偏移
    m_max_scroll_offset = std::max(0, m_doc_height - 720 + 80);
    
    DEBUG_LOG("[Litehtml] Document rendered: %dx%d (best width: %d)", 
              m_doc_width, m_doc_height, m_best_width);
}

void LitehtmlBrowserComponent::draw(int x, int y)
{
    if (!m_document)
        return;
    
    // 应用滚动偏移
    int draw_y = y - (int)m_scroll_offset;
    
    // 设置裁剪区域
    litehtml::position clip(0, 60, 1280, 660);  // 留出顶部地址栏空间
    
    m_document->draw((litehtml::uint_ptr)0, x, draw_y, &clip);
}

void LitehtmlBrowserComponent::handleInput()
{
    InputManager* input = Engine::getInstance().getInput();
    
    // 滚动控制
    if (input->getButton(Button::Up) || input->getLeftStick().y < -0.3f)
    {
        m_scroll_offset -= 300.0f * Time::deltaTime;
        if (m_scroll_offset < 0)
            m_scroll_offset = 0;
    }
    if (input->getButton(Button::Down) || input->getLeftStick().y > 0.3f)
    {
        m_scroll_offset += 300.0f * Time::deltaTime;
        if (m_scroll_offset > m_max_scroll_offset)
            m_scroll_offset = m_max_scroll_offset;
    }
    
    // 触摸交互
    if (m_document && input->isTouching())
    {
        Vector2 touchPos = input->getTouchPosition();
        int adj_y = touchPos.y + (int)m_scroll_offset - 60;  // 调整滚动偏移
        
        litehtml::position::vector redraw_boxes;
        m_document->on_mouse_over(touchPos.x, adj_y, touchPos.x, adj_y, redraw_boxes);
        
        if (input->getTouchDown())
        {
            m_document->on_lbutton_down(touchPos.x, adj_y, touchPos.x, adj_y, redraw_boxes);
        }
    }
}

void LitehtmlBrowserComponent::onUpdate()
{
    handleInput();
}

void LitehtmlBrowserComponent::onRender()
{
    draw(0, 60);  // 从顶部地址栏下方开始绘制
    
    // 绘制滚动条
    if (m_max_scroll_offset > 0)
    {
        SDL_Renderer* renderer = Engine::getInstance().getRenderer();
        
        float scrollBarHeight = 600.0f;
        float scrollBarY = 60 + (m_scroll_offset / m_max_scroll_offset) * (scrollBarHeight - 50);
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 200);
        SDL_Rect scrollBar = {1260, (int)scrollBarY, 10, 50};
        SDL_RenderFillRect(renderer, &scrollBar);
    }
}

// ============================================
// BrowserScene 实现
// ============================================

void BrowserScene::onLoad()
{
    DEBUG_LOG("=== Browser Scene (Litehtml Powered) ===");
    DEBUG_LOG("Loading www.baidu.com with full HTML5/CSS3 support");
    DEBUG_LOG("Press B to return to main menu");
    DEBUG_LOG("Use D-Pad/Stick to scroll, Touch to interact");
    DEBUG_LOG("Press X to reload Baidu, Y to load example.com");
    
    // 创建摄像机
    GameObject* cameraObj = createGameObject("Camera");
    Camera* camera = cameraObj->addComponent<Camera>();
    camera->backgroundColor = Color(20, 20, 30);
    
    // 创建浏览器对象
    browserObject = createGameObject("Browser");
    browser = browserObject->addComponent<LitehtmlBrowserComponent>();
    
    // 加载示例页面
    loadSamplePage();
    
    // 创建地址栏背景
    GameObject* addressBar = createGameObject("Address Bar");
    addressBar->transform->position = {640, 30};
    addressBar->transform->scale = {1280, 60};
    SpriteRenderer* addrRenderer = addressBar->addComponent<SpriteRenderer>();
    addrRenderer->tint = Color(40, 40, 60);
    addrRenderer->sortingOrder = 10;
    
    DEBUG_LOG("Browser Scene loaded successfully!");
    DEBUG_LOG("");
    DEBUG_LOG("=== Powered by Litehtml ===");
    DEBUG_LOG("Full HTML5/CSS3 support");
    DEBUG_LOG("- Complete CSS styling");
    DEBUG_LOG("- Flexbox layout");
    DEBUG_LOG("- CSS selectors");
    DEBUG_LOG("- @media queries");
    DEBUG_LOG("- And much more!");
}

void BrowserScene::loadSamplePage()
{
    // 尝试从网络加载百度首页
    DEBUG_LOG("=== Loading Baidu Homepage via Litehtml ===");
    DEBUG_LOG("URL: http://www.baidu.com");
    
    if (browser && browser->loadHTMLFromNetwork("http://www.baidu.com"))
    {
        browser->render(1200);
        DEBUG_LOG("✓ Baidu homepage loaded successfully with Litehtml!");
        return;
    }
    
    DEBUG_LOG("Network loading failed, showing local demo page");
    
    // 如果网络加载失败，显示本地示例页面
    const char* html = R"(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<style>
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: sans-serif;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    padding: 20px;
    line-height: 1.6;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
}

h1 {
    font-size: 48px;
    margin-bottom: 20px;
    text-align: center;
    text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
    background: linear-gradient(90deg, #ffd89b 0%, #19547b 100%);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
}

h2 {
    font-size: 32px;
    margin: 30px 0 15px 0;
    color: #ffd89b;
    border-bottom: 2px solid #ffd89b;
    padding-bottom: 10px;
}

h3 {
    font-size: 24px;
    margin: 20px 0 10px 0;
    color: #a8dadc;
}

p {
    margin: 10px 0;
    font-size: 18px;
}

.feature-grid {
    display: flex;
    flex-wrap: wrap;
    gap: 20px;
    margin: 20px 0;
}

.feature-card {
    flex: 1 1 calc(50% - 10px);
    background: rgba(255, 255, 255, 0.1);
    padding: 20px;
    border-radius: 10px;
    backdrop-filter: blur(10px);
    border: 1px solid rgba(255, 255, 255, 0.2);
}

.feature-card h3 {
    margin-top: 0;
}

ul {
    list-style-position: inside;
    margin: 15px 0;
}

li {
    margin: 8px 0;
    padding-left: 10px;
}

a {
    color: #81c784;
    text-decoration: underline;
}

a:hover {
    color: #a5d6a7;
}

.highlight {
    background: rgba(255, 215, 0, 0.3);
    padding: 2px 6px;
    border-radius: 3px;
}

.info-box {
    background: rgba(100, 181, 246, 0.2);
    border-left: 4px solid #64b5f6;
    padding: 15px;
    margin: 20px 0;
    border-radius: 5px;
}

.success-box {
    background: rgba(129, 199, 132, 0.2);
    border-left: 4px solid #81c784;
    padding: 15px;
    margin: 20px 0;
    border-radius: 5px;
}

code {
    background: rgba(0, 0, 0, 0.3);
    padding: 2px 6px;
    border-radius: 3px;
    font-family: monospace;
}

.footer {
    text-align: center;
    margin-top: 40px;
    padding-top: 20px;
    border-top: 1px solid rgba(255, 255, 255, 0.2);
    font-size: 14px;
    opacity: 0.8;
}
</style>
</head>
<body>
<div class="container">
    <h1>🎮 Welcome to Litehtml!</h1>
    
    <div class="success-box">
        <p><strong>🎉 Success!</strong> This page is rendered using the <span class="highlight">real litehtml engine</span> with full HTML5/CSS3 support!</p>
    </div>
    
    <h2>✨ Features</h2>
    
    <div class="feature-grid">
        <div class="feature-card">
            <h3>🎨 CSS3 Styling</h3>
            <p>Complete CSS support including gradients, shadows, borders, and more!</p>
        </div>
        
        <div class="feature-card">
            <h3>📐 Flexbox Layout</h3>
            <p>Modern CSS flexbox for responsive layouts (like this grid!)</p>
        </div>
        
        <div class="feature-card">
            <h3>🔤 Typography</h3>
            <p>Rich text formatting with custom fonts and styles</p>
        </div>
        
        <div class="feature-card">
            <h3>🖱️ Interactive</h3>
            <p>Click links, scroll pages, and interact with elements</p>
        </div>
    </div>
    
    <h2>📋 Supported Features</h2>
    
    <div class="info-box">
        <h3>HTML Elements</h3>
        <ul>
            <li>Headings (h1-h6)</li>
            <li>Paragraphs and text formatting</li>
            <li>Lists (ul, ol, li)</li>
            <li>Links (<code>&lt;a&gt;</code> tags)</li>
            <li>Divs and containers</li>
            <li>Tables and forms</li>
        </ul>
    </div>
    
    <div class="info-box">
        <h3>CSS Properties</h3>
        <ul>
            <li>Colors, backgrounds, gradients</li>
            <li>Borders, padding, margins</li>
            <li>Text alignment and decoration</li>
            <li>Flexbox and positioning</li>
            <li>Pseudo-classes (:hover, :active)</li>
            <li>Media queries for responsive design</li>
        </ul>
    </div>
    
    <h2>🚀 Performance</h2>
    <p>Litehtml is designed to be lightweight and fast, perfect for:</p>
    <ul>
        <li><strong>Game UI</strong> - Create beautiful interfaces with HTML/CSS</li>
        <li><strong>Documentation</strong> - Display help pages and tutorials</li>
        <li><strong>Web Content</strong> - Show formatted text and articles</li>
        <li><strong>Embedded Browser</strong> - Mini-browser functionality</li>
    </ul>
    
    <h2>🔗 Learn More</h2>
    <p>Visit the official website: <a href="https://github.com/litehtml/litehtml">github.com/litehtml/litehtml</a></p>
    
    <div class="success-box">
        <p><strong>💡 Pro Tip:</strong> You can load any HTML content dynamically, including pages from the internet!</p>
    </div>
    
    <div class="footer">
        <p>Powered by <strong>Litehtml</strong> • Running on <strong>Nintendo Switch</strong> 🎮</p>
        <p>Press B to return • Use D-Pad to scroll</p>
    </div>
</div>
</body>
</html>
)";
    
    if (browser)
    {
        browser->loadHTML(html);
        browser->render(1200);  // 渲染宽度 1200px
        DEBUG_LOG("[BrowserScene] Sample page loaded and rendered");
    }
}

void BrowserScene::loadNetworkPage(const std::string& url)
{
    if (browser)
    {
        browser->loadHTMLFromNetwork(url);
        browser->render(1200);
    }
}

void BrowserScene::onUpdate()
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
    
    // X 键 - 重新加载百度首页
    if (input->getButtonDown(Button::X))
    {
        DEBUG_LOG("Reloading Baidu homepage with Litehtml...");
        if (browser && browser->loadHTMLFromNetwork("http://www.baidu.com"))
        {
            browser->render(1200);
            DEBUG_LOG("✓ Baidu reloaded successfully!");
        }
        else
        {
            DEBUG_LOG("✗ Failed to reload. Check network connection.");
        }
    }
    
    // Y 键 - 加载 example.com
    if (input->getButtonDown(Button::Y))
    {
        DEBUG_LOG("Loading example.com with Litehtml...");
        if (browser && browser->loadHTMLFromNetwork("http://www.example.com"))
        {
            browser->render(1200);
            DEBUG_LOG("✓ Example.com loaded successfully!");
        }
    }
}
