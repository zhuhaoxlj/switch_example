# RomFS 资源文件夹

这个文件夹用于存放游戏资源文件。

## 文件夹结构

```
romfs/
├── sprites/     # 精灵图片
├── sounds/      # 音效文件
└── music/       # 背景音乐
```

## 使用方法

在游戏代码中，可以通过 `romfs:/` 前缀访问这些资源：

```cpp
// 加载纹理
auto texture = resources->loadTexture("romfs:/sprites/player.png");

// 加载音效
auto sound = resources->loadSound("romfs:/sounds/shoot.wav");

// 加载音乐
auto music = resources->loadMusic("romfs:/music/bgm.ogg");
```

## 资源迁移

如果需要从原 SpaceImpact 游戏迁移资源：
1. 从 `games/SpaceImpact/data/` 复制相关图片和音频文件
2. 根据需要转换格式（PNG, WAV, OGG 等）
3. 放置到对应的文件夹中
