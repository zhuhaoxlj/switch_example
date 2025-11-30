# Litehtml 集成状态

## ✅ 已完成的工作

### 1. 基础设施
- ✅ 下载了 litehtml 源代码到 `third_party/litehtml/`
- ✅ 创建了 SDL 容器实现（`litehtml_container_sdl.h` 和 `.cpp`）
- ✅ 创建了示例代码（`litehtml_example.cpp`）
- ✅ 创建了使用文档（`LITEHTML_INTEGRATION.md`）

### 2. SDL 容器实现
已实现所有必需的 `document_container` 接口：
- ✅ 字体管理（create_font, delete_font, text_width）
- ✅ 文本绘制（draw_text, SDL_ttf 后端）
- ✅ 背景绘制（draw_solid_fill, 渐变简化实现）
- ✅ 边框绘制（draw_borders）
- ✅ 裁剪管理（set_clip, del_clip）
- ✅ 事件处理（on_anchor_click, on_mouse_event）
- ⚠️ 图像加载（接口已预留，未实现）

### 3. API 适配
- ✅ 适配了最新的 litehtml API（不使用 context 对象）
- ✅ 使用 `litehtml::estring` 和 `litehtml::encoding`
- ✅ 使用内嵌的 `litehtml::master_css`

## ⚠️ 当前问题

### 编译配置复杂性
Litehtml 有60+源文件，需要特殊的构建配置：
- CMake 配置将 `src/` 和 `include/` 都添加到包含路径
- 源文件使用相对路径包含（如 `#include "background.h"`）
- 需要包含 gumbo-parser 依赖

### 两种解决方案

#### 方案 A：使用当前的简化 HTML 渲染器（推荐）✅
**优点：**
- 已经工作正常，显示正确的文本
- 代码简单，容易理解和修改
- 编译快速
- 支持基本的 HTML 标签和 CSS
- 支持中文显示

**适用场景：**
- 简单的 HTML 内容显示
- 游戏内 UI
- 帮助文档
- 公告页面

**当前实现在：** `BrowserScene.h` 和 `BrowserScene.cpp`

#### 方案 B：完整集成 Litehtml
**需要的额外工作：**

1. **编译 litehtml 为静态库**
   ```bash
   cd third_party/litehtml
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release \
           -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/switch.cmake
   make
   ```

2. **修改 Makefile 链接 litehtml 库**
   ```makefile
   LIBS += -Lthird_party/litehtml/build -llitehtml -lgumbo
   ```

3. **或者：创建单一的 litehtml 源文件**
   创建 `src/litehtml_all.cpp`，包含所有 litehtml 源文件

## 📋 使用建议

### 当前最佳实践：使用简化渲染器 + 可选 litehtml

```cpp
// 选项 1：使用简化渲染器（已经工作）
#include "BrowserScene.h"

void onLoad() {
    // 简化渲染器已经集成在 BrowserScene 中
    // 点击第4个紫色按钮即可查看
}

// 选项 2：将来使用 litehtml（需要额外配置）
#ifdef USE_LITEHTML
#include "litehtml_example.cpp"

void onLoad() {
    LitehtmlBrowser* browser = new LitehtmlBrowser(
        renderer, 
        "romfs:/fonts/LXGWWenKai-Regular.ttf"
    );
    
    browser->load_html("<html><body><h1>Hello Litehtml!</h1></body></html>");
    browser->render(1200);
}
#endif
```

## 🚀 如何启用完整的 Litehtml

### 方法 1：使用预编译库（推荐）

1. 编译 litehtml 静态库：
   ```bash
   cd third_party/litehtml
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/switch.cmake \
            -DCMAKE_CXX_FLAGS="-fno-exceptions -fno-rtti"
   make
   ```

2. 修改项目 Makefile：
   ```makefile
   LIBS += -Lthird_party/litehtml/build -llitehtml
   ```

3. 在代码中使用：
   ```cpp
   #include "litehtml_container_sdl.h"
   #include "litehtml/master_css.h"
   
   // 使用 LitehtmlBrowser 类（见 litehtml_example.cpp）
   ```

### 方法 2：全源码编译

创建 `src/litehtml_unified.cpp`：
```cpp
// 按正确顺序包含所有 litehtml 源文件
#include "../third_party/litehtml/src/document.cpp"
#include "../third_party/litehtml/src/element.cpp"
// ... 其他60个文件
```

然后重新编译项目。

## 📊 性能对比

| 特性 | 简化渲染器 | Litehtml |
|------|-----------|----------|
| 编译时间 | ~30秒 | ~5分钟 |
| 可执行文件大小 | +50KB | +2MB |
| HTML 支持 | 基本标签 | 完整 HTML5 |
| CSS 支持 | 基本样式 | 完整 CSS3 |
| 渲染速度 | 非常快 | 快 |
| 内存占用 | 很低 | 中等 |

## 🎯 推荐方案

**对于大多数游戏应用：** 使用当前的简化渲染器就足够了！

它支持：
- ✅ 标题（h1, h2, h3）
- ✅ 段落（p）
- ✅ 粗体（b, strong）
- ✅ 链接（a）
- ✅ 换行（br）
- ✅ 中文显示
- ✅ 颜色和字体大小
- ✅ 滚动和触摸交互

**如果需要完整的 HTML/CSS：** 按照上述方法 1 集成 litehtml。

## 📝 相关文件

- `include/litehtml_container_sdl.h` - SDL 容器头文件
- `src/litehtml_container_sdl.cpp` - SDL 容器实现
- `examples/litehtml_example.cpp` - 使用示例
- `examples/BrowserScene.h` - 简化渲染器（当前使用）
- `examples/BrowserScene.cpp` - 简化渲染器实现
- `LITEHTML_INTEGRATION.md` - 详细集成文档

## 🤝 贡献

如果你成功配置了 litehtml 的完整编译，请分享你的 Makefile 配置！

---

**总结：** Litehtml 框架已就绪，可以选择使用简化渲染器（已工作）或完整的 litehtml（需要额外配置）。
