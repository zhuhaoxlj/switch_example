# Space Impact 2D

一个基于 Switch2D 游戏引擎的横向射击游戏，灵感来自经典的诺基亚游戏 Space Impact。

## 🎮 游戏说明

### 玩法
- 控制你的飞船，击败来袭的敌人
- 收集分数，尽可能存活更长时间
- 避免被敌人或敌人的子弹击中

### 操作方法
- **方向键/左摇杆** - 移动飞船
- **A 或 B 键** - 射击
- **+ 键** - 退出游戏

## 🛠️ 编译

### 前置要求
```bash
# 确保已安装 DevkitPro
# 参考: https://devkitpro.org/wiki/Getting_Started

# 安装必要的库
sudo dkp-pacman -S switch-sdl2 switch-sdl2_image switch-sdl2_mixer
```

### 编译游戏
```bash
cd switch2d-engine/games/SpaceImpact2D
make
```

生成的 `space-impact-2d.nro` 文件可以在 Switch 的 Homebrew Launcher 中运行。

## ✨ 特性

- ✅ 流畅的玩家控制
- ✅ 敌人生成系统
- ✅ 射击系统（玩家和敌人）
- ✅ 碰撞检测
- ✅ 分数和生命系统
- ✅ 滚动背景
- ✅ 无敌时间和闪烁效果

## 🎯 游戏系统

### 组件系统
游戏使用 Switch2D 引擎的组件系统构建：

- **PlayerController** - 玩家飞船控制和射击
- **Enemy** - 敌人行为和射击
- **Bullet** - 子弹移动和碰撞
- **EnemySpawner** - 自动生成敌人
- **BackgroundScroller** - 背景滚动效果
- **GameManager** - 游戏状态管理
- **UIRenderer** - UI 显示

### 扩展建议
可以轻松添加新功能：
- 不同类型的敌人
- 能量道具和武器升级
- 关卡系统
- Boss 战
- 更丰富的视觉效果
- 音效和背景音乐

## 📝 原游戏说明

原 Space Impact 游戏来自 `games/SpaceImpact` 目录，使用 C 语言和传统 SDL 编写。
此版本使用 Switch2D 引擎重构，采用现代 C++ 和组件化设计。

## 📄 许可证

本项目基于原 Space Impact 的许可证条款，仅用于教育目的。

---

**祝你玩得开心！** 🚀✨
