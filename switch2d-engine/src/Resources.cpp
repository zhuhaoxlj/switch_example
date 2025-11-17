#include "Switch2D/Resources.h"
#include <cstdio>

namespace Switch2D {

ResourceManager::ResourceManager() {}

ResourceManager::~ResourceManager() {
    clear();
}

void ResourceManager::initialize(SDL_Renderer* r) {
    renderer = r;
}

std::shared_ptr<Texture> ResourceManager::loadTexture(const std::string& path) {
    // 检查是否已加载
    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }
    
    // 创建新纹理
    auto texture = std::make_shared<Texture>();
    if (texture->loadFromFile(path, renderer)) {
        textures[path] = texture;
        printf("Loaded texture: %s\n", path.c_str());
        return texture;
    }
    
    return nullptr;
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::unloadTexture(const std::string& path) {
    textures.erase(path);
}

std::shared_ptr<AudioClip> ResourceManager::loadSound(const std::string& path) {
    // 检查是否已加载
    auto it = audioClips.find(path);
    if (it != audioClips.end()) {
        return it->second;
    }
    
    // 创建新音频
    auto audio = std::make_shared<AudioClip>();
    if (audio->loadFromFile(path, AudioClip::Type::Sound)) {
        audioClips[path] = audio;
        printf("Loaded sound: %s\n", path.c_str());
        return audio;
    }
    
    return nullptr;
}

std::shared_ptr<AudioClip> ResourceManager::loadMusic(const std::string& path) {
    // 检查是否已加载
    auto it = audioClips.find(path);
    if (it != audioClips.end()) {
        return it->second;
    }
    
    // 创建新音频
    auto audio = std::make_shared<AudioClip>();
    if (audio->loadFromFile(path, AudioClip::Type::Music)) {
        audioClips[path] = audio;
        printf("Loaded music: %s\n", path.c_str());
        return audio;
    }
    
    return nullptr;
}

std::shared_ptr<AudioClip> ResourceManager::getAudio(const std::string& path) {
    auto it = audioClips.find(path);
    if (it != audioClips.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::unloadAudio(const std::string& path) {
    audioClips.erase(path);
}

void ResourceManager::clear() {
    textures.clear();
    audioClips.clear();
    printf("All resources cleared\n");
}

} // namespace Switch2D
