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
        
        // 检测 A+B 切换调试控制台
        static bool wasABPressed = false;
        bool isABPressed = input->getButton(Button::A) && input->getButton(Button::B);
        
        if (isABPressed && !wasABPressed) {
            DebugConsole::getInstance().toggle();
            DEBUG_LOG("Debug console toggled");
        }
        wasABPressed = isABPressed;
        
        // 检测是否在地面上（检测范围稍微宽松一点）
        float distanceToGround = transform->position.y - groundY;
        isGrounded = (distanceToGround >= -5.0f && distanceToGround <= 5.0f);
        
        // 调试输出到控制台（每秒一次）
        static float debugTimer = 0;
        debugTimer += Time::deltaTime;
        if (debugTimer >= 1.0f) {
            DEBUG_LOG("Player Y: %.2f, Ground: %.2f, IsGrounded: %s, Vel.Y: %.2f", 
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
            
            // 跳跃（只在地面上时，但不在按A+B时）
            if ((input->getButtonDown(Button::A) || input->getButtonDown(Button::B)) && !isABPressed) {
                DEBUG_LOG("Button pressed! IsGrounded: %s", isGrounded ? "YES" : "NO");
                if (isGrounded) {
                    rigidbody->velocity.y = -jumpForce;
                    DEBUG_LOG("Jump! velocity.y = %.2f", rigidbody->velocity.y);
                } else {
                    DEBUG_LOG("Cannot jump - not grounded!");
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
// 触控按钮组件
// ============================================
class TouchButton : public Component {
public:
    Rectangle bounds;
    Color normalColor = Color(80, 120, 200);
    Color hoverColor = Color(100, 150, 255);
    Color pressedColor = Color(50, 90, 150);
    std::string label;
    int buttonId = 0;
    
    bool isPressed = false;
    bool wasPressed = false;
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 获取按钮在屏幕上的实际位置
        Vector2 pos = gameObject->transform->position;
        Vector2 scale = gameObject->transform->scale;
        
        bounds.x = pos.x - scale.x / 2;
        bounds.y = pos.y - scale.y / 2;
        bounds.width = scale.x;
        bounds.height = scale.y;
        
        // 检查触摸
        wasPressed = isPressed;
        isPressed = false;
        
        if (input->isTouching()) {
            Vector2 touchPos = input->getTouchPosition();
            
            // 检查触摸点是否在按钮范围内
            if (touchPos.x >= bounds.x && touchPos.x <= bounds.x + bounds.width &&
                touchPos.y >= bounds.y && touchPos.y <= bounds.y + bounds.height) {
                isPressed = true;
                
                // 触摸按下事件
                if (input->getTouchDown()) {
                    onButtonClick();
                }
            }
        }
        
        // 更新按钮颜色
        SpriteRenderer* renderer = gameObject->getComponent<SpriteRenderer>();
        if (renderer) {
            if (isPressed) {
                renderer->tint = pressedColor;
            } else {
                renderer->tint = normalColor;
            }
        }
    }
    
    void onButtonClick() {
        DEBUG_LOG("=== Button %d Clicked! ===", buttonId);
        DEBUG_LOG("Label: %s", label.c_str());
        DEBUG_LOG("Position: (%.0f, %.0f)", bounds.x, bounds.y);
        
        // 根据不同按钮执行不同操作
        switch (buttonId) {
            case 1:
                DEBUG_LOG("Action: Test Button 1");
                DEBUG_LOG("Touch input is working!");
                break;
            case 2:
                DEBUG_LOG("Action: Network Test");
                testNetwork();
                break;
            case 3:
                DEBUG_LOG("Action: Change Color");
                changeRandomColor();
                break;
            case 4:
                DEBUG_LOG("Action: Show Info");
                showSystemInfo();
                break;
        }
    }
    
private:
    void testNetwork() {
        NetworkManager* net = Engine::getInstance().getNetwork();
        if (!net->isInitialized()) {
            DEBUG_LOG("Network not initialized");
            return;
        }
        DEBUG_LOG("Fetching example.com...");
        HTTPResponse resp = net->get("http://example.com");
        if (resp.isSuccess()) {
            DEBUG_LOG("Success! Size: %zu bytes", resp.body.size());
        } else {
            DEBUG_LOG("Failed: %s", resp.error.c_str());
        }
    }
    
    void changeRandomColor() {
        SpriteRenderer* renderer = gameObject->getComponent<SpriteRenderer>();
        if (renderer) {
            normalColor = Color(rand() % 255, rand() % 255, rand() % 255);
            DEBUG_LOG("New color: RGB(%d,%d,%d)", 
                     normalColor.r, normalColor.g, normalColor.b);
        }
    }
    
    void showSystemInfo() {
        DEBUG_LOG("=== System Info ===");
        DEBUG_LOG("Screen: %dx%d", 
                 Engine::getInstance().getScreenWidth(),
                 Engine::getInstance().getScreenHeight());
        DEBUG_LOG("FPS: %.1f", 1.0f / Time::deltaTime);
        DEBUG_LOG("Frame: %llu", Time::frameCount);
        DEBUG_LOG("Objects: %zu", gameObject->scene->getGameObjects().size());
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
        DEBUG_LOG("Loading Demo Scene...");
        DEBUG_LOG("Press X to fetch Baidu homepage!");
        
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
        
        // ========================================
        // 创建触控按钮（屏幕底部）
        // ========================================
        DEBUG_LOG("Creating touch buttons...");
        
        struct ButtonConfig {
            std::string label;
            float x, y;
            Color color;
            int id;
        };
        
        ButtonConfig buttons[] = {
            {"Touch Test", 200, 650, Color(80, 120, 200), 1},
            {"Network", 450, 650, Color(120, 180, 80), 2},
            {"Color", 700, 650, Color(200, 120, 80), 3},
            {"Info", 950, 650, Color(180, 80, 200), 4}
        };
        
        for (int i = 0; i < 4; i++) {
            GameObject* button = createGameObject(buttons[i].label);
            button->transform->position = {buttons[i].x, buttons[i].y};
            button->transform->scale = {180, 60};
            
            SpriteRenderer* btnRenderer = button->addComponent<SpriteRenderer>();
            btnRenderer->tint = buttons[i].color;
            
            TouchButton* touchBtn = button->addComponent<TouchButton>();
            touchBtn->label = buttons[i].label;
            touchBtn->buttonId = buttons[i].id;
            touchBtn->normalColor = buttons[i].color;
            touchBtn->pressedColor = Color(
                buttons[i].color.r * 0.6f,
                buttons[i].color.g * 0.6f,
                buttons[i].color.b * 0.6f
            );
        }
        
        // 显示触摸指示器（红色小圆点）
        touchIndicator = createGameObject("Touch Indicator");
        touchIndicator->transform->scale = {20, 20};
        SpriteRenderer* indicatorRenderer = touchIndicator->addComponent<SpriteRenderer>();
        indicatorRenderer->tint = Color(255, 0, 0, 200);
        touchIndicator->active = false;
        
        printf("Demo Scene loaded!\n");
        DEBUG_LOG("Demo Scene loaded! Press A+B to toggle debug console");
        DEBUG_LOG("=== TOUCH THE BUTTONS! ===");
        DEBUG_LOG("4 buttons at bottom of screen");
    }
    
    void onUpdate() override {
        // 检查退出
        InputManager* input = Engine::getInstance().getInput();
        if (input->getButtonDown(Button::Plus)) {
            Engine::getInstance().quit();
        }
        
        // X 键 - 请求百度网页源码
        if (input->getButtonDown(Button::X)) {
            fetchBaiduPage();
        }
        
        // 物理碰撞检测
        Physics::checkCollisions(this);
        
        // 更新触摸指示器
        if (input->isTouching()) {
            Vector2 touchPos = input->getTouchPosition();
            if (touchIndicator) {
                touchIndicator->active = true;
                touchIndicator->transform->position = touchPos;
            }
            
            // 显示触摸坐标
            static Vector2 lastTouchPos = {-1, -1};
            if (input->getTouchDown() || 
                (touchPos.x != lastTouchPos.x || touchPos.y != lastTouchPos.y)) {
                DEBUG_LOG("Touch at: (%.0f, %.0f)", touchPos.x, touchPos.y);
                lastTouchPos = touchPos;
            }
        } else {
            if (touchIndicator) {
                touchIndicator->active = false;
            }
        }
        
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
    
private:
    GameObject* touchIndicator = nullptr;
    // 获取百度首页源码
    void fetchBaiduPage() {
        DEBUG_LOG("=== Fetching Baidu ===");
        DEBUG_LOG("URL: http://www.baidu.com");
        DEBUG_LOG("Please wait...");
        
        // 获取网络管理器
        NetworkManager* net = Engine::getInstance().getNetwork();
        
        if (!net->isInitialized()) {
            DEBUG_LOG("ERROR: Network not initialized!");
            DEBUG_LOG("Check Switch network settings");
            return;
        }
        
        // 发送 GET 请求
        HTTPResponse resp = net->get("http://www.baidu.com");
        
        if (resp.isSuccess()) {
            DEBUG_LOG("SUCCESS! Status: %d", resp.statusCode);
            
            // 显示响应头
            if (resp.headers.find("Content-Type") != resp.headers.end()) {
                DEBUG_LOG("Content-Type: %s", resp.headers["Content-Type"].c_str());
            }
            if (resp.headers.find("Server") != resp.headers.end()) {
                DEBUG_LOG("Server: %s", resp.headers["Server"].c_str());
            }
            
            // 获取HTML内容
            std::string html = resp.getBodyAsString();
            DEBUG_LOG("Page size: %zu bytes", html.length());
            
            // 显示前500个字符
            if (html.length() > 500) {
                std::string preview = html.substr(0, 500);
                DEBUG_LOG("Preview (first 500 chars):");
                DEBUG_LOG("%s", preview.c_str());
                DEBUG_LOG("... (truncated)");
            } else {
                DEBUG_LOG("Full content:");
                DEBUG_LOG("%s", html.c_str());
            }
            
            // 查找标题
            size_t titleStart = html.find("<title>");
            if (titleStart != std::string::npos) {
                size_t titleEnd = html.find("</title>", titleStart);
                if (titleEnd != std::string::npos) {
                    std::string title = html.substr(titleStart + 7, titleEnd - titleStart - 7);
                    DEBUG_LOG("Page title: %s", title.c_str());
                }
            }
            
            DEBUG_LOG("Request complete!");
        } else {
            DEBUG_LOG("FAILED!");
            if (!resp.error.empty()) {
                DEBUG_LOG("Error: %s", resp.error.c_str());
            }
            if (resp.statusCode > 0) {
                DEBUG_LOG("Status: %d %s", resp.statusCode, resp.statusMessage.c_str());
            }
            DEBUG_LOG("Make sure Switch is connected to network");
        }
    }
};

// ============================================
// 主函数
// ============================================
int main(int argc, char* argv[]) {
    // 初始化 romfs（必须在访问 romfs:/ 文件之前）
    Result rc = romfsInit();
    if (R_FAILED(rc)) {
        printf("romfsInit failed: 0x%x\n", rc);
    } else {
        printf("romfs initialized successfully\n");
    }
    
    srand(time(NULL));
    
    printf("\n");
    printf("========================================\n");
    printf("  Switch2D Engine Demo\n");
    printf("========================================\n");
    printf("\n");
    printf("Controls:\n");
    printf("  D-Pad/Left Stick - Move player\n");
    printf("  A or B Button - Jump\n");
    printf("  X Button - Fetch Baidu.com (网络测试)\n");
    printf("  TOUCHSCREEN - Click buttons at bottom! 👆\n");
    printf("  A + B Together - Toggle Debug Console\n");
    printf("  + Button - Exit\n");
    printf("\n");
    printf("Touch Buttons:\n");
    printf("  🔵 Touch Test - Test touch input\n");
    printf("  🟢 Network - Quick network test\n");
    printf("  🟠 Color - Change button colors\n");
    printf("  🟣 Info - Show system info\n");
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
    
    // 退出 romfs
    romfsExit();
    
    printf("\nThanks for playing!\n");
    return 0;
}
