#pragma once
#include <SDL.h>
#include <string>
#include <memory>
#include "GameObject.h"
#include "Math.h"

// SDL_image 是可选的，如果未安装则使用SDL内置的BMP加载
#ifdef __has_include
#  if __has_include(<SDL_image.h>)
#    include <SDL_image.h>
#    define HAS_SDL_IMAGE 1
#  endif
#endif

namespace Switch2D {

// ============================================
// Texture - 纹理
// ============================================
class Texture {
public:
    Texture() = default;
    ~Texture();
    
    bool loadFromFile(const std::string& path, SDL_Renderer* renderer);
    void free();
    
    SDL_Texture* getSDLTexture() const { return texture; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
private:
    SDL_Texture* texture = nullptr;
    int width = 0;
    int height = 0;
};

// ============================================
// SpriteRenderer - 精灵渲染器
// ============================================
class SpriteRenderer : public Component {
public:
    std::shared_ptr<Texture> texture;
    Rectangle sourceRect{0, 0, 0, 0}; // 源纹理区域，0表示使用完整纹理
    Color tint = Color::White();
    Vector2 pivot{0.5f, 0.5f}; // 锚点 (0,0)左上角 (1,1)右下角
    int sortingOrder = 0; // 渲染顺序，越大越后渲染
    bool flipX = false;
    bool flipY = false;
    
    void setTexture(std::shared_ptr<Texture> tex);
    
    void onRender() override;
    
private:
    Rectangle getDestRect() const;
};

// ============================================
// Camera - 摄像机
// ============================================
class Camera : public Component {
public:
    static Camera* main;
    
    Color backgroundColor = Color(100, 149, 237); // 天蓝色
    float zoom = 1.0f;
    
    Camera();
    
    Vector2 worldToScreen(const Vector2& worldPos) const;
    Vector2 screenToWorld(const Vector2& screenPos) const;
    
    void onStart() override;
    void applyTransform(SDL_Renderer* renderer) const;
};

// ============================================
// Animation - 动画
// ============================================
struct AnimationFrame {
    Rectangle sourceRect;
    float duration; // 秒
};

class Animation {
public:
    std::vector<AnimationFrame> frames;
    bool loop = true;
    
    void addFrame(const Rectangle& rect, float duration) {
        frames.push_back({rect, duration});
    }
};

// ============================================
// Animator - 动画控制器
// ============================================
class Animator : public Component {
public:
    void addAnimation(const std::string& name, const Animation& anim);
    void play(const std::string& name);
    void stop();
    void pause();
    void resume();
    
    bool isPlaying() const { return playing && !paused; }
    std::string getCurrentAnimation() const { return currentAnimName; }
    
    void onUpdate() override;
    
    SpriteRenderer* spriteRenderer = nullptr;
    
private:
    std::unordered_map<std::string, Animation> animations;
    std::string currentAnimName;
    int currentFrame = 0;
    float frameTimer = 0;
    bool playing = false;
    bool paused = false;
};

// ============================================
// TextRenderer - 文本渲染器
// ============================================
class TextRenderer : public Component {
public:
    std::string text;
    Color color = Color::White();
    int fontSize = 24;
    std::string fontPath = "romfs:/fonts/default.ttf";
    
    void onRender() override;
    
private:
    // SDL_ttf 相关实现
};

} // namespace Switch2D
