# Switch2D Engine API 参考手册

## 核心类 (Core)

### `Engine`
引擎单例，管理整个游戏生命周期。

```cpp
// 获取实例
Engine& engine = Engine::getInstance();

// 配置
Engine::Config config;
config.title = "Game Title";
config.screenWidth = 1280;
config.screenHeight = 720;
config.vsync = true;
config.targetFPS = 60;

// 生命周期
bool initialize(const Config& config);
void run();
void shutdown();
void quit();

// 场景管理
void loadScene(std::unique_ptr<Scene> scene);
Scene* getCurrentScene();

// 访问子系统
SDL_Renderer* getRenderer();
InputManager* getInput();
AudioManager* getAudio();
ResourceManager* getResources();

// 屏幕信息
int getScreenWidth();
int getScreenHeight();
bool isRunning();
```

### `Time`
时间和帧率管理。

```cpp
// 静态成员
static float deltaTime;      // 上一帧耗时（秒）
static float timeScale;      // 时间缩放（慢动作/加速）
static uint64_t frameCount;  // 总帧数

// 使用示例
transform->position.x += 100 * Time::deltaTime;
```

---

## 场景系统 (Scene)

### `Scene`
游戏场景基类。

```cpp
class MyScene : public Scene {
public:
    MyScene() : Scene("Scene Name") {}
    
    // 重写生命周期方法
    void onLoad() override;     // 场景加载时
    void onUnload() override;   // 场景卸载时
    void onUpdate() override;   // 每帧更新
    void onRender() override;   // 每帧渲染后
};

// 游戏对象管理
GameObject* createGameObject(const std::string& name);
void destroyGameObject(GameObject* obj);

// 查找对象
GameObject* findGameObject(const std::string& name);
std::vector<GameObject*> findGameObjectsWithTag(const std::string& tag);
```

---

## 游戏对象 (GameObject)

### `GameObject`
游戏中的实体对象。

```cpp
// 属性
std::string name;
std::string tag;
bool active;
Transform* transform;
Scene* scene;

// 组件管理
template<typename T, typename... Args>
T* addComponent(Args&&... args);

template<typename T>
T* getComponent();

template<typename T>
bool hasComponent();

template<typename T>
void removeComponent();

// 使用示例
auto sprite = player->addComponent<SpriteRenderer>();
auto rb = player->getComponent<Rigidbody>();
if (player->hasComponent<BoxCollider>()) { /*...*/ }
```

### `Component`
所有组件的基类。

```cpp
class MyComponent : public Component {
public:
    GameObject* gameObject;  // 所属对象
    Transform* transform;    // 快捷访问变换
    bool enabled;            // 是否启用
    
    // 生命周期回调
    virtual void onStart();    // 首次更新前
    virtual void onUpdate();   // 每帧更新
    virtual void onRender();   // 渲染时
    virtual void onDestroy();  // 销毁时
};
```

### `Transform`
位置、旋转、缩放组件。

```cpp
Vector2 position;
float rotation;      // 角度
Vector2 scale;

// 层级关系
Transform* parent;
std::vector<Transform*> children;

// 世界坐标
Vector2 getWorldPosition();
float getWorldRotation();
Vector2 getWorldScale();

// 层级管理
void setParent(Transform* newParent);
void addChild(Transform* child);
void removeChild(Transform* child);

// 方向向量
Vector2 forward();
Vector2 right();
```

---

## 渲染系统 (Renderer)

### `SpriteRenderer`
精灵渲染器。

```cpp
std::shared_ptr<Texture> texture;
Rectangle sourceRect;    // 纹理源区域
Color tint;              // 颜色调制
Vector2 pivot;           // 锚点 (0-1)
int sortingOrder;        // 渲染顺序
bool flipX, flipY;       // 翻转

void setTexture(std::shared_ptr<Texture> tex);
```

### `Camera`
摄像机。

```cpp
static Camera* main;     // 主摄像机

Color backgroundColor;
float zoom;

Vector2 worldToScreen(const Vector2& worldPos);
Vector2 screenToWorld(const Vector2& screenPos);
```

### `Animator`
动画控制器。

```cpp
void addAnimation(const std::string& name, const Animation& anim);
void play(const std::string& name);
void stop();
void pause();
void resume();

bool isPlaying();
std::string getCurrentAnimation();

SpriteRenderer* spriteRenderer;  // 关联的精灵渲染器
```

### `Animation`
动画数据。

```cpp
struct AnimationFrame {
    Rectangle sourceRect;
    float duration;  // 秒
};

std::vector<AnimationFrame> frames;
bool loop;

void addFrame(const Rectangle& rect, float duration);
```

---

## 输入系统 (Input)

### `InputManager`
输入管理器。

```cpp
// 按钮枚举
enum class Button {
    A, B, X, Y,
    L, R, ZL, ZR,
    Plus, Minus,
    Left, Right, Up, Down,
    LeftStick, RightStick
};

// 按钮检测
bool getButton(Button button);      // 保持按下
bool getButtonDown(Button button);  // 刚按下
bool getButtonUp(Button button);    // 刚松开

// 摇杆
Vector2 getLeftStick();   // (-1 到 1)
Vector2 getRightStick();

// 触摸
bool isTouching();
Vector2 getTouchPosition();
bool getTouchDown();
bool getTouchUp();

// 陀螺仪
Vector2 getGyro();
```

---

## 音频系统 (Audio)

### `AudioManager`
音频管理器。

```cpp
// 音效
void playSound(std::shared_ptr<AudioClip> clip, int loops = 0, float volume = 1.0f);
void stopSound(int channel = -1);
void setSoundVolume(float volume, int channel = -1);

// 音乐
void playMusic(std::shared_ptr<AudioClip> clip, int loops = -1);
void stopMusic();
void pauseMusic();
void resumeMusic();
void setMusicVolume(float volume);
bool isMusicPlaying();

// 主音量
void setMasterVolume(float volume);
float getMasterVolume();
```

### `AudioClip`
音频资源。

```cpp
enum class Type {
    Sound,  // 短音效 (WAV)
    Music   // 背景音乐 (OGG/MP3)
};

bool loadFromFile(const std::string& path, Type type);
```

---

## 资源管理 (Resources)

### `ResourceManager`
资源加载和缓存。

```cpp
// 纹理
std::shared_ptr<Texture> loadTexture(const std::string& path);
std::shared_ptr<Texture> getTexture(const std::string& path);
void unloadTexture(const std::string& path);

// 音频
std::shared_ptr<AudioClip> loadSound(const std::string& path);
std::shared_ptr<AudioClip> loadMusic(const std::string& path);
std::shared_ptr<AudioClip> getAudio(const std::string& path);
void unloadAudio(const std::string& path);

// 清理
void clear();
```

### `Texture`
纹理资源。

```cpp
bool loadFromFile(const std::string& path, SDL_Renderer* renderer);
void free();

SDL_Texture* getSDLTexture();
int getWidth();
int getHeight();
```

---

## 物理系统 (Physics)

### `Rigidbody`
刚体组件。

```cpp
Vector2 velocity;
Vector2 acceleration;
float mass;
float drag;
float gravityScale;
bool useGravity;
bool isKinematic;

void addForce(const Vector2& force);
void addImpulse(const Vector2& impulse);

static Vector2 gravity;  // 全局重力
```

### `BoxCollider`
盒碰撞体。

```cpp
Vector2 size;
Vector2 offset;
bool isTrigger;

Rectangle getBounds();
```

### `CircleCollider`
圆形碰撞体。

```cpp
float radius;
Vector2 offset;
bool isTrigger;

Rectangle getBounds();
```

### `Physics`
物理工具类。

```cpp
static void checkCollisions(Scene* scene);
static bool checkCollision(BoxCollider* box1, BoxCollider* box2);
static bool checkCollision(CircleCollider* c1, CircleCollider* c2);
static bool checkCollision(BoxCollider* box, CircleCollider* circle);
```

---

## 数学库 (Math)

### `Vector2`
2D向量。

```cpp
float x, y;

// 构造
Vector2();
Vector2(float x, float y);

// 运算符
Vector2 operator+(const Vector2& v);
Vector2 operator-(const Vector2& v);
Vector2 operator*(float s);
Vector2 operator/(float s);

// 方法
float length();
float lengthSquared();
Vector2 normalized();
float dot(const Vector2& v);

// 预定义
static Vector2 Zero();
static Vector2 One();
static Vector2 Up();
static Vector2 Down();
static Vector2 Left();
static Vector2 Right();
```

### `Rectangle`
矩形。

```cpp
float x, y, width, height;

Rectangle();
Rectangle(float x, float y, float w, float h);

float left();
float right();
float top();
float bottom();
Vector2 center();

bool contains(const Vector2& point);
bool intersects(const Rectangle& other);
```

### `Color`
颜色。

```cpp
uint8_t r, g, b, a;

Color();
Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

// 预定义
static Color White();
static Color Black();
static Color Red();
static Color Green();
static Color Blue();
static Color Yellow();
static Color Cyan();
static Color Magenta();
static Color Transparent();
```

### `Math`
数学工具函数。

```cpp
namespace Math {
    constexpr float PI;
    constexpr float DEG2RAD;
    constexpr float RAD2DEG;
    
    float Clamp(float value, float min, float max);
    float Lerp(float a, float b, float t);
    Vector2 Lerp(const Vector2& a, const Vector2& b, float t);
}
```

---

## 完整示例

### 创建可控角色

```cpp
class Player : public Component {
public:
    float speed = 300.0f;
    float jumpForce = 500.0f;
    bool isGrounded = false;
    
    Rigidbody* rb;
    SpriteRenderer* sprite;
    Animator* animator;
    
    void onStart() override {
        rb = gameObject->getComponent<Rigidbody>();
        sprite = gameObject->getComponent<SpriteRenderer>();
        animator = gameObject->getComponent<Animator>();
    }
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 移动
        float horizontal = 0;
        if (input->getButton(Button::Left)) horizontal = -1;
        if (input->getButton(Button::Right)) horizontal = 1;
        
        rb->velocity.x = horizontal * speed;
        
        // 翻转精灵
        if (horizontal < 0) sprite->flipX = true;
        if (horizontal > 0) sprite->flipX = false;
        
        // 跳跃
        if (isGrounded && input->getButtonDown(Button::A)) {
            rb->addImpulse({0, -jumpForce});
        }
        
        // 动画
        if (horizontal != 0) {
            animator->play("walk");
        } else {
            animator->play("idle");
        }
    }
};
```

### 创建完整游戏场景

```cpp
class GameScene : public Scene {
    void onLoad() override {
        // 摄像机
        auto camObj = createGameObject("Camera");
        camObj->addComponent<Camera>();
        
        // 玩家
        auto player = createGameObject("Player");
        player->transform->position = {640, 500};
        player->addComponent<Player>();
        
        auto sprite = player->addComponent<SpriteRenderer>();
        auto tex = Engine::getInstance().getResources()
            ->loadTexture("romfs:/player.png");
        sprite->setTexture(tex);
        
        auto rb = player->addComponent<Rigidbody>();
        rb->useGravity = true;
        
        auto collider = player->addComponent<BoxCollider>();
        collider->size = {32, 64};
        
        // 敌人
        for (int i = 0; i < 5; i++) {
            auto enemy = createGameObject("Enemy");
            enemy->transform->position = {200.0f * i, 300};
            // ... 添加组件
        }
    }
    
    void onUpdate() override {
        Physics::checkCollisions(this);
    }
};
```

---

**完整 API 文档**
