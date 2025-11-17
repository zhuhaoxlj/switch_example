#pragma once
#include <switch.h>
#include <SDL.h>
#include <memory>
#include <vector>
#include <string>
#include "Math.h"

namespace Switch2D {

// 前向声明
class Scene;
class InputManager;
class AudioManager;
class ResourceManager;

// ============================================
// Time - 时间管理
// ============================================
class Time {
public:
    static float deltaTime;
    static float timeScale;
    static uint64_t frameCount;
    
    static void update(uint64_t currentTicks);
    
private:
    static uint64_t lastTicks;
};

// ============================================
// Engine - 引擎核心
// ============================================
class Engine {
public:
    struct Config {
        std::string title = "Switch2D Game";
        int screenWidth = 1280;
        int screenHeight = 720;
        bool fullscreen = true;
        bool vsync = true;
        int targetFPS = 60;
    };
    
    static Engine& getInstance();
    
    bool initialize(const Config& config);
    void run();
    void shutdown();
    
    void loadScene(std::unique_ptr<Scene> scene);
    Scene* getCurrentScene() const { return currentScene.get(); }
    
    SDL_Renderer* getRenderer() const { return renderer; }
    InputManager* getInput() const { return inputManager.get(); }
    AudioManager* getAudio() const { return audioManager.get(); }
    ResourceManager* getResources() const { return resourceManager.get(); }
    
    int getScreenWidth() const { return config.screenWidth; }
    int getScreenHeight() const { return config.screenHeight; }
    
    bool isRunning() const { return running; }
    void quit() { running = false; }
    
private:
    Engine() = default;
    ~Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    
    void processEvents();
    void update();
    void render();
    
    Config config;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    std::unique_ptr<Scene> currentScene;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<AudioManager> audioManager;
    std::unique_ptr<ResourceManager> resourceManager;
    
    bool running = false;
};

} // namespace Switch2D
