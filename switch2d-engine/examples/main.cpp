#include "Switch2D/Switch2D.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace Switch2D;

// ============================================
// 玩家控制器组件
// ============================================
class PlayerController : public Component {
public:
    float moveSpeed = 200.0f;
    
    Rigidbody* rigidbody = nullptr;
    SpriteRenderer* spriteRenderer = nullptr;
    
    void onStart() override {
        rigidbody = gameObject->getComponent<Rigidbody>();
        spriteRenderer = gameObject->getComponent<SpriteRenderer>();
    }
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 上下左右移动
        Vector2 movement = Vector2::Zero();
        
        if (input->getButton(Button::Left) || input->getLeftStick().x < -0.3f) {
            movement.x = -1;
            if (spriteRenderer) spriteRenderer->flipX = true;
        }
        if (input->getButton(Button::Right) || input->getLeftStick().x > 0.3f) {
            movement.x = 1;
            if (spriteRenderer) spriteRenderer->flipX = false;
        }
        if (input->getButton(Button::Up) || input->getLeftStick().y > 0.3f) {
            movement.y = -1;
        }
        if (input->getButton(Button::Down) || input->getLeftStick().y < -0.3f) {
            movement.y = 1;
        }
        
        // 归一化移动向量（避免斜向移动过快）
        if (movement.lengthSquared() > 0) {
            movement = movement.normalized();
        }
        
        if (rigidbody) {
            rigidbody->velocity = movement * moveSpeed;
        } else {
            transform->position += movement * moveSpeed * Time::deltaTime;
        }
        
        // 边界限制
        Engine& engine = Engine::getInstance();
        if (transform->position.x < 0) transform->position.x = 0;
        if (transform->position.x > engine.getScreenWidth()) 
            transform->position.x = engine.getScreenWidth();
        if (transform->position.y < 0) transform->position.y = 0;
        if (transform->position.y > engine.getScreenHeight())
            transform->position.y = engine.getScreenHeight();
    }
};

// ============================================
// 自动旋转组件
// ============================================
class Rotator : public Component {
public:
    float rotationSpeed = 90.0f; // 度/秒
    
    void onUpdate() override {
        transform->rotation += rotationSpeed * Time::deltaTime;
        if (transform->rotation >= 360.0f) {
            transform->rotation -= 360.0f;
        }
    }
};

// ============================================
// 随机移动组件
// ============================================
class RandomMover : public Component {
public:
    Vector2 velocity;
    float speed = 100.0f;
    Rectangle bounds{0, 0, 1280, 720};
    
    void onStart() override {
        // 随机初始方向
        float angle = (rand() % 360) * Math::DEG2RAD;
        velocity = {std::cos(angle), std::sin(angle)};
        velocity = velocity * speed;
    }
    
    void onUpdate() override {
        transform->position += velocity * Time::deltaTime;
        
        // 边界反弹
        if (transform->position.x < bounds.x || 
            transform->position.x > bounds.x + bounds.width) {
            velocity.x *= -1;
            transform->position.x = Math::Clamp(transform->position.x, 
                                               bounds.x, bounds.x + bounds.width);
        }
        
        if (transform->position.y < bounds.y || 
            transform->position.y > bounds.y + bounds.height) {
            velocity.y *= -1;
            transform->position.y = Math::Clamp(transform->position.y, 
                                               bounds.y, bounds.y + bounds.height);
        }
    }
};

// ============================================
// 示例游戏场景
// ============================================
class DemoScene : public Scene {
public:
    DemoScene() : Scene("Demo Scene") {}
    
    void onLoad() override {
        printf("Loading Demo Scene...\n");
        
        Engine& engine = Engine::getInstance();
        ResourceManager* resources = engine.getResources();
        
        // 创建摄像机
        GameObject* cameraObj = createGameObject("Main Camera");
        Camera* camera = cameraObj->addComponent<Camera>();
        camera->backgroundColor = Color(50, 50, 100);
        
        // 创建玩家（方块）
        GameObject* player = createGameObject("Player");
        player->transform->position = {640, 500};
        player->transform->scale = {50, 50};
        
        // 添加玩家渲染器（纯色方块）
        SpriteRenderer* playerRenderer = player->addComponent<SpriteRenderer>();
        playerRenderer->tint = Color::Green();
        
        // 添加玩家控制器
        PlayerController* playerCtrl = player->addComponent<PlayerController>();
        playerCtrl->moveSpeed = 300.0f;
        
        // 添加刚体和碰撞体
        Rigidbody* rb = player->addComponent<Rigidbody>();
        rb->mass = 1.0f;
        rb->drag = 1.0f;
        rb->useGravity = false;  // 禁用重力，让玩家可以自由移动
        
        BoxCollider* collider = player->addComponent<BoxCollider>();
        collider->size = {50, 50};
        
        // 创建一些旋转的装饰对象
        for (int i = 0; i < 5; i++) {
            GameObject* decorator = createGameObject("Decorator");
            decorator->transform->position = {
                200.0f + i * 200.0f,
                200.0f
            };
            decorator->transform->scale = {30, 30};
            
            SpriteRenderer* decRenderer = decorator->addComponent<SpriteRenderer>();
            decRenderer->tint = Color(255, 200, 0);
            
            Rotator* rotator = decorator->addComponent<Rotator>();
            rotator->rotationSpeed = 45.0f * (i + 1);
        }
        
        // 创建随机移动的对象
        for (int i = 0; i < 8; i++) {
            GameObject* mover = createGameObject("Mover");
            mover->transform->position = {
                (float)(rand() % 1280),
                (float)(rand() % 720)
            };
            mover->transform->scale = {20, 20};
            
            SpriteRenderer* moverRenderer = mover->addComponent<SpriteRenderer>();
            moverRenderer->tint = Color(
                rand() % 255,
                rand() % 255,
                rand() % 255
            );
            
            RandomMover* randomMover = mover->addComponent<RandomMover>();
            randomMover->speed = 50.0f + (rand() % 150);
            
            CircleCollider* circleCol = mover->addComponent<CircleCollider>();
            circleCol->radius = 10.0f;
        }
        
        printf("Demo Scene loaded!\n");
    }
    
    void onUpdate() override {
        // 检查退出
        InputManager* input = Engine::getInstance().getInput();
        if (input->getButtonDown(Button::Plus)) {
            Engine::getInstance().quit();
        }
        
        // 物理碰撞检测
        Physics::checkCollisions(this);
        
        // 显示FPS
        static uint64_t lastPrintTime = 0;
        if (Time::frameCount % 60 == 0) {
            if (lastPrintTime > 0) {
                float fps = 60.0f / (Time::deltaTime * 60.0f);
                printf("FPS: %.1f | Objects: %zu\n", fps, getGameObjects().size());
            }
            lastPrintTime = Time::frameCount;
        }
    }
};

// ============================================
// 主函数
// ============================================
int main(int argc, char* argv[]) {
    srand(time(NULL));
    
    printf("\n");
    printf("========================================\n");
    printf("  Switch2D Engine Demo\n");
    printf("========================================\n");
    printf("\n");
    printf("Controls:\n");
    printf("  D-Pad/Left Stick - Move player\n");
    printf("  A/B Button - Jump\n");
    printf("  + Button - Exit\n");
    printf("\n");
    
    // 初始化引擎
    Engine& engine = Engine::getInstance();
    
    Engine::Config config;
    config.title = "Switch2D Engine Demo";
    config.screenWidth = 1280;
    config.screenHeight = 720;
    config.vsync = true;
    config.targetFPS = 60;
    
    if (!engine.initialize(config)) {
        printf("Failed to initialize engine!\n");
        return -1;
    }
    
    // 加载场景
    engine.loadScene(std::make_unique<DemoScene>());
    
    // 运行游戏
    engine.run();
    
    // 清理
    engine.shutdown();
    
    printf("\nThanks for playing!\n");
    return 0;
}
