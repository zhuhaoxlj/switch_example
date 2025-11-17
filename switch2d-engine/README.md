# Switch2D Game Engine

一个为 Nintendo Switch 设计的现代化 2D 游戏引擎，基于 SDL2 构建。

## ⚡ 快速导航

| 文档 | 说明 | 适合 |
|------|------|------|
| **[PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)** | 📊 项目完成总结 | 想了解整体情况 |
| **[QUICKSTART.md](QUICKSTART.md)** | 🚀 5分钟快速入门 | 立即开始使用 |
| **[INSTALL.md](INSTALL.md)** | 🔧 编译和安装指南 | 遇到编译问题 |
| **[EXAMPLES.md](EXAMPLES.md)** | 💡 8个完整代码示例 | 学习如何使用 |
| **[API_REFERENCE.md](API_REFERENCE.md)** | 📚 完整API文档 | 查找具体函数 |
| **[STATUS.md](STATUS.md)** | ✅ 项目状态报告 | 了解功能完成度 |

## ✨ 项目状态

**✅ 编译成功！可立即使用！**

```bash
cd switch2d-engine
./build.sh
# 生成: switch2d-demo.nro
```

## ✨ 特性

### 核心功能
- 🎮 **完整的游戏对象系统** - 灵活的组件化架构
- 🎨 **2D渲染系统** - 精灵渲染、动画、摄像机
- 🎯 **输入管理** - 手柄、触摸屏、陀螺仪支持
- 🔊 **音频系统** - 音效和背景音乐播放
- ⚡ **物理引擎** - 简单的碰撞检测和刚体模拟
- 📦 **资源管理** - 自动缓存和生命周期管理

### 组件系统
- `Transform` - 位置、旋转、缩放
- `SpriteRenderer` - 精灵渲染
- `Animator` - 动画控制
- `Camera` - 摄像机
- `Rigidbody` - 物理刚体
- `BoxCollider` / `CircleCollider` - 碰撞检测
- 支持自定义组件

## 🛠️ 安装和编译

### 前置要求
```bash
# 安装 DevkitPro
# 参考: https://devkitpro.org/wiki/Getting_Started

# 安装必要的库
sudo dkp-pacman -S switch-sdl2 switch-sdl2_image switch-sdl2_mixer
```

### 编译引擎示例
```bash
cd switch2d-engine
make
```

生成的 `.nro` 文件可以在 Switch 的 Homebrew Launcher 中运行。

## 📖 快速开始

### 基础游戏结构

```cpp
#include "Switch2D/Switch2D.h"

using namespace Switch2D;

// 1. 创建场景
class MyGameScene : public Scene {
public:
    void onLoad() override {
        // 创建游戏对象
        GameObject* player = createGameObject("Player");
        player->transform->position = {640, 360};
        
        // 添加组件
        auto sprite = player->addComponent<SpriteRenderer>();
        sprite->tint = Color::Green();
        
        auto controller = player->addComponent<PlayerController>();
    }
    
    void onUpdate() override {
        // 每帧更新逻辑
    }
};

// 2. 主函数
int main() {
    Engine& engine = Engine::getInstance();
    
    Engine::Config config;
    config.title = "My Game";
    
    engine.initialize(config);
    engine.loadScene(std::make_unique<MyGameScene>());
    engine.run();
    engine.shutdown();
    
    return 0;
}
```

### 创建自定义组件

```cpp
class PlayerController : public Component {
public:
    float speed = 200.0f;
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 移动控制
        Vector2 movement{0, 0};
        if (input->getButton(Button::Left)) movement.x = -1;
        if (input->getButton(Button::Right)) movement.x = 1;
        
        transform->position += movement * speed * Time::deltaTime;
    }
};
```

### 加载和使用资源

```cpp
// 加载纹理
auto texture = resources->loadTexture("romfs:/sprites/player.png");

// 应用到精灵
spriteRenderer->setTexture(texture);

// 播放音效
auto sound = resources->loadSound("romfs:/sounds/jump.wav");
audio->playSound(sound);

// 播放音乐
auto music = resources->loadMusic("romfs:/music/bgm.ogg");
audio->playMusic(music, -1); // -1 = 循环播放
```

### 动画系统

```cpp
// 创建动画
Animation walkAnim;
walkAnim.addFrame(Rectangle{0, 0, 32, 32}, 0.1f);
walkAnim.addFrame(Rectangle{32, 0, 32, 32}, 0.1f);
walkAnim.addFrame(Rectangle{64, 0, 32, 32}, 0.1f);
walkAnim.loop = true;

// 添加到动画器
Animator* animator = obj->addComponent<Animator>();
animator->addAnimation("walk", walkAnim);
animator->play("walk");
```

### 物理和碰撞

```cpp
// 添加刚体
Rigidbody* rb = player->addComponent<Rigidbody>();
rb->mass = 1.0f;
rb->useGravity = true;

// 添加碰撞体
BoxCollider* collider = player->addComponent<BoxCollider>();
collider->size = {50, 50};

// 在 Scene 中检测碰撞
void onUpdate() override {
    Physics::checkCollisions(this);
}
```

## 🎮 输入系统

```cpp
InputManager* input = Engine::getInstance().getInput();

// 按钮检测
if (input->getButtonDown(Button::A)) {
    // A 键刚按下
}
if (input->getButton(Button::B)) {
    // B 键保持按下
}

// 摇杆输入
Vector2 leftStick = input->getLeftStick();
Vector2 rightStick = input->getRightStick();

// 触摸屏
if (input->isTouching()) {
    Vector2 touchPos = input->getTouchPosition();
}
```

## 🎨 渲染系统

### 精灵渲染选项
```cpp
SpriteRenderer* sprite = obj->addComponent<SpriteRenderer>();

sprite->tint = Color(255, 100, 100);  // 颜色调制
sprite->flipX = true;                  // 水平翻转
sprite->flipY = false;                 // 垂直翻转
sprite->pivot = {0.5f, 0.5f};         // 锚点 (中心)
sprite->sortingOrder = 10;             // 渲染顺序
```

### 摄像机
```cpp
GameObject* camObj = createGameObject("Camera");
Camera* camera = camObj->addComponent<Camera>();
camera->backgroundColor = Color(100, 149, 237);
camera->zoom = 2.0f;

// 坐标转换
Vector2 screenPos = camera->worldToScreen(worldPos);
Vector2 worldPos = camera->screenToWorld(screenPos);
```

## 📊 性能建议

1. **对象池** - 重用游戏对象而不是频繁创建/销毁
2. **批量渲染** - 使用相同纹理的对象会自动批处理
3. **碰撞优化** - 只在需要时启用碰撞检测
4. **资源管理** - 使用 ResourceManager 自动缓存资源

## 🗂️ 项目结构

```
switch2d-engine/
├── include/Switch2D/    # 头文件
│   ├── Switch2D.h       # 主头文件
│   ├── Core.h           # 引擎核心
│   ├── Math.h           # 数学库
│   ├── GameObject.h     # 游戏对象
│   ├── Scene.h          # 场景管理
│   ├── Renderer.h       # 渲染系统
│   ├── Input.h          # 输入管理
│   ├── Audio.h          # 音频系统
│   ├── Resources.h      # 资源管理
│   └── Physics.h        # 物理系统
├── src/                 # 源文件
├── examples/            # 示例代码
│   └── main.cpp         # Demo 游戏
├── romfs/               # 资源文件
├── Makefile             # 构建文件
└── README.md            # 本文件
```

## 🎯 示例 Demo

运行 `make` 编译的示例包含：
- ✅ 可控制的玩家角色
- ✅ 重力和跳跃物理
- ✅ 旋转的装饰物
- ✅ 随机移动的对象
- ✅ 碰撞检测
- ✅ FPS 显示

### 控制说明
- **方向键/左摇杆** - 移动玩家
- **A/B 按钮** - 跳跃
- **+ 按钮** - 退出

## 🔧 扩展引擎

### 添加新组件类型
```cpp
class MyCustomComponent : public Component {
public:
    void onStart() override {
        // 初始化
    }
    
    void onUpdate() override {
        // 每帧更新
    }
    
    void onRender() override {
        // 自定义渲染
    }
    
    void onDestroy() override {
        // 清理
    }
};
```

### 创建新场景
```cpp
class Level1 : public Scene {
    void onLoad() override {
        // 加载关卡
    }
};

class Level2 : public Scene {
    void onLoad() override {
        // 加载下一关
    }
};

// 切换场景
engine.loadScene(std::make_unique<Level2>());
```

## 📝 待办事项

- [ ] 添加 Tilemap 支持
- [ ] 粒子系统
- [ ] UI 系统 (按钮、文本、面板)
- [ ] 场景过渡效果
- [ ] 存档系统
- [ ] 更高级的物理引擎集成 (Box2D)
- [ ] 音频淡入淡出
- [ ] Shader 支持

## 📄 许可证

本项目基于 Public Domain 发布，可自由使用和修改。

## 🙏 致谢

- DevkitPro 团队
- SDL2 库
- Nintendo Switch 自制软件社区

## 📮 联系方式

如有问题或建议，欢迎提交 Issue 或 Pull Request！

---

**祝你游戏开发愉快！** 🎮✨
