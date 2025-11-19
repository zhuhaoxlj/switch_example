#include "Switch2D/Switch2D.h"
#include "GameComponents.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace Switch2D;

// ============================================
// Space Impact 游戏场景
// ============================================
class SpaceImpactScene : public Scene {
private:
    GameObject* player;
    GameObject* uiCanvas;
    GameManager* gameManager;
    
public:
    void onLoad() override {
        // 创建摄像机（必需！）
        GameObject* cameraObj = createGameObject("Camera");
        Camera* camera = cameraObj->addComponent<Camera>();
        camera->backgroundColor = Color(20, 20, 40); // 深蓝色背景
        
        // 创建游戏管理器
        GameObject* managerObj = createGameObject("GameManager");
        gameManager = managerObj->addComponent<GameManager>();
        
        // 创建玩家
        player = createGameObject("Player");
        player->transform->position = {100, 360}; // 屏幕左侧中间
        player->transform->scale = {40, 30}; // 设置大小（因为没有纹理）
        
        auto playerSprite = player->addComponent<SpriteRenderer>();
        playerSprite->tint = Color(0, 255, 0); // 绿色飞船
        
        auto playerController = player->addComponent<PlayerController>();
        playerController->gameManager = gameManager;
        
        auto playerCollider = player->addComponent<BoxCollider>();
        playerCollider->size = {40, 30};
        
        // 创建UI画布
        uiCanvas = createGameObject("UI");
        uiCanvas->transform->position = {10, 10};
        
        auto uiRenderer = uiCanvas->addComponent<UIRenderer>();
        uiRenderer->gameManager = gameManager;
        
        // 创建敌人生成器
        GameObject* spawnerObj = createGameObject("EnemySpawner");
        auto spawner = spawnerObj->addComponent<EnemySpawner>();
        spawner->gameManager = gameManager;
        
        // 创建背景
        createBackground();
    }
    
    void createBackground() {
        // 创建滚动背景
        for (int i = 0; i < 3; i++) {
            GameObject* bg = createGameObject("Background");
            bg->transform->position = {640.0f * i, 360};
            bg->transform->scale = {1280, 720}; // 设置背景大小
            
            auto sprite = bg->addComponent<SpriteRenderer>();
            sprite->tint = Color(50, 50, 80); // 深蓝色背景
            sprite->sortingOrder = -10; // 在最后面渲染
            
            auto scroller = bg->addComponent<BackgroundScroller>();
            scroller->scrollSpeed = 50.0f;
        }
    }
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 按 + 键退出
        if (input->getButtonDown(Button::Plus)) {
            Engine::getInstance().quit();
        }
        
        // 检查碰撞
        Physics::checkCollisions(this);
        checkGameCollisions();
    }
    
    void checkGameCollisions() {
        auto& objects = getGameObjects();
        
        // 检查玩家子弹与敌人的碰撞
        for (auto& obj1 : objects) {
            if (!obj1 || !obj1->active || obj1->tag != "PlayerBullet") continue;
            auto bullet1Collider = obj1->getComponent<BoxCollider>();
            if (!bullet1Collider) continue;
            
            for (auto& obj2 : objects) {
                if (!obj2 || !obj2->active || obj2->tag != "Enemy") continue;
                auto enemyCollider = obj2->getComponent<BoxCollider>();
                if (!enemyCollider) continue;
                
                if (Physics::checkCollision(bullet1Collider, enemyCollider)) {
                    // 子弹击中敌人
                    auto enemyComp = obj2->getComponent<Enemy>();
                    if (enemyComp) {
                        enemyComp->health--;
                        if (enemyComp->health <= 0) {
                            if (gameManager) {
                                gameManager->addScore(enemyComp->scoreValue);
                            }
                            obj2->destroy();
                        }
                    }
                    obj1->destroy();
                    break;
                }
            }
        }
        
        // 检查敌人子弹与玩家的碰撞
        for (auto& obj1 : objects) {
            if (!obj1 || !obj1->active || obj1->tag != "EnemyBullet") continue;
            auto bulletCollider = obj1->getComponent<BoxCollider>();
            if (!bulletCollider) continue;
            
            if (player && player->active) {
                auto playerCollider = player->getComponent<BoxCollider>();
                auto playerCtrl = player->getComponent<PlayerController>();
                
                if (playerCollider && playerCtrl && !playerCtrl->invincible) {
                    if (Physics::checkCollision(bulletCollider, playerCollider)) {
                        obj1->destroy();
                        if (gameManager) {
                            gameManager->loseLife();
                        }
                        playerCtrl->invincible = true;
                        playerCtrl->invincibleTimer = 0.0f;
                    }
                }
            }
        }
        
        // 检查敌人与玩家的碰撞
        for (auto& obj : objects) {
            if (!obj || !obj->active || obj->tag != "Enemy") continue;
            auto enemyCollider = obj->getComponent<BoxCollider>();
            if (!enemyCollider) continue;
            
            if (player && player->active) {
                auto playerCollider = player->getComponent<BoxCollider>();
                auto playerCtrl = player->getComponent<PlayerController>();
                
                if (playerCollider && playerCtrl && !playerCtrl->invincible) {
                    if (Physics::checkCollision(enemyCollider, playerCollider)) {
                        if (gameManager) {
                            gameManager->loseLife();
                        }
                        playerCtrl->invincible = true;
                        playerCtrl->invincibleTimer = 0.0f;
                    }
                }
            }
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
    }
    
    srand(time(NULL));
    
    printf("\n");
    printf("========================================\n");
    printf("  Space Impact 2D\n");
    printf("========================================\n");
    printf("\n");
    printf("Controls:\n");
    printf("  D-Pad/Left Stick - Move ship\n");
    printf("  A or B Button - Shoot\n");
    printf("  + Button - Exit\n");
    printf("\n");
    
    Engine& engine = Engine::getInstance();
    
    // 配置引擎
    Engine::Config config;
    config.title = "Space Impact 2D";
    config.screenWidth = 1280;
    config.screenHeight = 720;
    config.targetFPS = 60;
    
    // 初始化引擎
    if (!engine.initialize(config)) {
        printf("Failed to initialize engine!\n");
        return -1;
    }
    
    printf("Engine initialized successfully!\n");
    printf("Loading scene...\n");
    
    // 加载场景
    engine.loadScene(std::make_unique<SpaceImpactScene>());
    
    printf("Scene loaded! Starting game...\n");
    
    // 运行游戏
    engine.run();
    
    // 关闭引擎
    engine.shutdown();
    
    // 退出 romfs
    romfsExit();
    
    printf("\nThanks for playing!\n");
    
    return 0;
}
