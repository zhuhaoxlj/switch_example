# Snake Game - 日志输出指南

## 问题说明

在 Nintendo Switch 上（包括 Ryujinx 模拟器），`SDL_Log()` 和 `printf()` 的输出默认不会显示，需要正确配置才能查看日志。

## 已实现的解决方案

代码中已添加以下功能来启用日志输出：

### 1. Socket 初始化
```cpp
socketInitializeDefault();  // 初始化网络socket
nxlinkStdio();              // 重定向 stdout/stderr 到网络
```

### 2. 输出刷新
在关键位置调用 `fflush(stdout)` 确保日志及时输出。

## 在 Ryujinx 模拟器中查看日志

### 方法 1：查看模拟器日志文件

Ryujinx 会将应用输出保存到日志文件中：

**日志文件位置：**
- **Windows**: `%AppData%\Ryujinx\Logs\`
- **macOS**: `~/Library/Application Support/Ryujinx/Logs/`
- **Linux**: `~/.config/Ryujinx/Logs/`

**查看日志：**
```bash
# macOS/Linux
tail -f ~/Library/Application\ Support/Ryujinx/Logs/Ryujinx_*.log

# 或者查找最新的日志文件
ls -lt ~/Library/Application\ Support/Ryujinx/Logs/ | head
```

### 方法 2：启用控制台输出

1. 打开 Ryujinx
2. 进入 **Options** → **Settings** → **Logging**
3. 启用以下选项：
   - **Enable Logging to File**: ✓
   - **Enable Debug Logs**: ✓
   - **Enable Guest Logs**: ✓ (这个最重要！)
   - **Log Level**: 设置为 "Trace" 或 "Debug"

4. 重启 Ryujinx

### 方法 3：使用命令行启动 Ryujinx

从终端启动 Ryujinx 可以直接在终端看到输出：

```bash
# macOS
/Applications/Ryujinx.app/Contents/MacOS/Ryujinx

# Linux
./Ryujinx

# Windows (PowerShell)
& "C:\Program Files\Ryujinx\Ryujinx.exe"
```

## 在真实 Switch 上查看日志

### 使用 nxlink

1. 确保 Switch 和电脑在同一网络
2. 在电脑上运行：
```bash
nxlink -a <switch_ip_address> snake.nro -s
```

3. `-s` 参数会启动服务器模式，接收 Switch 的日志输出

### 预期日志输出

运行游戏后，你应该能看到类似的日志：

```
========================================
  Snake Game - Debug Build
  Logging enabled via nxlink/socket
========================================

===========================================
[Main] Snake Game Starting...
[Main] Version: 1.0 with Debug Logging
===========================================
[Game] Game object created
[Game] Starting initialization...
[Game] romfsInit succeeded
[Game] Initializing SDL subsystems...
[Game] SDL initialized successfully
[Game] Creating window (1280x720)...
[Game] Window created successfully
[Game] Creating renderer...
[Game] Renderer created successfully
[Game] Initializing audio mixer...
[Game] Audio mixer MP3 support initialized
[Game] Audio initialized successfully (44100Hz)
[Game] Loading sound effects...
[Game] Eat sound loaded successfully
[Game] Game over sound loaded successfully
[Game] BGM loaded successfully
[Game] BGM playing at 50% volume (looping)
[Game] Opening joysticks...
[Game] Joystick 0 opened successfully
[Game] Initializing game objects...
[Snake] Initialized at position (32, 18) with 3 segments
[Food] Respawned at position (45, 25)
[Game] Initialization complete! Grid: 64x36, Initial speed: 150ms
[Game] Starting main game loop
```

### 游戏运行时的日志：

```
[Input] Joy button pressed: 13
[Snake] Direction changed: RIGHT -> UP
[Game] Food eaten! Score: 10, Snake length: 4
[Game] Speed increased! New delay: 148ms
[Food] Respawned at position (30, 15)
[Game] Performance: 59.8 FPS, Score: 10, Snake length: 4, Speed: 148ms
```

## 日志标签说明

- `[Main]` - 主程序入口相关
- `[Game]` - 游戏系统、初始化、性能统计
- `[Snake]` - 蛇的移动、碰撞、成长
- `[Food]` - 食物生成位置
- `[Input]` - 输入事件（按键、手柄）

## 性能监控

游戏会每 5 秒自动输出一次性能统计：
- 当前 FPS
- 游戏分数
- 蛇的长度
- 移动速度（延迟时间）

## 故障排除

### 如果还是看不到日志：

1. **检查 Ryujinx 版本**：确保使用较新版本（建议 1.1.0+）
2. **检查 Guest Logs 设置**：必须启用 "Enable Guest Logs"
3. **查看日志文件**：即使界面没显示，日志可能已写入文件
4. **编译模式**：确保使用了最新编译的 .nro 文件

### 调试命令：

```bash
# 重新编译
cd /Users/mark/100-Project/52-Switch/switch-examples/snake
make clean
make -j8

# 检查生成的文件
ls -lh snake.nro
```

## 技术细节

### Socket 初始化的作用

在 Nintendo Switch 的 homebrew 环境中：
- 默认情况下 `stdout` 和 `stderr` 不会输出到任何地方
- `socketInitializeDefault()` 初始化网络栈
- `nxlinkStdio()` 将标准输出重定向到网络 socket
- 这允许日志通过网络发送到 nxlink 工具或被模拟器捕获

### 为什么 SDL_Log 需要这些？

`SDL_Log()` 在 Switch 上最终调用的是标准 C 的 `printf()`，而 `printf()` 输出到 `stdout`。如果不初始化 socket，`stdout` 就是一个"黑洞"，所有输出都会丢失。

## 相关文件

- `source/main.cpp` - 包含所有日志代码
- `Makefile` - 编译配置
- `/network/nxlink_stdio/` - nxlink 示例项目
