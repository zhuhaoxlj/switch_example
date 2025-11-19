#include "Switch2D/Renderer.h"
#include "Switch2D/Core.h"
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Switch2D {

// ============================================
// Texture 实现
// ============================================
Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(const std::string& path, SDL_Renderer* renderer) {
    free();

    // 使用 SDL_RWops 读取文件数据（支持 romfs:/ 路径）
    SDL_RWops* rwops = SDL_RWFromFile(path.c_str(), "rb");
    if (!rwops) {
        printf("Failed to open file %s: %s\n", path.c_str(), SDL_GetError());
        return false;
    }

    // 读取文件大小
    Sint64 fileSize = SDL_RWsize(rwops);
    if (fileSize < 0) {
        printf("Failed to get file size for %s\n", path.c_str());
        SDL_RWclose(rwops);
        return false;
    }

    // 读取文件数据到内存
    unsigned char* fileData = new unsigned char[fileSize];
    if (SDL_RWread(rwops, fileData, fileSize, 1) != 1) {
        printf("Failed to read file %s\n", path.c_str());
        delete[] fileData;
        SDL_RWclose(rwops);
        return false;
    }
    SDL_RWclose(rwops);

    // 使用 stb_image 解码图片
    int width, height, channels;
    unsigned char* imageData = stbi_load_from_memory(fileData, fileSize, &width, &height, &channels, STBI_rgb_alpha);
    delete[] fileData;

    if (!imageData) {
        printf("Failed to decode image %s: %s\n", path.c_str(), stbi_failure_reason());
        return false;
    }

    // 创建 SDL_Surface
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        imageData,
        width, height,
        32, // bits per pixel (RGBA = 32)
        width * 4, // pitch (bytes per row)
        0x000000FF, // R mask
        0x0000FF00, // G mask
        0x00FF0000, // B mask
        0xFF000000  // A mask
    );

    if (!surface) {
        printf("Failed to create surface from image data: %s\n", SDL_GetError());
        stbi_image_free(imageData);
        return false;
    }

    // 创建纹理
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Failed to create texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        stbi_image_free(imageData);
        return false;
    }

    this->width = width;
    this->height = height;

    SDL_FreeSurface(surface);
    stbi_image_free(imageData);

    printf("Successfully loaded texture: %s (%dx%d, %d channels)\n", path.c_str(), width, height, channels);
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
