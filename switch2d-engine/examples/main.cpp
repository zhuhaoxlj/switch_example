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
    float moveSpeed = 250.0f;
    float jumpForce = 450.0f;
    float groundY = 650.0f; // 地面位置
    bool isGrounded = false;
    
    Rigidbody* rigidbody = nullptr;
    SpriteRenderer* spriteRenderer = nullptr;
    
    void onStart() override {
        rigidbody = gameObject->getComponent<Rigidbody>();
        spriteRenderer = gameObject->getComponent<SpriteRenderer>();
    }
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 检测是否在地面上（检测范围稍微宽松一点）
        float distanceToGround = transform->position.y - groundY;
        isGrounded = (distanceToGround >= -5.0f && distanceToGround <= 5.0f);
        
        // 调试输出（每秒打印一次）
        static float debugTimer = 0;
        debugTimer += Time::deltaTime;
        if (debugTimer >= 1.0f) {
            printf("Player Y: %.2f, Ground: %.2f, IsGrounded: %s, Velocity.Y: %.2f\n", 
                   transform->position.y, groundY, isGrounded ? "YES" : "NO", 
                   rigidbody ? rigidbody->velocity.y : 0);
            debugTimer = 0;
        }
        
        // 左右移动
        float moveX = 0;
        if (input->getButton(Button::Left) || input->getLeftStick().x < -0.3f) {
            moveX = -1;
            if (spriteRenderer) spriteRenderer->flipX = true;
        }
        if (input->getButton(Button::Right) || input->getLeftStick().x > 0.3f) {
            moveX = 1;
            if (spriteRenderer) spriteRenderer->flipX = false;
        }
        
        if (rigidbody) {
            // 只控制水平速度，垂直速度由重力控制
            rigidbody->velocity.x = moveX * moveSpeed;
            
            // 跳跃（只在地面上时）
            if (input->getButtonDown(Button::A) || input->getButtonDown(Button::B)) {
                printf("Button pressed! IsGrounded: %s\n", isGrounded ? "YES" : "NO");
                if (isGrounded) {
                    rigidbody->velocity.y = -jumpForce;
                    printf("Jump! velocity.y = %.2f\n", rigidbody->velocity.y);
                } else {
                    printf("Cannot jump - not grounded!\n");
                }
            }
            
            // 简单的地面碰撞
            if (transform->position.y >= groundY) {
                transform->position.y = groundY;
                if (rigidbody->velocity.y > 0) {
                    rigidbody->velocity.y = 0;
                }
            }
        }
        
        // 边界限制
        Engine& engine = Engine::getInstance();
        if (transform->position.x < 25) transform->position.x = 25;
        if (transform->position.x > engine.getScreenWidth() - 25) 
            transform->position.x = engine.getScreenWidth() - 25;
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
        
        // 创建地面平台
        GameObject* ground = createGameObject("Ground");
        ground->transform->position = {640, 680};
        ground->transform->scale = {1280, 80};
        
        SpriteRenderer* groundRenderer = ground->addComponent<SpriteRenderer>();
        groundRenderer->tint = Color(80, 80, 80); // 深灰色
        
        // 创建玩家（方块）
        GameObject* player = createGameObject("Player");
        player->transform->position = {640, 650};
        player->transform->scale = {50, 50};
        
        // 添加玩家渲染器（纯色方块）
        SpriteRenderer* playerRenderer = player->addComponent<SpriteRenderer>();
        playerRenderer->tint = Color::Green();
        
        // 添加玩家控制器
        PlayerController* playerCtrl = player->addComponent<PlayerController>();
        playerCtrl->groundY = 650.0f;  // 设置地面位置
        
        // 添加刚体（启用重力）
        Rigidbody* rb = player->addComponent<Rigidbody>();
        rb->mass = 1.0f;
        rb->drag = 0.5f;
        rb->useGravity = true;  // 启用重力
        rb->gravityScale = 1.0f;
        
        BoxCollider* collider = player->addComponent<BoxCollider>();
        collider->size = {50, 50};
        
        // 创建几个跳跃平台
        float platformYPositions[] = {550, 450, 350, 450, 550};
        float platformXPositions[] = {200, 400, 640, 880, 1080};
        for (int i = 0; i < 5; i++) {
            GameObject* platform = createGameObject("Platform");
            platform->transform->position = {platformXPositions[i], platformYPositions[i]};
            platform->transform->scale = {150, 20};
            
            SpriteRenderer* platRenderer = platform->addComponent<SpriteRenderer>();
            platRenderer->tint = Color(120, 120, 120);
        }
        
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
