#include "Switch2D/Core.h"
#include "Switch2D/Scene.h"
#include "Switch2D/Input.h"
#include "Switch2D/Audio.h"
#include "Switch2D/Resources.h"
#include "Switch2D/Network.h"
#include "Switch2D/DebugConsole.h"
#include <cstdio>

namespace Switch2D {

// ============================================
// Time 实现
// ============================================
float Time::deltaTime = 0.0f;
float Time::timeScale = 1.0f;
uint64_t Time::frameCount = 0;
uint64_t Time::lastTicks = 0;

void Time::update(uint64_t currentTicks) {
    if (lastTicks == 0) {
        lastTicks = currentTicks;
        deltaTime = 0.0f;
    } else {
        uint64_t deltaTicks = currentTicks - lastTicks;
        deltaTime = (deltaTicks / 1000.0f) * timeScale; // SDL_GetTicks 返回毫秒
        lastTicks = currentTicks;
    }
    frameCount++;
}

// ============================================
// Engine 实现
// ============================================
Engine& Engine::getInstance() {
    static Engine instance;
    return instance;
}

bool Engine::initialize(const Config& cfg) {
    config = cfg;
    
    printf("\n========================================\n");
    printf("  Switch2D Engine v1.0.0\n");
    printf("========================================\n");
    printf("Initializing...\n");
    
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    
    // 初始化 SDL_image (如果可用)
#ifdef HAS_SDL_IMAGE
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("Warning: IMG_Init failed: %s\n", IMG_GetError());
        printf("Continuing without SDL_image support\n");
    }
#else
    printf("SDL_image not available, only BMP images will be supported\n");
#endif
    
    // 创建窗口
    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (config.fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }
    
    window = SDL_CreateWindow(
        config.title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.screenWidth,
        config.screenHeight,
        windowFlags
    );
    
    if (!window) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }
    
    // 创建渲染器
    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    if (config.vsync) {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }
    
    renderer = SDL_CreateRenderer(window, -1, rendererFlags);
    if (!renderer) {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }
    
    // 设置渲染缩放质量
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    
    // 初始化手柄
    for (int i = 0; i < 2; i++) {
        SDL_JoystickOpen(i);
    }
    
    // 创建子系统
    inputManager = std::make_unique<InputManager>();
    audioManager = std::make_unique<AudioManager>();
    resourceManager = std::make_unique<ResourceManager>();
    networkManager = std::make_unique<NetworkManager>();
    
    if (!audioManager->initialize()) {
        printf("Warning: AudioManager initialization failed\n");
    }
    
    resourceManager->initialize(renderer);
    
    // 初始化网络（可选，失败不影响游戏运行）
    if (!networkManager->initialize()) {
        printf("Warning: NetworkManager initialization failed\n");
    }
    
    // 初始化调试控制台（使用 JetBrains Mono Nerd Font）
    // 尝试多个可能的 romfs 路径格式
    const char* fontPaths[] = {
        "romfs:/fonts/LXGWWenKai-Regular.ttf",
    };
    
    const char* fontPath = fontPaths[0];
    DebugConsole::getInstance().initialize(renderer, fontPath);
    DebugConsole::getInstance().log("Switch2D Engine initialized with TTF font!");
    DebugConsole::getInstance().logf("Font: JetBrains Mono Nerd Font");
    DebugConsole::getInstance().logf("Resolution: %dx%d", config.screenWidth, config.screenHeight);
    
    printf("Switch2D Engine initialized successfully!\n");
    printf("Resolution: %dx%d\n", config.screenWidth, config.screenHeight);
    
    return true;
}

void Engine::run() {
    if (!window || !renderer) {
        printf("Engine not initialized!\n");
        return;
    }
    
    running = true;
    
    if (currentScene) {
        currentScene->start();
    }
    
    printf("Starting main loop...\n");
    
    while (running && appletMainLoop()) {
        uint64_t currentTicks = SDL_GetTicks();
        Time::update(currentTicks);
        
        processEvents();
        update();
        render();
        
        // 帧率限制
        if (config.targetFPS > 0 && !config.vsync) {
            uint32_t frameTime = SDL_GetTicks() - currentTicks;
            uint32_t targetFrameTime = 1000 / config.targetFPS;
            if (frameTime < targetFrameTime) {
                SDL_Delay(targetFrameTime - frameTime);
            }
        }
    }
    
    printf("Main loop ended\n");
}

void Engine::shutdown() {
    printf("Shutting down Switch2D Engine...\n");
    
    currentScene.reset();
    resourceManager.reset();
    networkManager.reset();
    audioManager.reset();
    inputManager.reset();
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
#ifdef HAS_SDL_IMAGE
    IMG_Quit();
#endif
    SDL_Quit();
    
    printf("Switch2D Engine shut down\n");
}

void Engine::loadScene(std::unique_ptr<Scene> scene) {
    if (currentScene) {
        currentScene->onUnload();
    }
    
    currentScene = std::move(scene);
    
    if (currentScene && running) {
        currentScene->onLoad();
        currentScene->start();
    }
}

void Engine::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
    
    if (inputManager) {
        inputManager->update();
    }
}

void Engine::update() {
    if (currentScene) {
        currentScene->update();
        currentScene->onUpdate();
    }
}

void Engine::render() {
    if (!renderer) return;
    
    // 清屏
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // 渲染场景
    if (currentScene) {
        currentScene->render();
        currentScene->onRender();
    }
    
    // 渲染调试控制台（最后渲染，显示在最上层）
    DebugConsole::getInstance().render();
    
    // 呈现
    SDL_RenderPresent(renderer);
}

} // namespace Switch2D
