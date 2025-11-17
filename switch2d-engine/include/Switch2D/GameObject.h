#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "Math.h"

namespace Switch2D {

// 前向声明
class Component;
class Transform;
class Scene;

// 简单的类型ID系统（不需要RTTI）
using ComponentTypeID = size_t;

namespace Internal {
    inline ComponentTypeID getNextComponentTypeID() {
        static ComponentTypeID nextID = 0;
        return nextID++;
    }
    
    template<typename T>
    ComponentTypeID getComponentTypeID() {
        static ComponentTypeID typeID = getNextComponentTypeID();
        return typeID;
    }
}

// ============================================
// GameObject - 游戏对象
// ============================================
class GameObject {
public:
    GameObject(const std::string& name = "GameObject");
    virtual ~GameObject() = default;
    
    // 组件管理
    template<typename T, typename... Args>
    T* addComponent(Args&&... args);
    
    template<typename T>
    T* getComponent();
    
    template<typename T>
    bool hasComponent();
    
    template<typename T>
    void removeComponent();
    
    // 属性
    std::string name;
    bool active = true;
    std::string tag;
    
    Transform* transform = nullptr;
    Scene* scene = nullptr;
    
    // 生命周期
    void start();
    void update();
    void render();
    void destroy();
    
private:
    std::vector<std::unique_ptr<Component>> components;
    std::unordered_map<ComponentTypeID, Component*> componentMap;
    bool started = false;
};

// ============================================
// Component - 组件基类
// ============================================
class Component {
public:
    virtual ~Component() = default;
    
    GameObject* gameObject = nullptr;
    Transform* transform = nullptr;
    bool enabled = true;
    
    virtual void onStart() {}
    virtual void onUpdate() {}
    virtual void onRender() {}
    virtual void onDestroy() {}
    
protected:
    friend class GameObject;
};

// ============================================
// Transform - 变换组件
// ============================================
class Transform : public Component {
public:
    Vector2 position{0, 0};
    float rotation = 0; // 角度
    Vector2 scale{1, 1};
    
    Transform* parent = nullptr;
    std::vector<Transform*> children;
    
    Vector2 getWorldPosition() const;
    float getWorldRotation() const;
    Vector2 getWorldScale() const;
    
    void setParent(Transform* newParent);
    void addChild(Transform* child);
    void removeChild(Transform* child);
    
    Vector2 forward() const;
    Vector2 right() const;
};

// 模板实现
template<typename T, typename... Args>
T* GameObject::addComponent(Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
    
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = component.get();
    
    ptr->gameObject = this;
    ptr->transform = transform;
    
    componentMap[Internal::getComponentTypeID<T>()] = ptr;
    components.push_back(std::move(component));
    
    if (started) {
        ptr->onStart();
    }
    
    return ptr;
}

template<typename T>
T* GameObject::getComponent() {
    auto it = componentMap.find(Internal::getComponentTypeID<T>());
    if (it != componentMap.end()) {
        return static_cast<T*>(it->second);
    }
    return nullptr;
}

template<typename T>
bool GameObject::hasComponent() {
    return componentMap.find(Internal::getComponentTypeID<T>()) != componentMap.end();
}

template<typename T>
void GameObject::removeComponent() {
    auto it = componentMap.find(Internal::getComponentTypeID<T>());
    if (it != componentMap.end()) {
        Component* comp = it->second;
        comp->onDestroy();
        
        componentMap.erase(it);
        components.erase(
            std::remove_if(components.begin(), components.end(),
                [comp](const std::unique_ptr<Component>& c) {
                    return c.get() == comp;
                }),
            components.end()
        );
    }
}

} // namespace Switch2D
