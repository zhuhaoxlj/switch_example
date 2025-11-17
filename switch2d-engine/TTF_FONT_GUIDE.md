# 🔤 TTF 字体使用指南

## ✨ 功能说明

调试控制台现在支持使用 TrueType 字体（TTF）！你可以使用任何 TTF 字体文件，比如 JetBrains Mono、Consolas、Fira Code 等。

---

## 📁 字体文件准备

### 方法 1：下载 JetBrains Mono（推荐）

```bash
# 1. 创建字体目录
mkdir -p romfs/fonts

# 2. 下载 JetBrains Mono
cd romfs/fonts
curl -L https://github.com/JetBrains/JetBrainsMono/releases/download/v2.304/JetBrainsMono-2.304.zip -o JetBrainsMono.zip

# 3. 解压
unzip JetBrainsMono.zip

# 4. 复制需要的字体文件
cp fonts/ttf/JetBrainsMono-Regular.ttf ./
```

### 方法 2：使用系统字体

```bash
# macOS
cp /System/Library/Fonts/Monaco.ttf romfs/fonts/

# 或者复制任何你喜欢的 TTF 字体到 romfs/fonts/
```

### 方法 3：使用其他等宽字体

推荐的编程字体：
- **JetBrains Mono** - https://www.jetbrains.com/lp/mono/
- **Fira Code** - https://github.com/tonsky/FiraCode
- **Source Code Pro** - https://adobe-fonts.github.io/source-code-pro/
- **Cascadia Code** - https://github.com/microsoft/cascadia-code
- **Consolas** - Windows 自带
- **Monaco** - macOS 自带

---

## 🗂️ 项目结构

```
switch2d-engine/
├── romfs/                    ← 资源目录（新建）
│   └── fonts/               ← 字体目录
│       └── JetBrainsMono-Regular.ttf
├── examples/
│   └── main.cpp
├── src/
│   └── Core.cpp
└── Makefile
```

---

## 💻 在代码中使用

### 方法 A：在初始化时指定字体（推荐）

修改 `src/Core.cpp`：

```cpp
void Engine::initialize(const Config& cfg) {
    // ... 其他初始化代码 ...
    
    // 初始化调试控制台并加载字体
    DebugConsole::getInstance().initialize(renderer, "romfs:/fonts/JetBrainsMono-Regular.ttf");
    
    // 或者设置字体大小
    DebugConsole::getInstance().loadFont("romfs:/fonts/JetBrainsMono-Regular.ttf", 16);
}
```

### 方法 B：运行时动态加载字体

在游戏代码中：

```cpp
// 加载字体
DebugConsole::getInstance().loadFont("romfs:/fonts/JetBrainsMono-Regular.ttf", 14);

// 更改字体大小
DebugConsole::getInstance().setFontSize(16);

// 切换到不同字体
DebugConsole::getInstance().loadFont("romfs:/fonts/Monaco.ttf", 12);
```

---

## ⚙️ 配置字体大小

```cpp
// 小字体（适合显示更多日志）
DebugConsole::getInstance().setFontSize(12);
DebugConsole::getInstance().setMaxLines(25);

// 中等字体（默认）
DebugConsole::getInstance().setFontSize(14);
DebugConsole::getInstance().setMaxLines(20);

// 大字体（适合远距离查看）
DebugConsole::getInstance().setFontSize(18);
DebugConsole::getInstance().setMaxLines(15);
```

---

## 🛠️ Makefile 配置

确保你的 `Makefile` 包含 romfs 目录：

```makefile
#---------------------------------------------------------------------------------
# romfs
#---------------------------------------------------------------------------------
export APP_ROMFS	:=	$(CURDIR)/romfs
```

如果没有这一行，添加到 Makefile 的顶部变量定义部分。

---

## 🔧 构建步骤

### 1. 准备字体文件

```bash
# 创建目录
mkdir -p romfs/fonts

# 下载或复制字体文件
cp /path/to/JetBrainsMono-Regular.ttf romfs/fonts/
```

### 2. 修改初始化代码

编辑 `src/Core.cpp`，在 `initialize()` 函数中：

```cpp
// 初始化调试控制台（带字体）
DebugConsole::getInstance().initialize(
    renderer, 
    "romfs:/fonts/JetBrainsMono-Regular.ttf"
);
```

### 3. 编译

```bash
./build.sh
```

### 4. 运行

在 Ryujinx 中加载 `switch2d-demo.nro`，按 A+B 查看使用新字体的调试控制台！

---

## 📊 字体对比

### 像素字体（默认回退）
- ✅ 体积小（无需字体文件）
- ✅ 总是可用
- ❌ 分辨率低
- ❌ 仅支持英文字母和数字

### TTF 字体
- ✅ 高清晰度
- ✅ 支持所有字符（中文、emoji 等）
- ✅ 可调整大小
- ✅ 更美观专业
- ❌ 需要字体文件（增加 ~200KB）

---

## 🎨 推荐字体设置

### JetBrains Mono（推荐）
```cpp
DebugConsole::getInstance().loadFont("romfs:/fonts/JetBrainsMono-Regular.ttf", 14);
DebugConsole::getInstance().setMaxLines(20);
```

**特点**：
- 专为编程设计
- 字符清晰易读
- 等宽字体
- 支持连字

### Fira Code
```cpp
DebugConsole::getInstance().loadFont("romfs:/fonts/FiraCode-Regular.ttf", 14);
```

**特点**：
- 支持编程连字（->、=>、!= 等）
- 现代设计

### Monaco（macOS 系统字体）
```cpp
DebugConsole::getInstance().loadFont("romfs:/fonts/Monaco.ttf", 12);
```

**特点**：
- 清晰紧凑
- macOS 经典字体

---

## 🐛 常见问题

### Q: 字体加载失败？

**A:** 检查：
1. 字体文件路径是否正确（`romfs:/fonts/xxx.ttf`）
2. 字体文件是否存在于 `romfs/fonts/` 目录
3. Makefile 是否配置了 `APP_ROMFS`
4. 查看启动日志：
   ```
   Loaded TTF font: romfs:/fonts/JetBrainsMono-Regular.ttf
   ```
   或
   ```
   Failed to load font: ...
   Using fallback pixel font
   ```

### Q: 显示乱码？

**A:** 
- 确保字体文件支持你要显示的字符
- 使用 UTF-8 编码
- JetBrains Mono 支持中文的版本：Sarasa Gothic（更纱黑体）

### Q: 字体文件太大？

**A:** 
- 使用 font subset 工具压缩字体
- 只保留需要的字符
- 或使用更小的字体文件（Monaco ~170KB）

### Q: 性能影响？

**A:** 
- TTF 渲染比像素字体略慢
- 但仍然非常快（每帧 <1ms）
- 如果关心性能，使用较小的字号

### Q: 如何回退到像素字体？

**A:** 
- 不传字体路径给 `initialize()`
- 或者字体加载失败会自动回退
```cpp
// 不使用 TTF 字体
DebugConsole::getInstance().initialize(renderer);  // 不传字体路径
```

---

## 📝 完整示例

### 修改 Core.cpp

```cpp
// 在 Engine::initialize() 中
void Engine::initialize(const Config& cfg) {
    // ... SDL 初始化等 ...
    
    resourceManager->initialize(renderer);
    
    // 初始化调试控制台（TTF 字体）
    const char* fontPath = "romfs:/fonts/JetBrainsMono-Regular.ttf";
    DebugConsole::getInstance().initialize(renderer, fontPath);
    
    // 可选：配置控制台
    DebugConsole::getInstance().setFontSize(14);
    DebugConsole::getInstance().setMaxLines(20);
    DebugConsole::getInstance().setOpacity(0.9f);
    
    DebugConsole::getInstance().log("Switch2D Engine initialized with TTF font!");
    DebugConsole::getInstance().logf("Font: %s, Size: %d", fontPath, 14);
    
    // ... 其他代码 ...
}
```

### 准备字体文件

```bash
# 快速设置
mkdir -p romfs/fonts
cd romfs/fonts

# 选项 1：下载 JetBrains Mono
curl -L "https://github.com/JetBrains/JetBrainsMono/releases/download/v2.304/JetBrainsMono-2.304.zip" -o jb.zip
unzip jb.zip
cp fonts/ttf/JetBrainsMono-Regular.ttf ./

# 选项 2：复制系统字体（macOS）
cp /System/Library/Fonts/Monaco.ttf ./

# 清理
rm -rf fonts/ variable/ jb.zip
cd ../..
```

### 编译和运行

```bash
# 编译
./build.sh

# 在 Ryujinx 中运行
# 按 A+B 查看使用新字体的调试控制台！
```

---

## 🎉 效果预览

使用 TTF 字体后，你的调试控制台会显示：

```
┌────────────────────────────────────────────────────────────┐
│ DEBUG CONSOLE - Press A+B to toggle                        │
├────────────────────────────────────────────────────────────┤
│ [12:34:56] Switch2D Engine initialized with TTF font!     │
│ [12:34:56] Font: romfs:/fonts/JetBrainsMono-Regular.ttf   │
│ [12:34:57] Loading Demo Scene...                          │
│ [12:34:58] Player Y: 650.00, Ground: 650.00, IsGr...      │
│ ...                                                         │
└────────────────────────────────────────────────────────────┘
```

文字更清晰、更专业、更易读！✨

---

## 📚 资源链接

- **JetBrains Mono**: https://www.jetbrains.com/lp/mono/
- **Fira Code**: https://github.com/tonsky/FiraCode
- **SDL_ttf 文档**: https://www.libsdl.org/projects/SDL_ttf/
- **字体 Subset 工具**: https://github.com/fonttools/fonttools

---

**开始使用 TTF 字体，让你的调试控制台更专业！** 🚀
