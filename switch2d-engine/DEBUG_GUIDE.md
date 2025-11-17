# 🐛 Switch2D Engine - 调试指南

## 📋 目录

1. [快速开始](#快速开始)
2. [Ryujinx 日志查看](#ryujinx-日志查看)
3. [使用 nxlink](#使用-nxlink)
4. [调试技巧](#调试技巧)
5. [常见问题](#常见问题)

---

## 🚀 快速开始

### 最简单的方法（推荐新手）

1. **启动 Ryujinx**
2. **菜单栏** → **Tools** → **Log Window**
3. **运行游戏** → 查看日志窗口的实时输出

### 使用监控脚本（推荐）

```bash
# 在项目目录下运行
./watch_log.sh
```

然后在 Ryujinx 中启动游戏，日志会自动显示！

---

## 📊 Ryujinx 日志查看

### 方法 1：内置日志窗口

**优点**：简单，无需额外配置  
**缺点**：日志混杂，不易过滤

#### 步骤：
1. 打开 Ryujinx
2. **Tools** → **Log Window**（或 **View** → **Logging**）
3. 运行游戏
4. 在日志窗口查找你的 `printf` 输出

#### 提示：
- 使用 **Ctrl+F** 搜索关键词（如 "Player", "Jump"）
- 可以清空日志：点击 **Clear** 按钮

---

### 方法 2：查看日志文件

**日志位置**：
```bash
# macOS
~/Library/Application Support/Ryujinx/Logs/

# Linux
~/.config/Ryujinx/Logs/

# Windows
C:\Users\你的用户名\AppData\Roaming\Ryujinx\Logs\
```

#### 实时查看（终端）：
```bash
# macOS/Linux
tail -f ~/Library/Application\ Support/Ryujinx/Logs/*.log

# 过滤游戏相关输出
tail -f ~/Library/Application\ Support/Ryujinx/Logs/*.log | grep -E "Player|Jump|Button"
```

---

### 方法 3：使用监控脚本

我已经为你创建了 `watch_log.sh`：

```bash
# 赋予执行权限（首次）
chmod +x watch_log.sh

# 运行监控
./watch_log.sh
```

**特点**：
- ✅ 自动找到最新日志
- ✅ 只显示游戏相关输出
- ✅ 彩色高亮关键词
- ✅ 实时刷新

---

## 🔗 使用 nxlink（高级）

`nxlink` 是专业的 Switch 调试工具，可以通过网络实时传输日志。

### 安装 nxlink

```bash
# DevkitPro 已包含
# 如果没有，手动安装：
cd /tmp
git clone https://github.com/devkitPro/tools
cd tools/nxlink
make
sudo cp nxlink /usr/local/bin/
```

### 配置你的项目

在 `main.cpp` 中添加：

```cpp
#ifdef __SWITCH__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

void nxlinkInit() {
    socketInitializeDefault();
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(28771);  // nxlink 默认端口
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.100");  // 你的电脑 IP
    
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0) {
        dup2(sock, STDOUT_FILENO);
        dup2(sock, STDERR_FILENO);
        printf("nxlink connected!\n");
    }
}
#endif

int main() {
#ifdef __SWITCH__
    nxlinkInit();
#endif
    
    // 你的代码...
}
```

### 使用 nxlink

```bash
# 终端 1：启动 nxlink 服务器
nxlink -s -a 0.0.0.0

# 终端 2：运行 Ryujinx 并加载游戏
# 所有输出会显示在终端 1
```

---

## 🛠️ 调试技巧

### 1. 添加调试宏

在 `include/Switch2D/Core.h` 添加：

```cpp
// 调试宏
#ifdef DEBUG
    #define DEBUG_LOG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG_LOG(fmt, ...)
#endif

#define LOG_INFO(fmt, ...) printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) printf("[WARN] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
```

使用：
```cpp
LOG_INFO("Player position: %.2f, %.2f", x, y);
DEBUG_LOG("Button A pressed!");
LOG_ERROR("Failed to load texture!");
```

---

### 2. 性能监控

添加 FPS 显示：

```cpp
void Scene::onUpdate() {
    static float fpsTimer = 0;
    static int frameCount = 0;
    
    frameCount++;
    fpsTimer += Time::deltaTime;
    
    if (fpsTimer >= 1.0f) {
        printf("FPS: %d\n", frameCount);
        frameCount = 0;
        fpsTimer = 0;
    }
}
```

---

### 3. 变量监控

创建调试面板：

```cpp
void printDebugInfo() {
    printf("========== DEBUG INFO ==========\n");
    printf("Player Pos: (%.2f, %.2f)\n", player->transform->position.x, 
                                          player->transform->position.y);
    printf("Velocity: (%.2f, %.2f)\n", player->rb->velocity.x, 
                                        player->rb->velocity.y);
    printf("IsGrounded: %s\n", isGrounded ? "YES" : "NO");
    printf("================================\n");
}

// 每秒调用一次
static float timer = 0;
timer += Time::deltaTime;
if (timer >= 1.0f) {
    printDebugInfo();
    timer = 0;
}
```

---

### 4. 条件断点（printf 版）

```cpp
if (player->transform->position.y > 1000) {
    printf("WARNING: Player fell too far! Y = %.2f\n", 
           player->transform->position.y);
}

if (rigidbody->velocity.y < -1000) {
    printf("ERROR: Velocity too high! %.2f\n", rigidbody->velocity.y);
}
```

---

## 🎯 当前游戏的调试输出

你的游戏已经包含以下调试信息：

### 每秒输出：
```
Player Y: 650.00, Ground: 650.00, IsGrounded: YES, Velocity.Y: 0.00
```

### 按下 A/B 按钮时：
```
Button pressed! IsGrounded: YES
Jump! velocity.y = -450.00
```

或：
```
Button pressed! IsGrounded: NO
Cannot jump - not grounded!
```

---

## ❓ 常见问题

### Q1: 看不到任何日志输出？

**解决方案**：
1. 确认 Ryujinx 日志窗口已打开
2. 检查日志级别：**Settings** → **Logging** → 设为 **Info** 或 **Debug**
3. 确认程序正在运行（不是卡住或崩溃）

### Q2: 日志太多，看不清？

**解决方案**：
```bash
# 使用 grep 过滤
tail -f log.txt | grep "你关心的关键词"

# 或使用提供的 watch_log.sh 脚本
./watch_log.sh
```

### Q3: printf 不显示？

**可能原因**：
1. 输出缓冲未刷新
2. 格式字符串错误

**解决方案**：
```cpp
// 强制刷新输出
printf("Debug info\n");
fflush(stdout);

// 或使用
fprintf(stderr, "Debug info\n");  // stderr 通常无缓冲
```

### Q4: 想要更详细的系统日志？

在 Ryujinx 中：
1. **Options** → **Settings** → **Logging**
2. **Log Level** 改为 **Trace**（最详细）
3. 启用 **Enable Debug Logs**

**警告**：Trace 级别会产生大量日志！

---

## 📚 更多资源

### 官方文档
- [Ryujinx GitHub](https://github.com/Ryujinx/Ryujinx)
- [DevkitPro Wiki](https://devkitpro.org/wiki/)
- [libnx Documentation](https://switchbrew.org/wiki/Homebrew_Development)

### 调试工具
- **nxlink** - 网络日志传输
- **Ryujinx** - Switch 模拟器
- **nxdumptool** - 文件系统访问

---

## 💡 提示

1. **编译时添加调试信息**：
   ```makefile
   CFLAGS += -g -DDEBUG
   ```

2. **使用颜色区分日志级别**：
   ```cpp
   #define LOG_ERROR(fmt, ...) \
       printf("\033[31m[ERROR]\033[0m " fmt "\n", ##__VA_ARGS__)
   ```

3. **记录时间戳**：
   ```cpp
   printf("[%.2f] Event happened\n", Time::time);
   ```

---

**开始调试吧！** 🚀

如果遇到问题，查看日志输出，它会告诉你发生了什么！
