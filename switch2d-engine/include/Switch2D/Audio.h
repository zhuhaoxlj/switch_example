#pragma once
#include <SDL_mixer.h>
#include <string>
#include <memory>
#include <unordered_map>

namespace Switch2D {

// ============================================
// AudioClip - 音频片段
// ============================================
class AudioClip {
public:
    enum class Type {
        Sound,  // 短音效 (WAV)
        Music   // 背景音乐 (OGG/MP3)
    };
    
    AudioClip() = default;
    ~AudioClip();
    
    bool loadFromFile(const std::string& path, Type type);
    void free();
    
    Type getType() const { return type; }
    Mix_Chunk* getChunk() const { return chunk; }
    Mix_Music* getMusic() const { return music; }
    
private:
    Type type;
    Mix_Chunk* chunk = nullptr;
    Mix_Music* music = nullptr;
};

// ============================================
// AudioManager - 音频管理器
// ============================================
class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    
    bool initialize();
    void shutdown();
    
    // 音效播放
    void playSound(std::shared_ptr<AudioClip> clip, int loops = 0, float volume = 1.0f);
    void stopSound(int channel = -1);
    void setSoundVolume(float volume, int channel = -1);
    
    // 音乐播放
    void playMusic(std::shared_ptr<AudioClip> clip, int loops = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(float volume);
    
    bool isMusicPlaying() const;
    
    // 主音量控制
    void setMasterVolume(float volume);
    float getMasterVolume() const { return masterVolume; }
    
private:
    bool initialized = false;
    float masterVolume = 1.0f;
};

} // namespace Switch2D
