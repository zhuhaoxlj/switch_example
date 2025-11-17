# Switch2D 引擎快速入门

## 🚀 5分钟上手指南

### 第一步：环境准备

确保已安装 DevkitPro 和必要的库：

```bash
# 设置环境变量（添加到 ~/.bashrc 或 ~/.zshrc）
export DEVKITPRO=/opt/devkitpro
export PATH=$DEVKITPRO/tools/bin:$PATH

# 安装 SDL2 库
sudo dkp-pacman -S switch-sdl2 switch-sdl2_image switch-sdl2_mixer
```

### 第二步：编译引擎示例

```bash
cd switch2d-engine
make
```

成功后会生成 `switch2d-demo.nro` 文件。

### 第三步：在 Switch 上运行

1. 将 `switch2d-demo.nro` 复制到 SD 卡的 `/switch/` 目录
2. 在 Switch 上启动 Homebrew Launcher
3. 找到并运行 Switch2D Demo

## 📝 创建你的第一个游戏

### 1. 项目结构

创建新项目：
```
my-game/
├── include/
├── src/
│   └── main.cpp
├── romfs/
│   ├── sprites/
│   ├── sounds/
│   └── music/
├── Makefile
└── icon.jpg
```

### 2. 最小化代码示例

```cpp
#include "Switch2D/Switch2D.h"

using namespace Switch2D;

class MyScene : public Scene {
public:
    void onLoad() override {
        // 创建玩家
        GameObject* player = createGameObject("Player");
        player->transform->position = {640, 360};
        player->transform->scale = {64, 64};
        
        // 添加渲染器
        auto renderer = player->addComponent<SpriteRenderer>();
        renderer->tint = Color::Blue();
    }
    
    void onUpdate() override {
        // 按 + 退出
        if (Engine::getInstance().getInput()->getButtonDown(Button::Plus)) {
            Engine::getInstance().quit();
        }
    }
};

int main() {
    Engine& engine = Engine::getInstance();
    
    Engine::Config config;
    config.title = "My First Game";
    
    if (!engine.initialize(config)) return -1;
    
    engine.loadScene(std::make_unique<MyScene>());
    engine.run();
    engine.shutdown();
    
    return 0;
}
```

### 3. 复制 Makefile

从示例项目复制 Makefile 并修改：
```makefile
TARGET := my-game
SOURCES := src
```

### 4. 编译并运行

```bash
make
# 生成 my-game.nro
```

## 🎮 常用功能速查

### 输入检测
```cpp
InputManager* input = Engine::getInstance().getInput();

// 按钮
if (input->getButtonDown(Button::A)) { /* 按下 */ }
if (input->getButton(Button::B)) { /* 保持 */ }
if (input->getButtonUp(Button::X)) { /* 松开 */ }

// 摇杆
Vector2 stick = input->getLeftStick();
float horizontal = stick.x;  // -1.0 到 1.0

// 触摸
if (input->getTouchDown()) {
    Vector2 pos = input->getTouchPosition();
}
```

### 移动物体
```cpp
// 方法1: 直接修改位置
transform->position.x += 100 * Time::deltaTime;

// 方法2: 使用物理
Rigidbody* rb = obj->addComponent<Rigidbody>();
rb->velocity = {100, 0};

// 方法3: 使用方向
Vector2 direction = Vector2::Right();
transform->position += direction * speed * Time::deltaTime;
```

### 加载资源
```cpp
ResourceManager* res = Engine::getInstance().getResources();

// 纹理
auto texture = res->loadTexture("romfs:/sprites/player.png");
sprite->setTexture(texture);

// 音效
auto sound = res->loadSound("romfs:/sounds/jump.wav");
Engine::getInstance().getAudio()->playSound(sound);
```

### 碰撞检测
```cpp
// 添加碰撞体
BoxCollider* box = obj->addComponent<BoxCollider>();
box->size = {50, 50};

// 或圆形
CircleCollider* circle = obj->addComponent<CircleCollider>();
circle->radius = 25;

// 在 Scene::onUpdate() 中检测
Physics::checkCollisions(this);
```

### 创建动画
```cpp
Animation walkAnim;
for (int i = 0; i < 4; i++) {
    walkAnim.addFrame(Rectangle{i * 32, 0, 32, 32}, 0.1f);
}

Animator* anim = player->addComponent<Animator>();
anim->spriteRenderer = player->getComponent<SpriteRenderer>();
anim->addAnimation("walk", walkAnim);
anim->play("walk");
```

## 🐛 调试技巧

### 1. 使用 nxlink 查看日志

```bash
# 电脑端
nxlink -s switch2d-demo.nro

# 然后在 Switch 上运行程序
# 日志会显示在电脑终端
```

### 2. 添加调试信息

```cpp
printf("Player position: %.1f, %.1f\n", 
       transform->position.x, 
       transform->position.y);
```

### 3. FPS 监控

```cpp
void onUpdate() override {
    if (Time::frameCount % 60 == 0) {
        printf("FPS: %.1f\n", 1.0f / Time::deltaTime);
    }
}
```

## 💡 常见问题

### Q: 编译时找不到 SDL2 库
```bash
# 重新安装
sudo dkp-pacman -S switch-sdl2 switch-sdl2_image switch-sdl2_mixer
```

### Q: 游戏运行很卡
- 减少活动对象数量
- 优化碰撞检测（只在必要时使用）
- 关闭不需要的功能

### Q: 触摸不工作
- 确保 `hidInitializeTouchScreen()` 被调用
- 检查是否在手持模式

### Q: 音频没有声音
- 检查音频文件格式（建议 WAV/OGG）
- 确保文件在 romfs 目录中
- 检查音量设置

## 📚 下一步学习

1. **查看完整示例** - `examples/main.cpp`
2. **阅读 API 文档** - 浏览头文件中的注释
3. **参考其他项目** - `../graphics/sdl2/` 中的示例
4. **加入社区** - GBAtemp, r/SwitchHacks

## 🎯 项目创意

开始简单，逐步提升：

1. **初级** - Pong 游戏
2. **初级** - 飞机射击游戏
3. **中级** - 平台跳跃游戏
4. **中级** - 拼图游戏
5. **高级** - RPG 游戏
6. **高级** - 物理益智游戏

## 🔗 有用链接

- [DevkitPro 文档](https://devkitpro.org/wiki/)
- [SDL2 文档](https://wiki.libsdl.org/)
- [Switch 自制软件指南](https://switchbrew.org/)

---

**开始创造吧！** 🚀
