#pragma once
#include "GameObject.h"
#include "Math.h"
#include <vector>

namespace Switch2D {

// ============================================
// Collider - 碰撞体基类
// ============================================
enum class ColliderType {
    Box,
    Circle
};

class Collider : public Component {
public:
    bool isTrigger = false;
    Vector2 offset{0, 0};
    ColliderType colliderType;
    
    virtual bool checkCollision(Collider* other) = 0;
    virtual Rectangle getBounds() const = 0;
    
protected:
    Collider(ColliderType type) : colliderType(type) {}
};

// ============================================
// BoxCollider - 盒碰撞体
// ============================================
class BoxCollider : public Collider {
public:
    BoxCollider() : Collider(ColliderType::Box) {}
    Vector2 size{1, 1};
    
    bool checkCollision(Collider* other) override;
    Rectangle getBounds() const override;
};

// ============================================
// CircleCollider - 圆形碰撞体
// ============================================
class CircleCollider : public Collider {
public:
    CircleCollider() : Collider(ColliderType::Circle) {}
    float radius = 0.5f;
    
    bool checkCollision(Collider* other) override;
    Rectangle getBounds() const override;
};

// ============================================
// Rigidbody - 刚体
// ============================================
class Rigidbody : public Component {
public:
    Vector2 velocity{0, 0};
    Vector2 acceleration{0, 0};
    float mass = 1.0f;
    float drag = 0.0f;
    float gravityScale = 1.0f;
    bool useGravity = true;
    bool isKinematic = false;
    
    void addForce(const Vector2& force);
    void addImpulse(const Vector2& impulse);
    
    void onUpdate() override;
    
    static Vector2 gravity;
};

// ============================================
// Physics - 物理系统
// ============================================
class Physics {
public:
    static void checkCollisions(Scene* scene);
    
    static bool checkCollision(BoxCollider* box1, BoxCollider* box2);
    static bool checkCollision(CircleCollider* circle1, CircleCollider* circle2);
    static bool checkCollision(BoxCollider* box, CircleCollider* circle);
};

} // namespace Switch2D
