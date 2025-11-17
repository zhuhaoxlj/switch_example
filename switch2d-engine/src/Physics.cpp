#include "Switch2D/Physics.h"
#include "Switch2D/Core.h"
#include "Switch2D/Scene.h"

namespace Switch2D {

// 默认重力
Vector2 Rigidbody::gravity = {0, 980.0f}; // 980 像素/秒²

// ============================================
// BoxCollider 实现
// ============================================
bool BoxCollider::checkCollision(Collider* other) {
    if (other->colliderType == ColliderType::Box) {
        return Physics::checkCollision(this, static_cast<BoxCollider*>(other));
    } else if (other->colliderType == ColliderType::Circle) {
        return Physics::checkCollision(this, static_cast<CircleCollider*>(other));
    }
    return false;
}

Rectangle BoxCollider::getBounds() const {
    Vector2 pos = transform->getWorldPosition() + offset;
    Vector2 scale = transform->getWorldScale();
    return {
        pos.x - size.x * scale.x * 0.5f,
        pos.y - size.y * scale.y * 0.5f,
        size.x * scale.x,
        size.y * scale.y
    };
}

// ============================================
// CircleCollider 实现
// ============================================
bool CircleCollider::checkCollision(Collider* other) {
    if (other->colliderType == ColliderType::Circle) {
        return Physics::checkCollision(this, static_cast<CircleCollider*>(other));
    } else if (other->colliderType == ColliderType::Box) {
        return Physics::checkCollision(static_cast<BoxCollider*>(other), this);
    }
    return false;
}

Rectangle CircleCollider::getBounds() const {
    Vector2 pos = transform->getWorldPosition() + offset;
    float r = radius * std::max(transform->getWorldScale().x, transform->getWorldScale().y);
    return {pos.x - r, pos.y - r, r * 2, r * 2};
}

// ============================================
// Rigidbody 实现
// ============================================
void Rigidbody::addForce(const Vector2& force) {
    if (!isKinematic && mass > 0) {
        acceleration += force / mass;
    }
}

void Rigidbody::addImpulse(const Vector2& impulse) {
    if (!isKinematic && mass > 0) {
        velocity += impulse / mass;
    }
}

void Rigidbody::onUpdate() {
    if (isKinematic) return;
    
    // 应用重力
    if (useGravity) {
        acceleration += gravity * gravityScale;
    }
    
    // 更新速度
    velocity += acceleration * Time::deltaTime;
    
    // 应用阻力
    if (drag > 0) {
        float dragFactor = 1.0f - drag * Time::deltaTime;
        if (dragFactor < 0) dragFactor = 0;
        velocity *= dragFactor;
    }
    
    // 更新位置
    transform->position += velocity * Time::deltaTime;
    
    // 重置加速度
    acceleration = Vector2::Zero();
}

// ============================================
// Physics 实现
// ============================================
void Physics::checkCollisions(Scene* scene) {
    if (!scene) return;
    
    // 获取所有碰撞体
    std::vector<Collider*> colliders;
    for (auto& obj : scene->getGameObjects()) {
        if (!obj->active) continue;
        
        if (auto collider = obj->getComponent<Collider>()) {
            if (collider->enabled) {
                colliders.push_back(collider);
            }
        }
    }
    
    // 检测碰撞
    for (size_t i = 0; i < colliders.size(); i++) {
        for (size_t j = i + 1; j < colliders.size(); j++) {
            if (colliders[i]->checkCollision(colliders[j])) {
                // TODO: 触发碰撞事件
            }
        }
    }
}

bool Physics::checkCollision(BoxCollider* box1, BoxCollider* box2) {
    Rectangle r1 = box1->getBounds();
    Rectangle r2 = box2->getBounds();
    return r1.intersects(r2);
}

bool Physics::checkCollision(CircleCollider* circle1, CircleCollider* circle2) {
    Vector2 pos1 = circle1->transform->getWorldPosition() + circle1->offset;
    Vector2 pos2 = circle2->transform->getWorldPosition() + circle2->offset;
    
    float r1 = circle1->radius * std::max(circle1->transform->getWorldScale().x, 
                                          circle1->transform->getWorldScale().y);
    float r2 = circle2->radius * std::max(circle2->transform->getWorldScale().x, 
                                          circle2->transform->getWorldScale().y);
    
    float distSq = (pos2 - pos1).lengthSquared();
    float radiusSum = r1 + r2;
    
    return distSq <= radiusSum * radiusSum;
}

bool Physics::checkCollision(BoxCollider* box, CircleCollider* circle) {
    Rectangle rect = box->getBounds();
    Vector2 circlePos = circle->transform->getWorldPosition() + circle->offset;
    float radius = circle->radius * std::max(circle->transform->getWorldScale().x,
                                             circle->transform->getWorldScale().y);
    
    // 找到矩形上最接近圆心的点
    Vector2 closestPoint;
    closestPoint.x = Math::Clamp(circlePos.x, rect.left(), rect.right());
    closestPoint.y = Math::Clamp(circlePos.y, rect.top(), rect.bottom());
    
    // 检查距离
    float distSq = (circlePos - closestPoint).lengthSquared();
    return distSq <= radius * radius;
}

} // namespace Switch2D
