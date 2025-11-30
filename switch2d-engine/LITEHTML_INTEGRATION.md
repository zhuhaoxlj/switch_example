# Litehtml 集成指南

## 概述

Litehtml 是一个轻量级的 HTML/CSS 渲染引擎，已成功集成到本项目中。

## 📁 项目结构

```
switch2d-engine/
├── third_party/litehtml/          # Litehtml 源代码
├── include/litehtml_container_sdl.h  # SDL 容器实现（头文件）
├── src/litehtml_container_sdl.cpp    # SDL 容器实现
└── examples/litehtml_example.cpp     # 使用示例
```

## ✨ 特性

- ✅ 完整的 HTML5/CSS3 支持
- ✅ SDL2 渲染后端
- ✅ TTF 字体支持（包括中文）
- ✅ 触摸屏交互
- ✅ Nintendo Switch 优化

## 🚀 快速开始

### 1. 基础使用

```cpp
#include "litehtml_container_sdl.h"
#include "litehtml/master_css.h"

// 创建容器
SDL_Renderer* renderer = /* 你的渲染器 */;
litehtml_container_sdl container(renderer);
container.set_font_path("romfs:/fonts/LXGWWenKai-Regular.ttf");

// 创建 context 并加载 master CSS
auto context = std::make_shared<litehtml::context>();
context->load_master_stylesheet(litehtml::master_css);

// 加载 HTML
const char* html = "<h1>Hello World!</h1><p>This is <b>HTML</b>!</p>";
auto document = litehtml::document::createFromString(html, &container, context.get());

// 渲染和绘制
document->render(1200);  // 最大宽度
document->draw((litehtml::uint_ptr)0, 0, 0, nullptr);  // 绘制到屏幕
```

### 2. 使用 LitehtmlBrowser 包装类

更简单的方式是使用 `examples/litehtml_example.cpp` 中提供的 `LitehtmlBrowser` 类：

```cpp
// 创建浏览器
LitehtmlBrowser* browser = new LitehtmlBrowser(renderer, "romfs:/fonts/LXGWWenKai-Regular.ttf");

// 加载 HTML
browser->load_html("<html><body><h1>Hello!</h1></body></html>");

// 渲染
browser->render(1200);

// 绘制
browser->draw(0, 0);
```

## 📝 完整示例

查看 `examples/litehtml_example.cpp` 获取完整的使用示例，包括：
- HTML 加载
- 布局和渲染
- 事件处理
- 滚动和裁剪

## 🔧 API 参考

### LitehtmlBrowser 类

#### 方法

- `load_html(const char* html)` - 从字符串加载 HTML
- `render(int max_width)` - 渲染文档（布局计算）
- `draw(int x, int y, const litehtml::position* clip)` - 绘制文档
- `get_width()` - 获取文档宽度
- `get_height()` - 获取文档高度
- `on_mouse_move(int x, int y)` - 处理鼠标移动
- `on_mouse_click(int x, int y)` - 处理鼠标点击

## 🎨 CSS 支持

Litehtml 支持大部分 CSS2/CSS3 特性：

- 文本格式化（字体、颜色、对齐）
- 盒模型（margin、padding、border）
- 浮动和定位
- 表格布局
- Flexbox
- CSS 选择器
- @media 查询
- 渐变背景

## 🖼️ 图像支持

当前实现：
- ⚠️ 图像加载功能已预留但未完全实现
- 图像会显示为灰色占位矩形
- 要添加图像支持，需要实现 `load_image()` 和 `draw_image()` 方法

## 🌐 网络加载

要从网络加载 HTML：

```cpp
// 使用 Switch2D 的 NetworkManager
NetworkManager* net = Engine::getInstance().getNetwork();
HTTPResponse resp = net->get("http://example.com");

if (resp.isSuccess())
{
    std::string html = resp.getBodyAsString();
    browser->load_html(html.c_str());
    browser->render(1200);
}
```

## ⚙️ 性能优化

### 编译优化

当前配置已经优化了编译：
- `-O2` 优化级别
- `-ffunction-sections` 函数节
- `-DLITEHTML_NO_THREADS` 禁用多线程（Switch 不需要）

### 运行时优化

1. **缓存渲染结果**：如果内容不变，不要重复调用 `render()`
2. **使用裁剪区域**：只绘制可见部分
3. **限制文档复杂度**：避免过于复杂的 HTML/CSS

## 🔍 调试

启用调试输出：

```cpp
#define LITEHTML_DEBUG
#include "litehtml_container_sdl.h"
```

查看日志：
- 使用 Switch2D 的 DEBUG_LOG 宏
- 按 A+B 切换调试控制台

## 🐛 已知限制

1. **图像**：需要实现图像加载器
2. **渐变**：目前使用纯色替代
3. **字体**：只支持单一字体文件
4. **JavaScript**：不支持（litehtml 本身不支持）

## 📚 更多资源

- [Litehtml 官方文档](https://github.com/litehtml/litehtml/wiki)
- [Litehtml API 参考](https://github.com/litehtml/litehtml/wiki/document_container)
- [CSS 参考](https://developer.mozilla.org/en-US/docs/Web/CSS)

## 🤝 贡献

要改进 litehtml 集成：

1. 编辑 `include/litehtml_container_sdl.h`
2. 编辑 `src/litehtml_container_sdl.cpp`
3. 测试并提交

## 📄 许可证

- Litehtml: BSD License
- gumbo-parser: Apache License 2.0
- 本集成代码: 遵循项目许可证

---

**祝你使用愉快！** 🎮✨
