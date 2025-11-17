#include "Switch2D/GameObject.h"
#include <algorithm>

namespace Switch2D {

// ============================================
// GameObject 实现
// ============================================
GameObject::GameObject(const std::string& n) : name(n) {
    transform = addComponent<Transform>();
}

void GameObject::start() {
    if (started) return;
    
    for (auto& comp : components) {
        if (comp->enabled) {
            comp->onStart();
        }
    }
    
    started = true;
}

void GameObject::update() {
    if (!active) return;
    
    for (auto& comp : components) {
        if (comp->enabled) {
            comp->onUpdate();
        }
    }
}

void GameObject::render() {
    if (!active) return;
    
    for (auto& comp : components) {
        if (comp->enabled) {
            comp->onRender();
        }
    }
}

void GameObject::destroy() {
    for (auto& comp : components) {
        comp->onDestroy();
    }
    components.clear();
    componentMap.clear();
}

// ============================================
// Transform 实现
// ============================================
Vector2 Transform::getWorldPosition() const {
    if (parent) {
        return parent->getWorldPosition() + position;
    }
    return position;
}

float Transform::getWorldRotation() const {
    if (parent) {
        return parent->getWorldRotation() + rotation;
    }
    return rotation;
}

Vector2 Transform::getWorldScale() const {
    if (parent) {
        Vector2 parentScale = parent->getWorldScale();
        return {scale.x * parentScale.x, scale.y * parentScale.y};
    }
    return scale;
}

void Transform::setParent(Transform* newParent) {
    if (parent) {
        parent->removeChild(this);
    }
    
    parent = newParent;
    
    if (parent) {
        parent->addChild(this);
    }
}

void Transform::addChild(Transform* child) {
    if (child && std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
    }
}

void Transform::removeChild(Transform* child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
    }
}

Vector2 Transform::forward() const {
    float rad = rotation * Math::DEG2RAD;
    return Vector2(std::cos(rad), std::sin(rad));
}

Vector2 Transform::right() const {
    float rad = (rotation + 90) * Math::DEG2RAD;
    return Vector2(std::cos(rad), std::sin(rad));
}

} // namespace Switch2D
