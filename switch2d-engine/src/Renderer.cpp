#include "Switch2D/Renderer.h"
#include "Switch2D/Core.h"
#include <cmath>

namespace Switch2D {

// ============================================
// Texture 实现
// ============================================
Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(const std::string& path, SDL_Renderer* renderer) {
    free();
    
    SDL_Surface* surface = nullptr;
    
#ifdef HAS_SDL_IMAGE
    surface = IMG_Load(path.c_str());
    if (!surface) {
        printf("Failed to load texture %s: %s\n", path.c_str(), IMG_GetError());
        return false;
    }
#else
    // 如果没有SDL_image，只支持BMP格式
    surface = SDL_LoadBMP(path.c_str());
    if (!surface) {
        printf("Failed to load texture %s: %s\n", path.c_str(), SDL_GetError());
        printf("Note: SDL_image not available, only BMP format is supported\n");
        return false;
    }
#endif
    
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Failed to create texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }
    
    width = surface->w;
    height = surface->h;
    
    SDL_FreeSurface(surface);
    return true;
}

void Texture::free() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        width = 0;
        height = 0;
    }
}

// ============================================
// SpriteRenderer 实现
// ============================================
void SpriteRenderer::setTexture(std::shared_ptr<Texture> tex) {
    texture = tex;
    if (texture && sourceRect.width == 0) {
        sourceRect.width = texture->getWidth();
        sourceRect.height = texture->getHeight();
    }
}

Rectangle SpriteRenderer::getDestRect() const {
    if (!texture) return {0, 0, 0, 0};
    
    Vector2 worldPos = transform->getWorldPosition();
    Vector2 worldScale = transform->getWorldScale();
    
    float width = (sourceRect.width > 0 ? sourceRect.width : texture->getWidth()) * worldScale.x;
    float height = (sourceRect.height > 0 ? sourceRect.height : texture->getHeight()) * worldScale.y;
    
    return {
        worldPos.x - width * pivot.x,
        worldPos.y - height * pivot.y,
        width,
        height
    };
}

void SpriteRenderer::onRender() {
    SDL_Renderer* renderer = Engine::getInstance().getRenderer();
    if (!renderer) return;
    
    // 如果没有纹理，渲染纯色矩形
    if (!texture || !texture->getSDLTexture()) {
        Vector2 worldPos = transform->getWorldPosition();
        Vector2 worldScale = transform->getWorldScale();
        
        SDL_Rect rect = {
            (int)(worldPos.x - worldScale.x * pivot.x),
            (int)(worldPos.y - worldScale.y * pivot.y),
            (int)worldScale.x,
            (int)worldScale.y
        };
        
        SDL_SetRenderDrawColor(renderer, tint.r, tint.g, tint.b, tint.a);
        SDL_RenderFillRect(renderer, &rect);
        return;
    }
    
    Rectangle dest = getDestRect();
    
    SDL_Rect srcRect;
    if (sourceRect.width > 0) {
        srcRect = {(int)sourceRect.x, (int)sourceRect.y, 
                   (int)sourceRect.width, (int)sourceRect.height};
    } else {
        srcRect = {0, 0, texture->getWidth(), texture->getHeight()};
    }
    
    SDL_Rect destRect = {(int)dest.x, (int)dest.y, (int)dest.width, (int)dest.height};
    
    // 设置颜色调制
    SDL_SetTextureColorMod(texture->getSDLTexture(), tint.r, tint.g, tint.b);
    SDL_SetTextureAlphaMod(texture->getSDLTexture(), tint.a);
    
    // 渲染
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (flipX) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
    if (flipY) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
    
    double angle = transform->getWorldRotation();
    SDL_Point center = {(int)(dest.width * pivot.x), (int)(dest.height * pivot.y)};
    
    SDL_RenderCopyEx(renderer, texture->getSDLTexture(), &srcRect, &destRect, 
                     angle, &center, flip);
}

// ============================================
// Camera 实现
// ============================================
Camera* Camera::main = nullptr;

Camera::Camera() {
    if (!main) {
        main = this;
    }
}

void Camera::onStart() {
    if (!main) {
        main = this;
    }
}

Vector2 Camera::worldToScreen(const Vector2& worldPos) const {
    Vector2 camPos = transform->getWorldPosition();
    Vector2 offset = worldPos - camPos;
    offset.x *= zoom;
    offset.y *= zoom;
    
    Engine& engine = Engine::getInstance();
    return {
        offset.x + engine.getScreenWidth() / 2.0f,
        offset.y + engine.getScreenHeight() / 2.0f
    };
}

Vector2 Camera::screenToWorld(const Vector2& screenPos) const {
    Engine& engine = Engine::getInstance();
    Vector2 camPos = transform->getWorldPosition();
    
    Vector2 offset = {
        screenPos.x - engine.getScreenWidth() / 2.0f,
        screenPos.y - engine.getScreenHeight() / 2.0f
    };
    
    offset.x /= zoom;
    offset.y /= zoom;
    
    return camPos + offset;
}

void Camera::applyTransform(SDL_Renderer* renderer) const {
    // 在 SDL2 中，我们通过调整渲染坐标来模拟摄像机
    // 实际项目中可能需要更复杂的变换矩阵
}

// ============================================
// Animator 实现
// ============================================
void Animator::addAnimation(const std::string& name, const Animation& anim) {
    animations[name] = anim;
}

void Animator::play(const std::string& name) {
    if (animations.find(name) == animations.end()) return;
    
    currentAnimName = name;
    currentFrame = 0;
    frameTimer = 0;
    playing = true;
    paused = false;
}

void Animator::stop() {
    playing = false;
    paused = false;
    currentFrame = 0;
    frameTimer = 0;
}

void Animator::pause() {
    paused = true;
}

void Animator::resume() {
    paused = false;
}

void Animator::onUpdate() {
    if (!playing || paused || animations.find(currentAnimName) == animations.end()) {
        return;
    }
    
    Animation& anim = animations[currentAnimName];
    if (anim.frames.empty()) return;
    
    frameTimer += Time::deltaTime;
    
    if (frameTimer >= anim.frames[currentFrame].duration) {
        frameTimer = 0;
        currentFrame++;
        
        if (currentFrame >= (int)anim.frames.size()) {
            if (anim.loop) {
                currentFrame = 0;
            } else {
                currentFrame = anim.frames.size() - 1;
                playing = false;
            }
        }
    }
    
    // 更新精灵渲染器
    if (spriteRenderer) {
        spriteRenderer->sourceRect = anim.frames[currentFrame].sourceRect;
    }
}

// ============================================
// TextRenderer 实现
// ============================================
void TextRenderer::onRender() {
    // TODO: 实现 SDL_ttf 文本渲染
    // 需要链接 SDL2_ttf 库
}

} // namespace Switch2D
