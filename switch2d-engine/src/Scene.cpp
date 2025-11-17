#include "Switch2D/Scene.h"

namespace Switch2D {

Scene::Scene(const std::string& n) : name(n) {}

void Scene::start() {
    if (isStarted) return;
    
    onLoad();
    
    for (auto& obj : gameObjects) {
        obj->start();
    }
    
    isStarted = true;
}

void Scene::update() {
    for (auto& obj : gameObjects) {
        if (obj->active) {
            obj->update();
        }
    }
    
    // 清理待销毁对象
    cleanup();
}

void Scene::render() {
    for (auto& obj : gameObjects) {
        if (obj->active) {
            obj->render();
        }
    }
}

void Scene::cleanup() {
    if (objectsToDestroy.empty()) return;
    
    for (GameObject* obj : objectsToDestroy) {
        obj->destroy();
        
        gameObjects.erase(
            std::remove_if(gameObjects.begin(), gameObjects.end(),
                [obj](const std::unique_ptr<GameObject>& ptr) {
                    return ptr.get() == obj;
                }),
            gameObjects.end()
        );
    }
    
    objectsToDestroy.clear();
}

GameObject* Scene::createGameObject(const std::string& name) {
    auto obj = std::make_unique<GameObject>(name);
    obj->scene = this;
    
    GameObject* ptr = obj.get();
    gameObjects.push_back(std::move(obj));
    
    if (isStarted) {
        ptr->start();
    }
    
    return ptr;
}

void Scene::destroyGameObject(GameObject* obj) {
    if (obj) {
        objectsToDestroy.push_back(obj);
    }
}

GameObject* Scene::findGameObject(const std::string& name) {
    for (auto& obj : gameObjects) {
        if (obj->name == name) {
            return obj.get();
        }
    }
    return nullptr;
}

std::vector<GameObject*> Scene::findGameObjectsWithTag(const std::string& tag) {
    std::vector<GameObject*> result;
    for (auto& obj : gameObjects) {
        if (obj->tag == tag) {
            result.push_back(obj.get());
        }
    }
    return result;
}

} // namespace Switch2D
