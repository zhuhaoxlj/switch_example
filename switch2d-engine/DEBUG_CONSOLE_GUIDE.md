# 🎮 调试控制台使用指南

## ✨ 功能介绍

我为你创建了一个**游戏内置的实时调试控制台**，可以在游戏运行时直接显示日志信息，无需查看外部日志文件！

### 主要特点

- ✅ **游戏内显示** - 日志直接显示在游戏画面上
- ✅ **实时更新** - 所有日志立即可见
- ✅ **快捷切换** - 按 A+B 键即可显示/隐藏
- ✅ **自动滚动** - 自动显示最新的 20 条日志
- ✅ **时间戳** - 每条日志都带有时间标记
- ✅ **像素字体** - 使用简单清晰的3x5像素字体
- ✅ **半透明背景** - 不会完全遮挡游戏画面

---

## 🎮 如何使用

### 1. 启动游戏

正常运行 `switch2d-demo.nro`

### 2. 切换控制台

**同时按下 A 和 B 按钮**即可显示/隐藏调试控制台

```
按键组合: A + B = 切换调试控制台
```

### 3. 查看日志

控制台会显示：
- 引擎初始化信息
- 场景加载状态
- 玩家位置和状态（每秒更新）
- 按钮按下事件
- 跳跃事件
- 其他调试信息

---

## 📊 控制台界面

```
┌────────────────────────────────────────────────────┐
│ DEBUG CONSOLE - Press A+B to toggle                │
├────────────────────────────────────────────────────┤
│ [12:34:56] Switch2D Engine initialized!            │
│ [12:34:56] Resolution: 1280x720                    │
│ [12:34:57] Loading Demo Scene...                   │
│ [12:34:57] Demo Scene loaded! Press A+B to toggle  │
│ [12:34:58] Player Y: 650.00, Ground: 650.00...     │
│ [12:34:59] Button pressed! IsGrounded: YES         │
│ [12:34:59] Jump! velocity.y = -450.00              │
│ ...                                                 │
│ (最多显示20条日志)                                   │
└────────────────────────────────────────────────────┘
```

---

## 💻 在代码中使用

### 基本日志

```cpp
#include "Switch2D/DebugConsole.h"

// 简单日志
DEBUG_LOG("Hello Debug Console!");

// 格式化日志（类似 printf）
DEBUG_LOG("Player position: %.2f, %.2f", x, y);
DEBUG_LOG("FPS: %d, Objects: %zu", fps, count);
```

### 控制台控制

```cpp
// 显示控制台
DebugConsole::getInstance().show();

// 隐藏控制台
DebugConsole::getInstance().hide();

// 切换显示/隐藏
DebugConsole::getInstance().toggle();

// 清空所有日志
DebugConsole::getInstance().clear();
```

### 配置选项

```cpp
// 设置最大显示行数（默认20行）
DebugConsole::getInstance().setMaxLines(30);

// 设置字体大小（默认8像素）
DebugConsole::getInstance().setFontSize(10);

// 设置背景透明度（0.0-1.0，默认0.85）
DebugConsole::getInstance().setOpacity(0.9f);
```

---

## 🎯 当前游戏中的日志

### 启动时
- 引擎初始化信息
- 分辨率设置
- 场景加载状态

### 游戏中（每秒更新）
```
Player Y: 650.00, Ground: 650.00, IsGrounded: YES, Vel.Y: 0.00
```

### 按下按钮时
```
Button pressed! IsGrounded: YES
Jump! velocity.y = -450.00
```
或
```
Button pressed! IsGrounded: NO
Cannot jump - not grounded!
```

### 切换控制台时
```
Debug console toggled
```

---

## 📝 示例代码

### 添加自定义日志

在 `PlayerController` 中：

```cpp
void onUpdate() override {
    InputManager* input = Engine::getInstance().getInput();
    
    // 记录玩家移动
    if (input->getButton(Button::Left)) {
        DEBUG_LOG("Player moving left");
    }
    
    // 记录碰撞
    if (hasCollision) {
        DEBUG_LOG("Collision detected with %s", otherObject->name.c_str());
    }
    
    // 记录状态变化
    if (health != oldHealth) {
        DEBUG_LOG("Health changed: %d -> %d", oldHealth, health);
    }
}
```

### 性能监控

```cpp
void onUpdate() override {
    static int frameCount = 0;
    static float timer = 0;
    
    frameCount++;
    timer += Time::deltaTime;
    
    if (timer >= 1.0f) {
        float fps = frameCount / timer;
        DEBUG_LOG("FPS: %.1f, DeltaTime: %.3f", fps, Time::deltaTime);
        frameCount = 0;
        timer = 0;
    }
}
```

### 条件调试

```cpp
#ifdef DEBUG_BUILD
    DEBUG_LOG("Debug mode active - showing extra info");
    DEBUG_LOG("Memory usage: %zu bytes", getMemoryUsage());
#endif

// 或者运行时控制
if (DebugConsole::getInstance().isVisible()) {
    // 只在控制台可见时计算和显示额外信息
    DEBUG_LOG("Detailed physics info: ...");
}
```

---

## ⚙️ 技术细节

### 像素字体

控制台使用自定义的 3x5 像素字体，支持：
- 数字 0-9
- 字母 A-Z（大小写）
- 空格和标点符号

### 渲染层级

调试控制台在所有游戏内容之上渲染，确保始终可见。

### 性能影响

- 日志写入：极低（仅字符串操作）
- 渲染：低（简单的 SDL 矩形绘制）
- 内存：约 10-20KB（存储最多 40 行日志）

### 自动管理

- 自动限制日志数量（避免内存溢出）
- 自动显示时间戳
- 自动换行处理
- 自动滚动到最新日志

---

## 🎨 自定义样式

想要修改控制台外观？编辑 `src/DebugConsole.cpp`:

```cpp
// 背景颜色（RGBA）
SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(opacity * 255));

// 边框颜色
SDL_Color titleColor = {100, 200, 255, 255}; // 青色

// 文本颜色
SDL_Color textColor = {255, 255, 255, 255}; // 白色

// 警告文本颜色
SDL_Color warnColor = {255, 255, 0, 255}; // 黄色
```

---

## 🐛 常见问题

### Q: 控制台不显示？

**A:** 确保：
1. 同时按下 A 和 B（不是连续按）
2. 检查日志：`DEBUG_LOG("Test message")`
3. 确认控制台已初始化（启动时会有日志）

### Q: 日志显示不完整？

**A:** 
```cpp
// 增加最大行数
DebugConsole::getInstance().setMaxLines(30);
```

### Q: 字体太小？

**A:** 
```cpp
// 增大字体（需要重新实现字体系统）
// 当前字体是固定的3x5像素
```

### Q: 影响性能？

**A:** 
- 日志写入几乎没有性能影响
- 渲染只在控制台可见时进行
- 如果关心性能，隐藏控制台即可

### Q: 如何只在调试版本启用？

**A:** 
```cpp
#ifdef DEBUG
    DEBUG_LOG("This only appears in debug builds");
#endif
```

---

## 🚀 进阶用法

### 日志级别

可以自己实现日志级别：

```cpp
enum LogLevel { INFO, WARN, ERROR, DEBUG };

void log(LogLevel level, const char* format, ...) {
    const char* prefix[] = {"[INFO]", "[WARN]", "[ERROR]", "[DEBUG]"};
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s ", prefix[level]);
    // ... 添加格式化内容
    DEBUG_LOG("%s", buffer);
}
```

### 过滤日志

```cpp
bool enablePhysicsLogs = true;
bool enableRenderLogs = false;

if (enablePhysicsLogs) {
    DEBUG_LOG("Physics update: ...");
}
```

### 命令系统

可以扩展成支持命令的控制台：

```cpp
// 输入命令（未实现，仅示例）
if (command == "clear") {
    DebugConsole::getInstance().clear();
} else if (command == "fps") {
    showFPSCounter = !showFPSCounter;
}
```

---

## 📚 更多示例

查看 `examples/main.cpp` 中的完整实现，包括：
- 玩家控制器中的状态日志
- 场景加载日志
- 按钮事件日志
- 定时日志输出

---

**现在开始使用吧！** 🎮

按 **A + B** 打开控制台，查看游戏的实时运行状态！
