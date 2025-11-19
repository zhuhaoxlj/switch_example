#pragma once
#include "Switch2D/Switch2D.h"

using namespace Switch2D;

// ============================================
// 小狐狸控制器组件
// ============================================
class FoxController : public Component
{
public:
    float moveSpeed = 300.0f;
    SpriteRenderer *spriteRenderer = nullptr;
    Animator *animator = nullptr;

    void onStart() override;
    void onUpdate() override;
};

// ============================================
// 小狐狸场景
// ============================================
class FoxScene : public Scene
{
public:
    FoxScene() : Scene("Fox Scene") {}

    void onLoad() override;
    void onUpdate() override;

private:
    GameObject *fox = nullptr;
};
