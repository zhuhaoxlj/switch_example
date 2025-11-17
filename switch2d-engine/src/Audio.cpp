#include "Switch2D/Audio.h"
#include <cstdio>

namespace Switch2D {

// ============================================
// AudioClip 实现
// ============================================
AudioClip::~AudioClip() {
    free();
}

bool AudioClip::loadFromFile(const std::string& path, Type t) {
    free();
    type = t;
    
    if (type == Type::Sound) {
        chunk = Mix_LoadWAV(path.c_str());
        if (!chunk) {
            printf("Failed to load sound %s: %s\n", path.c_str(), Mix_GetError());
            return false;
        }
    } else {
        music = Mix_LoadMUS(path.c_str());
        if (!music) {
            printf("Failed to load music %s: %s\n", path.c_str(), Mix_GetError());
            return false;
        }
    }
    
    return true;
}

void AudioClip::free() {
    if (chunk) {
        Mix_FreeChunk(chunk);
        chunk = nullptr;
    }
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
}

// ============================================
// AudioManager 实现
// ============================================
AudioManager::AudioManager() {}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::initialize() {
    if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) == 0) {
        printf("Mix_Init failed: %s\n", Mix_GetError());
        return false;
    }
    
    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
        return false;
    }
    
    Mix_AllocateChannels(16); // 分配16个音效通道
    
    initialized = true;
    printf("AudioManager initialized\n");
    return true;
}

void AudioManager::shutdown() {
    if (!initialized) return;
    
    Mix_CloseAudio();
    Mix_Quit();
    initialized = false;
}

void AudioManager::playSound(std::shared_ptr<AudioClip> clip, int loops, float volume) {
    if (!initialized || !clip || clip->getType() != AudioClip::Type::Sound) return;
    
    Mix_Chunk* chunk = clip->getChunk();
    if (!chunk) return;
    
    int channel = Mix_PlayChannel(-1, chunk, loops);
    if (channel >= 0) {
        Mix_Volume(channel, (int)(volume * 128.0f * masterVolume));
    }
}

void AudioManager::stopSound(int channel) {
    if (!initialized) return;
    Mix_HaltChannel(channel);
}

void AudioManager::setSoundVolume(float volume, int channel) {
    if (!initialized) return;
    Mix_Volume(channel, (int)(volume * 128.0f * masterVolume));
}

void AudioManager::playMusic(std::shared_ptr<AudioClip> clip, int loops) {
    if (!initialized || !clip || clip->getType() != AudioClip::Type::Music) return;
    
    Mix_Music* music = clip->getMusic();
    if (!music) return;
    
    Mix_PlayMusic(music, loops);
}

void AudioManager::stopMusic() {
    if (!initialized) return;
    Mix_HaltMusic();
}

void AudioManager::pauseMusic() {
    if (!initialized) return;
    Mix_PauseMusic();
}

void AudioManager::resumeMusic() {
    if (!initialized) return;
    Mix_ResumeMusic();
}

void AudioManager::setMusicVolume(float volume) {
    if (!initialized) return;
    Mix_VolumeMusic((int)(volume * 128.0f * masterVolume));
}

bool AudioManager::isMusicPlaying() const {
    if (!initialized) return false;
    return Mix_PlayingMusic() != 0;
}

void AudioManager::setMasterVolume(float volume) {
    masterVolume = volume;
    if (initialized) {
        Mix_VolumeMusic((int)(masterVolume * 128.0f));
    }
}

} // namespace Switch2D
