/*
 * Litehtml 集成示例
 * 
 * 这个文件演示如何使用 litehtml 渲染 HTML 内容
 * 
 * 使用方法：
 * 1. 包含此文件到你的场景中
 * 2. 创建 litehtml_container_sdl 实例
 * 3. 创建 litehtml::context 和 litehtml::document
 * 4. 调用 document->render() 和 document->draw()
 */

#include "litehtml_container_sdl.h"
#include "litehtml/master_css.h"
#include <stdio.h>

// ========================================
// Litehtml 示例使用函数
// ========================================

class LitehtmlBrowser
{
public:
    LitehtmlBrowser(SDL_Renderer* renderer, const char* font_path)
        : m_container(renderer)
    {
        // 设置字体路径
        m_container.set_font_path(font_path);
        
        printf("[Litehtml] Browser initialized\n");
    }
    
    // 从字符串加载 HTML
    bool load_html(const char* html)
    {
        if (!html)
            return false;
        
        // 创建文档（新版 API：直接传入 master CSS 字符串）
        litehtml::estring html_str(html, litehtml::encoding::utf_8);
        
        m_document = litehtml::document::createFromString(
            html_str, 
            &m_container,
            litehtml::master_css  // master CSS 作为第三个参数
        );
        
        if (!m_document)
        {
            printf("[Litehtml] Failed to create document\n");
            return false;
        }
        
        printf("[Litehtml] Document loaded successfully\n");
        return true;
    }
    
    // 渲染文档（布局）
    void render(int max_width)
    {
        if (!m_document)
            return;
        
        m_best_width = m_document->render(max_width);
        m_doc_width = m_document->width();
        m_doc_height = m_document->height();
        
        printf("[Litehtml] Document rendered: %dx%d (best width: %d)\n",
               m_doc_width, m_doc_height, m_best_width);
    }
    
    // 绘制文档
    void draw(int x, int y, const litehtml::position* clip = nullptr)
    {
        if (!m_document)
            return;
        
        // hdc 参数在 SDL 实现中未使用，可以传 0
        m_document->draw((litehtml::uint_ptr)0, x, y, clip);
    }
    
    // 获取文档尺寸
    int get_width() const { return m_doc_width; }
    int get_height() const { return m_doc_height; }
    int get_best_width() const { return m_best_width; }
    
    // 处理鼠标事件
    bool on_mouse_move(int x, int y)
    {
        if (!m_document)
            return false;
        
        litehtml::position::vector redraw_boxes;
        return m_document->on_mouse_over(x, y, x, y, redraw_boxes);
    }
    
    bool on_mouse_click(int x, int y)
    {
        if (!m_document)
            return false;
        
        litehtml::position::vector redraw_boxes;
        return m_document->on_lbutton_down(x, y, x, y, redraw_boxes);
    }
    
private:
    litehtml_container_sdl m_container;
    litehtml::document::ptr m_document;
    int m_doc_width = 0;
    int m_doc_height = 0;
    int m_best_width = 0;
};

// ========================================
// 使用示例
// ========================================

/*
// 在你的场景中使用 litehtml：

void MyScene::onLoad()
{
    // 创建浏览器实例
    SDL_Renderer* renderer = Engine::getInstance().getRenderer();
    browser = new LitehtmlBrowser(renderer, "romfs:/fonts/LXGWWenKai-Regular.ttf");
    
    // 加载 HTML 内容
    const char* html = R"(<!DOCTYPE html>
<html>
<head>
<style>
body { font-family: sans-serif; margin: 20px; background: #1a1a2e; color: white; }
h1 { color: #64b5f6; margin-bottom: 10px; }
p { line-height: 1.6; margin: 10px 0; }
a { color: #81c784; text-decoration: underline; }
ul { margin: 10px 0; padding-left: 20px; }
li { margin: 5px 0; }
</style>
</head>
<body>
<h1>Welcome to Litehtml!</h1>
<p>This is a <b>real HTML page</b> rendered with the litehtml engine.</p>
<p>Features:</p>
<ul>
<li>Full HTML5 support</li>
<li>CSS3 styling</li>
<li>Text formatting</li>
<li>Links and images</li>
</ul>
<p><a href="https://github.com/litehtml/litehtml">Learn more</a></p>
</body>
</html>
)";
    
    browser->load_html(html);
    browser->render(1200); // 渲染宽度1200px
}

void MyScene::onUpdate()
{
    // 处理输入
    InputManager* input = Engine::getInstance().getInput();
    
    if (input->isTouching())
    {
        Vector2 touchPos = input->getTouchPosition();
        browser->on_mouse_move(touchPos.x, touchPos.y);
        
        if (input->getTouchDown())
        {
            browser->on_mouse_click(touchPos.x, touchPos.y);
        }
    }
}

void MyScene::onRender()
{
    // 绘制 HTML
    litehtml::position clip(0, 0, 1280, 720);
    browser->draw(0, 0, &clip);
}

*/
