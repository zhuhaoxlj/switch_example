# 音频调试指南

## 问题原因

在 Nintendo Switch 上使用 SDL2 访问 romfs 文件系统中的音频文件时，必须先调用 `romfsInit()` 初始化 romfs 文件系统。

## 已修复的问题

### 1. 添加 romfs 初始化

```cpp
// 在 init() 函数开始处
Result rc = romfsInit();
if (R_FAILED(rc)) {
    SDL_Log("romfsInit failed: 0x%x\n", rc);
}
```

### 2. 添加 romfs 清理

```cpp
// 在 cleanup() 函数结束前
romfsExit();
```

### 3. 优化音频初始化顺序

- 先初始化 MP3 支持 (`Mix_Init`)
- 再打开音频设备 (`Mix_OpenAudio`)
- 最后加载音频文件

### 4. 添加详细日志

现在会输出以下调试信息：
- romfs 初始化状态
- 音频系统初始化状态
- 每个音频文件的加载状态
- BGM 播放状态

## 如何查看日志

### 方法 1: 使用 nxlink

1. 在电脑上运行：
```bash
nxlink -s snake.nro
```

2. 在 Switch 上通过 nxlink 启动游戏

3. 查看电脑终端的输出日志

### 方法 2: 使用模拟器

如果使用 Yuzu 或 Ryujinx 模拟器，日志会输出到模拟器的控制台窗口。

## 预期日志输出

如果一切正常，你应该看到：

```
Audio initialized successfully
Eat sound loaded successfully
Game over sound loaded successfully
BGM loaded successfully
BGM playing...
```

## 常见问题排查

### 问题 1: romfsInit 失败

**症状**：看到 "romfsInit failed: 0x..."

**原因**：
- romfs 目录为空
- 编译时没有正确打包 romfs

**解决方法**：
```bash
make clean
make
```
确保编译输出中看到：
```
Writing /path/to/snake/romfs/bgm.mp3 to RomFS image...
Writing /path/to/snake/romfs/eat.wav to RomFS image...
Writing /path/to/snake/romfs/gameover.wav to RomFS image...
```

### 问题 2: 音频文件加载失败

**症状**：看到 "Failed to load ... sound"

**可能原因**：
1. 音频文件格式不支持
2. 文件损坏
3. romfs 未正确初始化

**解决方法**：
1. 确认音频文件格式：
   - WAV: PCM 格式，44100Hz，16-bit
   - MP3: 标准 MP3 格式

2. 测试文件是否有效（在电脑上播放）

3. 重新编译游戏

### 问题 3: Mix_OpenAudio 失败

**症状**：看到 "Mix_OpenAudio failed"

**可能原因**：
- SDL2_mixer 库未正确安装
- 音频设备不可用

**解决方法**：
```bash
# 确保已安装 SDL2_mixer
sudo dkp-pacman -S switch-sdl2_mixer
```

### 问题 4: BGM 不播放但加载成功

**症状**：看到 "BGM loaded successfully" 但听不到声音

**可能原因**：
1. 音量设置为 0
2. Switch 系统音量太低
3. MP3 文件编码问题

**解决方法**：
1. 检查 Switch 系统音量
2. 尝试不同的 MP3 文件
3. 转换 MP3 为标准格式：
```bash
ffmpeg -i input.mp3 -ar 44100 -ac 2 -b:a 192k output.mp3
```

## 音频文件要求

### WAV 文件
- 格式：PCM
- 采样率：44100 Hz
- 位深度：16-bit
- 声道：单声道或立体声
- 文件大小：< 5MB（推荐）

### MP3 文件
- 格式：MPEG Audio Layer 3
- 比特率：128-320 kbps
- 采样率：44100 Hz
- 文件大小：< 10MB（推荐）

## 转换音频文件

### 转换 WAV

```bash
# 转换为正确格式
ffmpeg -i input.wav -ar 44100 -ac 2 -sample_fmt s16 output.wav

# 或者使用 Audacity
# 1. 打开文件
# 2. 选择 "文件" -> "导出" -> "导出为 WAV"
# 3. 设置：PCM 16-bit, 44100Hz
```

### 转换 MP3

```bash
# 转换为正确格式
ffmpeg -i input.mp3 -ar 44100 -ac 2 -b:a 192k output.mp3

# 降低文件大小
ffmpeg -i input.mp3 -ar 44100 -ac 2 -b:a 128k output.mp3
```

## 音量调整

当前音量设置：
- **BGM**: 50% (`MIX_MAX_VOLUME / 2`)
- **音效**: 100% (默认)

如需调整，修改代码：

```cpp
// BGM 音量 (0-128)
Mix_VolumeMusic(64);  // 50%
Mix_VolumeMusic(128); // 100%
Mix_VolumeMusic(32);  // 25%

// 音效音量 (0-128)
Mix_VolumeChunk(eatSound, 64); // 50%
```

## 测试步骤

1. **编译游戏**
```bash
cd /path/to/snake
make clean
make
```

2. **检查编译输出**
确保看到 romfs 文件被打包：
```
Writing .../romfs/bgm.mp3 to RomFS image...
Writing .../romfs/eat.wav to RomFS image...
Writing .../romfs/gameover.wav to RomFS image...
```

3. **运行游戏**
通过 nxlink 或直接在 Switch 上运行

4. **测试音频**
- 启动游戏 → BGM 应该自动播放
- 吃食物 → 播放 eat.wav
- 游戏结束 → BGM 停止，播放 gameover.wav
- 暂停游戏 → BGM 暂停
- 恢复游戏 → BGM 继续

## 技术支持

如果问题仍然存在：
1. 收集完整的日志输出
2. 检查音频文件是否正确
3. 确认 Switch 系统音量不是静音
4. 尝试使用示例音频文件测试

## 参考

- SDL2_mixer 文档: https://www.libsdl.org/projects/SDL_mixer/
- libnx romfs 文档: https://switchbrew.github.io/libnx/
