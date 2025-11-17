#pragma once
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include "GameObject.h"

namespace Switch2D {

// ============================================
// Scene - 场景
// ============================================
class Scene {
public:
    Scene(const std::string& name = "Scene");
    virtual ~Scene() = default;
    
    // 场景生命周期
    virtual void onLoad() {}
    virtual void onUnload() {}
    virtual void onUpdate() {}
    virtual void onRender() {}
    
    // 游戏对象管理
    GameObject* createGameObject(const std::string& name = "GameObject");
    void destroyGameObject(GameObject* obj);
    
    GameObject* findGameObject(const std::string& name);
    std::vector<GameObject*> findGameObjectsWithTag(const std::string& tag);
    
    const std::vector<std::unique_ptr<GameObject>>& getGameObjects() const {
        return gameObjects;
    }
    
    std::string name;
    
    // 内部使用
    void start();
    void update();
    void render();
    void cleanup();
    
private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::vector<GameObject*> objectsToDestroy;
    bool isStarted = false;
};

} // namespace Switch2D
