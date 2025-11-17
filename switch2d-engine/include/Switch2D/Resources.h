#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "Renderer.h"
#include "Audio.h"

namespace Switch2D {

// ============================================
// ResourceManager - 资源管理器
// ============================================
class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();
    
    void initialize(SDL_Renderer* renderer);
    
    // 纹理管理
    std::shared_ptr<Texture> loadTexture(const std::string& path);
    std::shared_ptr<Texture> getTexture(const std::string& path);
    void unloadTexture(const std::string& path);
    
    // 音频管理
    std::shared_ptr<AudioClip> loadSound(const std::string& path);
    std::shared_ptr<AudioClip> loadMusic(const std::string& path);
    std::shared_ptr<AudioClip> getAudio(const std::string& path);
    void unloadAudio(const std::string& path);
    
    // 清理所有资源
    void clear();
    
private:
    SDL_Renderer* renderer = nullptr;
    
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
    std::unordered_map<std::string, std::shared_ptr<AudioClip>> audioClips;
};

} // namespace Switch2D
