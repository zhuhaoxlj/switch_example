#include "FoxScene.h"
#include <cmath>

// ============================================
// FoxController 实现
// ============================================
void FoxController::onStart()
{
    spriteRenderer = gameObject->getComponent<SpriteRenderer>();
    animator = gameObject->getComponent<Animator>();

    // 默认播放 idle 动画
    if (animator)
    {
        animator->play("idle");
    }
}

void FoxController::onUpdate()
{
    InputManager *input = Engine::getInstance().getInput();

    // 左右移动
    float moveX = 0;
    float moveY = 0;

    if (input->getButton(Button::Left) || input->getLeftStick().x < -0.3f)
    {
        moveX = -1;
        if (spriteRenderer)
            spriteRenderer->flipX = true;
    }
    if (input->getButton(Button::Right) || input->getLeftStick().x > 0.3f)
    {
        moveX = 1;
        if (spriteRenderer)
            spriteRenderer->flipX = false;
    }
    if (input->getButton(Button::Up) || input->getLeftStick().y < -0.3f)
    {
        moveY = -1;
    }
    if (input->getButton(Button::Down) || input->getLeftStick().y > 0.3f)
    {
        moveY = 1;
    }

    // 应用移动
    transform->position.x += moveX * moveSpeed * Time::deltaTime;
    transform->position.y += moveY * moveSpeed * Time::deltaTime;

    // 根据移动状态切换动画
    if (animator)
    {
        bool isMoving = (moveX != 0 || moveY != 0);

        if (isMoving)
        {
            // 计算移动速度（判断是走还是跑）
            float speed = sqrt(moveX * moveX + moveY * moveY);

            // 快速移动时播放跑步动画，否则播放走路动画
            if (speed > 0.8f)
            {
                if (animator->getCurrentAnimation() != "run")
                {
                    animator->play("run");
                }
            }
            else
            {
                if (animator->getCurrentAnimation() != "walk")
                {
                    animator->play("walk");
                }
            }
        }
        else
        {
            // 停止时播放站立动画
            if (animator->getCurrentAnimation() != "idle")
            {
                animator->play("idle");
            }
        }
    }

    // 边界限制
    Engine &engine = Engine::getInstance();
    if (transform->position.x < 50)
        transform->position.x = 50;
    if (transform->position.x > engine.getScreenWidth() - 50)
        transform->position.x = engine.getScreenWidth() - 50;
    if (transform->position.y < 50)
        transform->position.y = 50;
    if (transform->position.y > engine.getScreenHeight() - 50)
        transform->position.y = engine.getScreenHeight() - 50;
}

// ============================================
// FoxScene 实现
// ============================================
void FoxScene::onLoad()
{
    DEBUG_LOG("=== Fox Scene ===");
    DEBUG_LOG("Use D-Pad or Left Stick to move the fox!");
    DEBUG_LOG("Press B to return to main menu");

    Engine &engine = Engine::getInstance();
    ResourceManager *resources = engine.getResources();
    AudioManager *audio = engine.getAudio();

    // 加载并播放背景音乐
    DEBUG_LOG("Loading background music...");
    auto bgmClip = resources->loadMusic("romfs:/img/BGM.ogg");
    if (bgmClip)
    {
        audio->playMusic(bgmClip, -1); // -1 表示无限循环
        audio->setMusicVolume(0.5f);   // 设置音量为50%
        DEBUG_LOG("✓ Background music loaded and playing!");
    }
    else
    {
        DEBUG_LOG("✗ Warning: Could not load BGM.ogg");
        DEBUG_LOG("  File should be at: romfs:/img/BGM.ogg");
    }

    // 创建摄像机
    GameObject *cameraObj = createGameObject("Camera");
    Camera *camera = cameraObj->addComponent<Camera>();
    camera->backgroundColor = Color(135, 206, 235); // 天蓝色
    DEBUG_LOG("Camera created with sky blue background");

    // 加载森林背景图（横向平铺）
    DEBUG_LOG("Attempting to load forest background...");
    auto bgTexture = resources->loadTexture("romfs:/img/ForestBackground.png");
    if (bgTexture)
    {
        // 计算垂直缩放以填充屏幕高度（ForestBackground.png 是 240x280）
        float scaleY = 720.0f / bgTexture->getHeight(); // 720 / 280 = 2.57
        float scale = scaleY; // 保持宽高比
        
        // 计算缩放后单个背景宽度
        float scaledWidth = bgTexture->getWidth() * scale; // 240 * 2.57 = 617
        
        // 计算需要多少个背景图来覆盖屏幕宽度（1280像素）+ 额外1个用于无缝循环
        int tileCount = (int)(1280.0f / scaledWidth) + 2; // 约3-4个
        
        DEBUG_LOG("✓ Creating tiled background:");
        DEBUG_LOG("  Texture size: %dx%d", bgTexture->getWidth(), bgTexture->getHeight());
        DEBUG_LOG("  Scale: %.2f | Scaled size: %.0fx%.0f", scale, scaledWidth, bgTexture->getHeight() * scale);
        DEBUG_LOG("  Tile count: %d", tileCount);
        
        // 创建多个背景平铺
        for (int i = 0; i < tileCount; i++)
        {
            GameObject *bgTile = createGameObject("BackgroundTile");
            
            // 横向排列：从左边开始，每个平铺向右偏移 scaledWidth
            // 第一个从 scaledWidth/2 开始（左边缘），后续依次向右
            float posX = (scaledWidth / 2.0f) + (i * scaledWidth);
            float posY = 360; // 垂直居中
            
            bgTile->transform->position = {posX, posY};
            bgTile->transform->scale = {scale, scale};
            
            SpriteRenderer *bgRenderer = bgTile->addComponent<SpriteRenderer>();
            bgRenderer->texture = bgTexture;
            bgRenderer->sortingOrder = -100; // 确保背景在最底层
        }
    }
    else
    {
        DEBUG_LOG("✗ Warning: Could not load forest background");
        DEBUG_LOG("  File should be at: romfs:/img/ForestBackground.png");
    }

    // 加载小狐狸精灵图
    DEBUG_LOG("Attempting to load Foxy sprite...");
    auto foxyTexture = resources->loadTexture("romfs:/img/Foxy.png");
    if (foxyTexture)
    {
        fox = createGameObject("Foxy");
        fox->transform->position = {640, 360}; // 屏幕中心
        fox->transform->scale = {2.0f, 2.0f};  // 2倍缩放显示单个精灵帧（33x32）

        SpriteRenderer *foxyRenderer = fox->addComponent<SpriteRenderer>();
        foxyRenderer->texture = foxyTexture;
        foxyRenderer->tint = Color::White(); // 确保使用白色，不改变原图颜色
        foxyRenderer->sortingOrder = 0; // 确保小狐狸显示在背景之上

        // 创建动画控制器
        Animator *animator = fox->addComponent<Animator>();
        animator->spriteRenderer = foxyRenderer;

        // 定义精灵帧尺寸（Foxy.png 是 198x384，分成 6列x12行）
        const int frameWidth = 33;
        const int frameHeight = 32;

        // 创建 idle（站立）动画 - 使用第0行的帧
        Animation idleAnim;
        idleAnim.loop = true;
        for (int i = 0; i < 4; i++)
        {
            idleAnim.addFrame(Rectangle{(float)(i * frameWidth), 0 * frameHeight, frameWidth, frameHeight}, 0.15f);
        }
        animator->addAnimation("idle", idleAnim);

        // 创建 walk（走路）动画 - 使用第1行的帧
        Animation walkAnim;
        walkAnim.loop = true;
        for (int i = 0; i < 6; i++)
        {
            walkAnim.addFrame(Rectangle{(float)(i * frameWidth), 1 * frameHeight, frameWidth, frameHeight}, 0.1f);
        }
        animator->addAnimation("walk", walkAnim);

        // 创建 run（跑步）动画 - 使用第1行的帧
        Animation runAnim;
        runAnim.loop = true;
        for (int i = 0; i < 6; i++)
        {
            runAnim.addFrame(Rectangle{(float)(i * frameWidth), 1 * frameHeight, frameWidth, frameHeight}, 0.08f);
        }
        animator->addAnimation("run", runAnim);

        // 添加小狐狸控制器
        FoxController *foxCtrl = fox->addComponent<FoxController>();
        foxCtrl->moveSpeed = 300.0f;

        DEBUG_LOG("✓ Foxy loaded with animations!");
        DEBUG_LOG("  Texture size: %dx%d", foxyTexture->getWidth(), foxyTexture->getHeight());
        DEBUG_LOG("  Frame size: 33x32 (6x12 grid)");
        DEBUG_LOG("  Animations: idle(4 frames), walk(6 frames), run(4 frames)");
        DEBUG_LOG("  Display size: 66x64 (2x scale)");
    }
    else
    {
        DEBUG_LOG("✗ ERROR: Could not load Foxy sprite!");
        DEBUG_LOG("  File should be at: romfs:/img/Foxy.png");
        DEBUG_LOG("  Creating fallback colored square instead...");

        // 创建一个备用的彩色方块代替小狐狸
        fox = createGameObject("Foxy_Fallback");
        fox->transform->position = {640, 360};
        fox->transform->scale = {100, 100};

        SpriteRenderer *fallbackRenderer = fox->addComponent<SpriteRenderer>();
        fallbackRenderer->tint = Color(255, 140, 0); // 橙色方块

        FoxController *foxCtrl = fox->addComponent<FoxController>();
        foxCtrl->moveSpeed = 300.0f;
    }

    DEBUG_LOG("Fox Scene loaded successfully!");
    DEBUG_LOG("Total game objects: %zu", getGameObjects().size());
}

// 外部场景创建函数声明
extern Scene* createDemoScene();

void FoxScene::onUpdate()
{
    InputManager *input = Engine::getInstance().getInput();

    // B 键返回主菜单
    if (input->getButtonDown(Button::B))
    {
        DEBUG_LOG("Returning to main menu...");
        // 停止背景音乐
        AudioManager *audio = Engine::getInstance().getAudio();
        audio->stopMusic();
        DEBUG_LOG("Background music stopped");
        
        Engine::getInstance().loadScene(std::unique_ptr<Switch2D::Scene>(createDemoScene()));
        return;
    }
}
