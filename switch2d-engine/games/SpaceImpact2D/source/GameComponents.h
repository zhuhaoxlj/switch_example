#ifndef GAME_COMPONENTS_H
#define GAME_COMPONENTS_H

#include "Switch2D/Switch2D.h"
#include <vector>
#include <cstdlib>

using namespace Switch2D;

// 前向声明
class GameManager;

// ============================================
// 游戏管理器 - 管理分数、生命值和游戏状态
// ============================================
class GameManager : public Component {
public:
    int score = 0;
    int lives = 3;
    int level = 0;
    bool gameOver = false;
    
    void addScore(int points) {
        score += points;
    }
    
    void loseLife() {
        if (lives > 0) {
            lives--;
            if (lives == 0) {
                gameOver = true;
            }
        }
    }
    
    void onUpdate() override {
        // 游戏结束检查
        if (gameOver) {
            // 这里可以显示游戏结束界面
        }
    }
};

// ============================================
// 子弹组件
// ============================================
class Bullet : public Component {
public:
    float speed = 400.0f;
    bool fromPlayer = true;
    int damage = 1;
    
    void onUpdate() override {
        // 子弹向右移动
        if (fromPlayer) {
            transform->position.x += speed * Time::deltaTime;
        } else {
            transform->position.x -= speed * Time::deltaTime;
        }
        
        // 超出屏幕则销毁
        if (transform->position.x > 1300 || transform->position.x < -20) {
            gameObject->destroy();
        }
    }
    
};

// ============================================
// 玩家控制器组件
// ============================================
class PlayerController : public Component {
public:
    float moveSpeed = 300.0f;
    float shootCooldown = 0.2f;
    float shootTimer = 0.0f;
    GameManager* gameManager = nullptr;
    bool invincible = false;
    float invincibleTimer = 0.0f;
    float invincibleDuration = 2.0f;
    
    SpriteRenderer* spriteRenderer = nullptr;
    
    void onStart() override {
        spriteRenderer = gameObject->getComponent<SpriteRenderer>();
        gameObject->tag = "Player";
    }
    
    void onUpdate() override {
        InputManager* input = Engine::getInstance().getInput();
        
        // 更新无敌时间
        if (invincible) {
            invincibleTimer += Time::deltaTime;
            if (invincibleTimer >= invincibleDuration) {
                invincible = false;
                invincibleTimer = 0.0f;
                if (spriteRenderer) {
                    spriteRenderer->tint = Color(0, 255, 0);
                }
            } else {
                // 闪烁效果
                if (spriteRenderer) {
                    int flash = (int)(invincibleTimer * 10) % 2;
                    spriteRenderer->tint = flash ? Color(0, 255, 0) : Color(100, 100, 100);
                }
            }
        }
        
        // 移动控制
        Vector2 movement{0, 0};
        
        if (input->getButton(Button::Up)) movement.y = -1;
        if (input->getButton(Button::Down)) movement.y = 1;
        if (input->getButton(Button::Left)) movement.x = -1;
        if (input->getButton(Button::Right)) movement.x = 1;
        
        // 摇杆输入
        Vector2 stick = input->getLeftStick();
        if (std::abs(stick.x) > 0.3f || std::abs(stick.y) > 0.3f) {
            movement = stick;
        }
        
        // 应用移动
        transform->position += movement * moveSpeed * Time::deltaTime;
        
        // 边界限制
        if (transform->position.x < 50) transform->position.x = 50;
        if (transform->position.x > 1230) transform->position.x = 1230;
        if (transform->position.y < 50) transform->position.y = 50;
        if (transform->position.y > 670) transform->position.y = 670;
        
        // 射击
        shootTimer += Time::deltaTime;
        if ((input->getButton(Button::A) || input->getButton(Button::B)) && shootTimer >= shootCooldown) {
            shoot();
            shootTimer = 0.0f;
        }
    }
    
    void shoot() {
        Scene* scene = gameObject->scene;
        if (!scene) return;
        
        // 创建子弹
        GameObject* bullet = scene->createGameObject("PlayerBullet");
        bullet->transform->position = transform->position + Vector2{40, 0};
        bullet->transform->scale = {10, 5}; // 设置子弹大小
        
        auto bulletSprite = bullet->addComponent<SpriteRenderer>();
        bulletSprite->tint = Color(255, 255, 0); // 黄色子弹
        
        auto bulletComponent = bullet->addComponent<Bullet>();
        bulletComponent->fromPlayer = true;
        bulletComponent->speed = 500.0f;
        
        auto bulletCollider = bullet->addComponent<BoxCollider>();
        bulletCollider->size = {10, 5};
        
        bullet->tag = "PlayerBullet";
    }
    
};

// ============================================
// 敌人组件
// ============================================
class Enemy : public Component {
public:
    float moveSpeed = 100.0f;
    int health = 1;
    int scoreValue = 100;
    float shootInterval = 2.0f;
    float shootTimer = 0.0f;
    GameManager* gameManager = nullptr;
    
    void onStart() override {
        gameObject->tag = "Enemy";
        shootTimer = (float)rand() / RAND_MAX * shootInterval;
    }
    
    void onUpdate() override {
        // 敌人向左移动
        transform->position.x -= moveSpeed * Time::deltaTime;
        
        // 超出屏幕左侧则销毁
        if (transform->position.x < -50) {
            gameObject->destroy();
            return;
        }
        
        // 射击
        shootTimer += Time::deltaTime;
        if (shootTimer >= shootInterval) {
            shoot();
            shootTimer = 0.0f;
        }
    }
    
    void shoot() {
        Scene* scene = gameObject->scene;
        if (!scene) return;
        
        // 创建敌人子弹
        GameObject* bullet = scene->createGameObject("EnemyBullet");
        bullet->transform->position = transform->position + Vector2{-20, 0};
        bullet->transform->scale = {10, 5}; // 设置子弹大小
        
        auto bulletSprite = bullet->addComponent<SpriteRenderer>();
        bulletSprite->tint = Color(255, 0, 0); // 红色子弹
        
        auto bulletComponent = bullet->addComponent<Bullet>();
        bulletComponent->fromPlayer = false;
        bulletComponent->speed = 300.0f;
        
        auto bulletCollider = bullet->addComponent<BoxCollider>();
        bulletCollider->size = {10, 5};
        
        bullet->tag = "EnemyBullet";
    }
    
};

// ============================================
// 敌人生成器
// ============================================
class EnemySpawner : public Component {
public:
    float spawnInterval = 2.0f;
    float spawnTimer = 0.0f;
    GameManager* gameManager = nullptr;
    
    void onUpdate() override {
        spawnTimer += Time::deltaTime;
        
        if (spawnTimer >= spawnInterval) {
            spawnEnemy();
            spawnTimer = 0.0f;
        }
    }
    
    void spawnEnemy() {
        Scene* scene = gameObject->scene;
        if (!scene) return;
        
        // 在屏幕右侧随机位置生成敌人
        float randomY = 100 + (rand() % 520);
        
        GameObject* enemy = scene->createGameObject("Enemy");
        enemy->transform->position = {1300, randomY};
        enemy->transform->scale = {40, 30}; // 设置敌人大小
        
        auto enemySprite = enemy->addComponent<SpriteRenderer>();
        enemySprite->tint = Color(255, 0, 0); // 红色敌人
        
        auto enemyComponent = enemy->addComponent<Enemy>();
        enemyComponent->gameManager = gameManager;
        enemyComponent->health = 1 + (rand() % 3); // 1-3 生命值
        
        auto enemyCollider = enemy->addComponent<BoxCollider>();
        enemyCollider->size = {40, 30};
    }
};

// ============================================
// 背景滚动组件
// ============================================
class BackgroundScroller : public Component {
public:
    float scrollSpeed = 50.0f;
    
    void onUpdate() override {
        transform->position.x -= scrollSpeed * Time::deltaTime;
        
        // 如果移出屏幕左侧，移到右侧
        if (transform->position.x < -640) {
            transform->position.x += 1920;
        }
    }
};

// ============================================
// UI 渲染器
// ============================================
class UIRenderer : public Component {
public:
    GameManager* gameManager = nullptr;
    
    void onRender() override {
        if (!gameManager) return;
        
        SDL_Renderer* renderer = Engine::getInstance().getRenderer();
        if (!renderer) return;
        
        // 绘制生命值指示器（简单的方块）
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < gameManager->lives; i++) {
            SDL_Rect rect = {10 + i * 30, 10, 20, 20};
            SDL_RenderFillRect(renderer, &rect);
        }
        
        // 绘制分数指示器（简单的方块数量表示）
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        int scoreBlocks = gameManager->score / 100;
        if (scoreBlocks > 20) scoreBlocks = 20;
        for (int i = 0; i < scoreBlocks; i++) {
            SDL_Rect rect = {10 + i * 15, 40, 10, 10};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};

#endif // GAME_COMPONENTS_H
