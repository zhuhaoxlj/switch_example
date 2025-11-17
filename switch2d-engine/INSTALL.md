# Switch2D Engine 安装指南

## ✅ 编译状态

**状态：编译成功！** ✨

所有核心功能已实现并可以正常编译和运行。

## 🔧 已解决的技术问题

### 1. RTTI 禁用问题
**问题**：Makefile 使用 `-fno-rtti` 标志，但代码使用了 `typeid` 和 `dynamic_cast`

**解决方案**：
- 实现了自定义类型ID系统替代 `typeid`
- 使用类型标记（enum）替代 `dynamic_cast`
- 完全避免了RTTI依赖

### 2. SDL2 头文件路径
**问题**：SDL2 头文件找不到

**解决方案**：
- 在 Makefile 中添加 `-I$(PORTLIBS)/include/SDL2`
- 使用正确的头文件包含方式：`<SDL.h>` 而不是 `<SDL2/SDL.h>`

### 3. SDL_image 库缺失
**问题**：系统未安装 SDL2_image

**解决方案**：
- 将 SDL_image 设为可选依赖
- 提供回退到 SDL 内置的 BMP 加载器
- 使用条件编译 `#ifdef HAS_SDL_IMAGE`

### 4. EGL 库依赖
**问题**：SDL2 需要 EGL 和 OpenGL 库

**解决方案**：
- 在链接时添加 `-lEGL -lglapi -ldrm_nouveau`

### 5. 陀螺仪 API
**问题**：HidSixAxisSensorState 结构体成员名称错误

**解决方案**：
- 使用正确的成员：`angular_velocity` 而不是 `gyro`
- 正确初始化 `HidSixAxisSensorHandle`

## 📦 当前功能状态

### ✅ 已实现并可用
- ✅ **核心引擎** - 游戏循环、场景管理
- ✅ **游戏对象系统** - 组件化架构
- ✅ **2D 渲染** - 精灵、摄像机、动画
- ✅ **输入系统** - 手柄、触摸屏、陀螺仪
- ✅ **音频系统** - 音效和背景音乐
- ✅ **物理系统** - 碰撞检测、刚体
- ✅ **资源管理** - 自动缓存

### ⚠️ 限制说明
- **图像格式**：由于 SDL_image 未安装，目前只支持 **BMP** 格式
  - 如需 PNG/JPG 支持，请安装：`sudo dkp-pacman -S switch-sdl2_image`
- **字体渲染**：TextRenderer 组件需要 SDL_ttf 支持（可选功能）

## 🚀 快速开始

### 1. 编译项目
```bash
cd switch2d-engine
./build.sh
```

### 2. 运行测试
生成的文件：`switch2d-demo.nro`

将文件复制到 Switch SD 卡的 `/switch/` 目录，然后通过 Homebrew Launcher 运行。

### 3. Demo 控制
- **方向键/左摇杆** - 移动绿色玩家方块
- **A/B 按钮** - 跳跃
- **+ 按钮** - 退出

## 📝 创建你的第一个游戏

### 最简单的示例
```cpp
#include "Switch2D/Switch2D.h"

using namespace Switch2D;

class MyGame : public Scene {
    void onLoad() override {
        // 创建玩家
        auto player = createGameObject("Player");
        player->transform->position = {640, 360};
        player->transform->scale = {50, 50};
        
        // 添加渲染器（纯色方块）
        auto sprite = player->addComponent<SpriteRenderer>();
        sprite->tint = Color::Blue();
    }
    
    void onUpdate() override {
        // 退出控制
        if (Engine::getInstance().getInput()->getButtonDown(Button::Plus)) {
            Engine::getInstance().quit();
        }
    }
};

int main() {
    Engine& engine = Engine::getInstance();
    
    Engine::Config config;
    config.title = "My Game";
    
    engine.initialize(config);
    engine.loadScene(std::make_unique<MyGame>());
    engine.run();
    engine.shutdown();
    
    return 0;
}
```

### 添加纹理（需要 BMP 格式）
```cpp
// 加载纹理
auto texture = Engine::getInstance().getResources()
    ->loadTexture("romfs:/sprite.bmp");

// 应用到精灵
sprite->setTexture(texture);
```

### 添加物理
```cpp
// 添加重力
auto rb = player->addComponent<Rigidbody>();
rb->useGravity = true;
rb->mass = 1.0f;

// 添加碰撞体
auto collider = player->addComponent<BoxCollider>();
collider->size = {50, 50};
```

## 🔄 可选：安装完整功能

如果需要 PNG/JPG 图像支持：
```bash
# 安装 SDL2_image
sudo dkp-pacman -S switch-sdl2_image

# 重新编译项目
make clean && make
```

如果需要字体渲染支持：
```bash
# 安装 SDL2_ttf
sudo dkp-pacman -S switch-sdl2_ttf
```

## 📚 文档

- **README.md** - 完整功能介绍
- **QUICKSTART.md** - 5分钟快速入门
- **API_REFERENCE.md** - API 详细文档
- **INSTALL.md** - 本文档

## 🎯 下一步

1. **阅读示例代码** - `examples/main.cpp` 包含完整的功能演示
2. **尝试修改** - 改变颜色、大小、速度等参数
3. **创建新场景** - 实现自己的游戏逻辑
4. **添加资源** - 放入 BMP 图像和 OGG 音乐到 `romfs/` 目录

## 🐛 已知问题

无重大问题！引擎核心功能完全可用。

## 💡 提示

1. **使用 BMP 图像**：虽然 PNG/JPG 需要额外安装，但 BMP 格式同样有效
2. **颜色方块足够**：很多 2D 游戏可以只用彩色方块（如 Thomas Was Alone）
3. **性能优化**：避免创建过多游戏对象，使用对象池模式
4. **调试输出**：使用 `printf` 配合 nxlink 进行调试

## ✨ 成就解锁

你现在拥有：
- ✅ 一个功能完整的 2D 游戏引擎
- ✅ 可以立即运行的 Demo
- ✅ 清晰的代码结构和文档
- ✅ 完整的示例代码

**开始创造你的游戏吧！** 🎮
