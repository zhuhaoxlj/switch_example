# Switch2D Engine - 代码示例集

## 📚 目录

1. [最简单的游戏](#1-最简单的游戏)
2. [添加移动控制](#2-添加移动控制)
3. [添加动画](#3-添加动画)
4. [物理和重力](#4-物理和重力)
5. [碰撞检测](#5-碰撞检测)
6. [音效和音乐](#6-音效和音乐)
7. [多个场景](#7-多个场景)
8. [粒子效果模拟](#8-粒子效果模拟)

---

## 1. 最简单的游戏

只显示一个彩色方块：

```cpp
#include "Switch2D/Switch2D.h"

using namespace Switch2D;

class SimpleGame : public Scene {
    void onLoad() override {
        // 创建一个物体
        auto box = createGameObject("Box");
        box->transform->position = {640, 360};
        box->transform->scale = {100, 100};
        
        // 添加颜色
        auto sprite = box->addComponent<SpriteRenderer>();
        sprite->tint = Color::Red();
    }
    
    void onUpdate() override {
        // 按 + 退出
        if (Engine::getInstance().getInput()->getButtonDown(Button::Plus)) {
            Engine::getInstance().quit();
        }
    }
};

int main() {
    Engine::getInstance().initialize({});
    Engine::getInstance().loadScene(std::make_unique<SimpleGame>());
    Engine::getInstance().run();
    return 0;
}
```

---

## 2. 添加移动控制

让方块可以用手柄移动：

```cpp
class MovableBox : public Component {
public:
    float speed = 300.0f;
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 获取方向
        Vector2 direction{0, 0};
        if (input->getButton(Button::Left)) direction.x = -1;
        if (input->getButton(Button::Right)) direction.x = 1;
        if (input->getButton(Button::Up)) direction.y = -1;
        if (input->getButton(Button::Down)) direction.y = 1;
        
        // 归一化方向
        if (direction.lengthSquared() > 0) {
            direction = direction.normalized();
        }
        
        // 移动
        transform->position += direction * speed * Time::deltaTime;
    }
};

class MoveGame : public Scene {
    void onLoad() override {
        auto player = createGameObject("Player");
        player->transform->position = {640, 360};
        player->transform->scale = {50, 50};
        
        auto sprite = player->addComponent<SpriteRenderer>();
        sprite->tint = Color::Blue();
        
        // 添加移动组件
        player->addComponent<MovableBox>();
    }
};
```

---

## 3. 添加动画

创建简单的帧动画：

```cpp
class AnimatedGame : public Scene {
    void onLoad() override {
        auto character = createGameObject("Character");
        character->transform->position = {640, 360};
        
        auto sprite = character->addComponent<SpriteRenderer>();
        
        // 假设有一个精灵表
        auto texture = Engine::getInstance().getResources()
            ->loadTexture("romfs:/spritesheet.bmp");
        sprite->setTexture(texture);
        
        // 创建动画
        Animation walkAnim;
        walkAnim.addFrame(Rectangle{0, 0, 32, 32}, 0.1f);
        walkAnim.addFrame(Rectangle{32, 0, 32, 32}, 0.1f);
        walkAnim.addFrame(Rectangle{64, 0, 32, 32}, 0.1f);
        walkAnim.addFrame(Rectangle{96, 0, 32, 32}, 0.1f);
        walkAnim.loop = true;
        
        auto animator = character->addComponent<Animator>();
        animator->spriteRenderer = sprite;
        animator->addAnimation("walk", walkAnim);
        animator->play("walk");
    }
};
```

---

## 4. 物理和重力

添加重力让物体下落：

```cpp
class PhysicsGame : public Scene {
    void onLoad() override {
        // 创建地面
        auto ground = createGameObject("Ground");
        ground->transform->position = {640, 600};
        ground->transform->scale = {800, 50};
        
        auto groundSprite = ground->addComponent<SpriteRenderer>();
        groundSprite->tint = Color(100, 100, 100);
        
        auto groundCollider = ground->addComponent<BoxCollider>();
        groundCollider->size = {800, 50};
        
        // 创建可以跳跃的玩家
        auto player = createGameObject("Player");
        player->transform->position = {640, 200};
        player->transform->scale = {40, 40};
        
        auto sprite = player->addComponent<SpriteRenderer>();
        sprite->tint = Color::Green();
        
        // 添加刚体
        auto rb = player->addComponent<Rigidbody>();
        rb->mass = 1.0f;
        rb->useGravity = true;
        
        auto collider = player->addComponent<BoxCollider>();
        collider->size = {40, 40};
        
        // 添加跳跃控制
        player->addComponent<JumpController>();
    }
};

class JumpController : public Component {
public:
    float jumpForce = 600.0f;
    bool isGrounded = false;
    
    Rigidbody* rb;
    
    void onStart() override {
        rb = gameObject->getComponent<Rigidbody>();
    }
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 简单的地面检测（Y > 550）
        isGrounded = (transform->position.y > 550);
        
        // 跳跃
        if (isGrounded && input->getButtonDown(Button::A)) {
            rb->velocity.y = -jumpForce;
        }
        
        // 左右移动
        float moveX = 0;
        if (input->getButton(Button::Left)) moveX = -1;
        if (input->getButton(Button::Right)) moveX = 1;
        
        rb->velocity.x = moveX * 200.0f;
    }
};
```

---

## 5. 碰撞检测

检测物体之间的碰撞：

```cpp
class CollisionGame : public Scene {
private:
    GameObject* player = nullptr;
    
    void onLoad() override {
        // 玩家
        player = createGameObject("Player");
        player->transform->position = {640, 360};
        player->transform->scale = {50, 50};
        player->tag = "Player";
        
        auto playerSprite = player->addComponent<SpriteRenderer>();
        playerSprite->tint = Color::Blue();
        
        auto playerCollider = player->addComponent<CircleCollider>();
        playerCollider->radius = 25;
        
        player->addComponent<MovableBox>();
        
        // 创建多个障碍物
        for (int i = 0; i < 5; i++) {
            auto obstacle = createGameObject("Obstacle");
            obstacle->transform->position = {
                200.0f + i * 200.0f,
                300.0f
            };
            obstacle->transform->scale = {60, 60};
            obstacle->tag = "Obstacle";
            
            auto sprite = obstacle->addComponent<SpriteRenderer>();
            sprite->tint = Color::Red();
            
            auto collider = obstacle->addComponent<BoxCollider>();
            collider->size = {60, 60};
        }
    }
    
    void onUpdate() override {
        // 检查碰撞
        Physics::checkCollisions(this);
        
        // 简单的碰撞响应
        auto playerCollider = player->getComponent<CircleCollider>();
        auto obstacles = findGameObjectsWithTag("Obstacle");
        
        for (auto* obj : obstacles) {
            auto obstacleCollider = obj->getComponent<BoxCollider>();
            if (playerCollider->checkCollision(obstacleCollider)) {
                // 碰撞时改变颜色
                auto sprite = obj->getComponent<SpriteRenderer>();
                sprite->tint = Color::Yellow();
            } else {
                auto sprite = obj->getComponent<SpriteRenderer>();
                sprite->tint = Color::Red();
            }
        }
    }
};
```

---

## 6. 音效和音乐

添加声音效果：

```cpp
class AudioGame : public Scene {
private:
    std::shared_ptr<AudioClip> jumpSound;
    std::shared_ptr<AudioClip> bgMusic;
    
    void onLoad() override {
        ResourceManager* res = Engine::getInstance().getResources();
        AudioManager* audio = Engine::getInstance().getAudio();
        
        // 加载音频
        jumpSound = res->loadSound("romfs:/jump.wav");
        bgMusic = res->loadMusic("romfs:/bgm.ogg");
        
        // 播放背景音乐（循环）
        audio->playMusic(bgMusic, -1);
        audio->setMusicVolume(0.5f);
        
        // 创建玩家
        auto player = createGameObject("Player");
        player->transform->position = {640, 360};
        player->addComponent<SoundPlayer>()->jumpSound = jumpSound;
    }
};

class SoundPlayer : public Component {
public:
    std::shared_ptr<AudioClip> jumpSound;
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 按 A 播放音效
        if (input->getButtonDown(Button::A)) {
            Engine::getInstance().getAudio()->playSound(jumpSound);
        }
    }
};
```

---

## 7. 多个场景

实现场景切换：

```cpp
// 菜单场景
class MenuScene : public Scene {
    void onLoad() override {
        printf("=== Main Menu ===\n");
        printf("Press A to Start Game\n");
        printf("Press + to Exit\n");
    }
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        if (input->getButtonDown(Button::A)) {
            // 切换到游戏场景
            Engine::getInstance().loadScene(std::make_unique<GameScene>());
        }
        
        if (input->getButtonDown(Button::Plus)) {
            Engine::getInstance().quit();
        }
    }
};

// 游戏场景
class GameScene : public Scene {
    void onLoad() override {
        printf("=== Game Started ===\n");
        
        auto player = createGameObject("Player");
        player->transform->position = {640, 360};
        // ... 游戏逻辑
    }
    
    void onUpdate() override {
        // 按 B 返回菜单
        if (Engine::getInstance().getInput()->getButtonDown(Button::B)) {
            Engine::getInstance().loadScene(std::make_unique<MenuScene>());
        }
    }
};

int main() {
    Engine::getInstance().initialize({});
    // 从菜单开始
    Engine::getInstance().loadScene(std::make_unique<MenuScene>());
    Engine::getInstance().run();
    return 0;
}
```

---

## 8. 粒子效果模拟

用多个小对象模拟粒子：

```cpp
class Particle : public Component {
public:
    Vector2 velocity;
    float lifetime = 2.0f;
    float age = 0;
    
    void onUpdate() override {
        age += Time::deltaTime;
        
        // 移动
        transform->position += velocity * Time::deltaTime;
        
        // 淡出
        auto sprite = gameObject->getComponent<SpriteRenderer>();
        if (sprite) {
            float alpha = 1.0f - (age / lifetime);
            sprite->tint.a = (uint8_t)(alpha * 255);
        }
        
        // 生命周期结束后销毁
        if (age >= lifetime) {
            gameObject->scene->destroyGameObject(gameObject);
        }
    }
};

class ParticleEmitter : public Component {
public:
    float emitInterval = 0.1f;
    float emitTimer = 0;
    int maxParticles = 50;
    int particleCount = 0;
    
    void onUpdate() override {
        emitTimer += Time::deltaTime;
        
        if (emitTimer >= emitInterval && particleCount < maxParticles) {
            emitParticle();
            emitTimer = 0;
        }
    }
    
    void emitParticle() {
        auto particle = gameObject->scene->createGameObject("Particle");
        particle->transform->position = transform->position;
        particle->transform->scale = {5, 5};
        
        auto sprite = particle->addComponent<SpriteRenderer>();
        sprite->tint = Color(255, 200, 0, 255);
        
        auto p = particle->addComponent<Particle>();
        
        // 随机方向
        float angle = (rand() % 360) * Math::DEG2RAD;
        float speed = 100.0f + (rand() % 100);
        p->velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        
        particleCount++;
    }
};

class ParticleGame : public Scene {
    void onLoad() override {
        auto emitter = createGameObject("Emitter");
        emitter->transform->position = {640, 360};
        emitter->addComponent<ParticleEmitter>();
    }
};
```

---

## 🎯 实用模式

### 单例访问引擎
```cpp
// 快速访问引擎系统
Engine& engine = Engine::getInstance();
InputManager* input = engine.getInput();
AudioManager* audio = engine.getAudio();
ResourceManager* res = engine.getResources();
```

### 组件通信
```cpp
// 方法1：通过 GameObject
auto otherComponent = gameObject->getComponent<OtherComponent>();

// 方法2：通过场景查找
auto target = gameObject->scene->findGameObject("TargetName");

// 方法3：通过标签
auto enemies = gameObject->scene->findGameObjectsWithTag("Enemy");
```

### 对象池模式
```cpp
class ObjectPool {
    std::vector<GameObject*> pool;
    Scene* scene;
    
public:
    GameObject* spawn() {
        // 复用或创建新对象
        for (auto* obj : pool) {
            if (!obj->active) {
                obj->active = true;
                return obj;
            }
        }
        
        auto* newObj = scene->createGameObject("Pooled");
        pool.push_back(newObj);
        return newObj;
    }
    
    void recycle(GameObject* obj) {
        obj->active = false;
    }
};
```

---

## 💡 提示和技巧

### 1. 性能优化
```cpp
// 避免每帧查找
void onStart() override {
    cachedComponent = gameObject->getComponent<SomeComponent>();
}

void onUpdate() override {
    // 使用缓存的引用
    cachedComponent->doSomething();
}
```

### 2. 边界检查
```cpp
void onUpdate() override {
    // 限制在屏幕内
    Engine& engine = Engine::getInstance();
    transform->position.x = Math::Clamp(
        transform->position.x, 0, engine.getScreenWidth()
    );
}
```

### 3. 平滑移动
```cpp
// 使用插值
Vector2 targetPos = {640, 360};
transform->position = Math::Lerp(
    transform->position, 
    targetPos, 
    5.0f * Time::deltaTime
);
```

### 4. 调试输出
```cpp
void onUpdate() override {
    // 每秒输出一次
    static float timer = 0;
    timer += Time::deltaTime;
    if (timer >= 1.0f) {
        printf("Position: %.1f, %.1f\n", 
               transform->position.x, 
               transform->position.y);
        timer = 0;
    }
}
```

---

## 📚 更多资源

- **完整 Demo**：`examples/main.cpp`
- **API 文档**：`API_REFERENCE.md`
- **快速入门**：`QUICKSTART.md`

**开始编码吧！** 🚀
