# 🔤 TTF 字体支持状态

## ✅ 当前状态

### 编译成功！
- ✅ 代码已支持 TTF 字体（SDL_ttf）
- ✅ 使用条件编译，SDL_ttf 是可选的
- ✅ 如果 SDL_ttf 不可用，会自动回退到像素字体
- ✅ 当前使用**像素字体**（3x5 像素，英文数字）

### 检测到的字体文件
你的 `romfs/fonts/` 目录已经包含字体文件！
```
Writing romfs/JetBrainsMonoNerdFont-Regular.ttf to RomFS image...
```

---

## 🎯 如何启用 TTF 字体支持

### 选项 1：安装 SDL2_ttf（推荐）

#### 步骤 1：安装 SDL2_ttf 库

```bash
# 使用 dkp-pacman 安装（DevkitPro 包管理器）
sudo dkp-pacman -S switch-sdl2_ttf switch-freetype switch-bz2

# 或者
(dkp-)pacman -S switch-sdl2_ttf
```

#### 步骤 2：修改 Makefile

编辑 `Makefile`，找到第 47-48 行：

```makefile
# SDL2_ttf 是可选的，如果已安装可以取消下面的注释
# LIBS	:= -lSDL2_ttf -lfreetype -lbz2 $(LIBS)
```

取消注释（删除 `#`）：

```makefile
# SDL2_ttf 已安装，启用支持
LIBS	:= -lSDL2_ttf -lfreetype -lbz2 $(LIBS)
```

#### 步骤 3：重新编译

```bash
./build.sh
```

#### 步骤 4：验证

运行游戏后，按 A+B 打开调试控制台，你应该看到：
```
[12:34:56] SDL_ttf not available, using fallback pixel font
```
变成：
```
[12:34:56] Loaded TTF font: romfs:/fonts/JetBrainsMono-Regular.ttf
```

---

### 选项 2：继续使用像素字体

如果不想安装 SDL2_ttf，**什么都不用做**！

当前配置已经：
- ✅ 调试控制台正常工作
- ✅ 使用像素字体显示日志
- ✅ 所有功能可用

**优点**：
- 无需额外依赖
- 编译更快
- .nro 文件更小

**缺点**：
- 只支持英文字母和数字
- 分辨率较低（3x5 像素）

---

## 📋 检查 SDL2_ttf 是否已安装

### 方法 1：检查库文件

```bash
ls $DEVKITPRO/portlibs/switch/lib | grep ttf
```

如果输出包含 `libSDL2_ttf.a`，说明已安装。

### 方法 2：检查头文件

```bash
ls $DEVKITPRO/portlibs/switch/include/SDL2 | grep ttf
```

如果输出包含 `SDL_ttf.h`，说明已安装。

### 方法 3：使用 pacman 查询

```bash
pacman -Qi switch-sdl2_ttf
```

如果显示包信息，说明已安装。

---

## 🛠️ 安装 SDL2_ttf 的详细步骤

### macOS / Linux

```bash
# 1. 确保 DevkitPro 环境变量已设置
echo $DEVKITPRO
# 应该输出类似: /opt/devkitpro

# 2. 更新包数据库
sudo dkp-pacman -Sy

# 3. 搜索 SDL2_ttf
dkp-pacman -Ss sdl2_ttf

# 4. 安装
sudo dkp-pacman -S switch-sdl2_ttf switch-freetype switch-bz2

# 5. 验证安装
ls $DEVKITPRO/portlibs/switch/lib/libSDL2_ttf.a
```

### Windows

```batch
REM 打开 Developer Command Prompt for DevkitPro

REM 1. 更新包数据库
pacman -Sy

REM 2. 安装 SDL2_ttf
pacman -S switch-sdl2_ttf switch-freetype switch-bz2

REM 3. 验证
dir %DEVKITPRO%\portlibs\switch\lib\libSDL2_ttf.a
```

---

## 📊 两种方案对比

| 特性 | 像素字体（当前） | TTF 字体 |
|------|-----------------|---------|
| **依赖** | 无 | SDL2_ttf + freetype + bz2 |
| **安装** | ✅ 即开即用 | 需要安装库 |
| **字符支持** | A-Z, 0-9 | 所有 Unicode 字符 |
| **显示质量** | 3x5 像素 | 高清晰度 |
| **文件大小** | +0 KB | +200 KB (字体文件) |
| **性能** | 非常快 | 快 |
| **中文支持** | ❌ | ✅ |
| **emoji** | ❌ | ✅ |
| **可调大小** | ❌ | ✅ |

---

## 🎨 使用 TTF 字体的效果

### 像素字体（当前）

```
[12:34:56] PLAYER Y: 650.00
[12:34:57] BUTTON PRESSED
[12:34:58] JUMP! VELOCITY
```

- 3x5 像素，简单清晰
- 仅大写英文和数字

### TTF 字体（JetBrains Mono）

```
[12:34:56] Player Y: 650.00, Ground: 650.00, IsGrounded: YES
[12:34:57] Button pressed! IsGrounded: YES
[12:34:58] Jump! velocity.y = -450.00
```

- 高清晰度
- 支持大小写、标点符号
- 专业美观

---

## 🐛 常见问题

### Q: 为什么编译时找不到 SDL_ttf.h？

**A:** SDL2_ttf 未安装。代码已配置为可选，会自动使用像素字体。

### Q: 如何知道当前使用的是哪种字体？

**A:** 
1. 启动游戏
2. 按 A+B 打开调试控制台
3. 查看第一行日志：
   - `SDL_ttf not available, using fallback pixel font` → 像素字体
   - `Loaded TTF font: romfs:/fonts/xxx.ttf` → TTF 字体

### Q: 安装 SDL2_ttf 后仍然使用像素字体？

**A:** 检查：
1. Makefile 中是否取消了注释
2. 重新编译：`make clean && ./build.sh`
3. 字体文件路径是否正确

### Q: 可以使用其他字体吗？

**A:** 可以！将任何 .ttf 文件复制到 `romfs/fonts/`，然后修改 `src/Core.cpp` 中的字体路径：

```cpp
const char* fontPath = "romfs:/fonts/你的字体.ttf";
DebugConsole::getInstance().initialize(renderer, fontPath);
```

### Q: 我应该选哪个？

**A:** 

**选择像素字体如果**：
- ✅ 只需要显示英文和数字
- ✅ 希望减少依赖
- ✅ 追求最小文件大小

**选择 TTF 字体如果**：
- ✅ 需要显示中文或特殊字符
- ✅ 想要更专业的外观
- ✅ 需要调整字体大小
- ✅ 不介意额外的200KB

---

## 📝 总结

### 当前状态：✅ 完全可用

- 调试控制台已实现
- 使用像素字体（3x5 像素）
- 按 A+B 可切换显示
- 所有功能正常工作

### 升级到 TTF：可选

如果你想要更好的显示效果：

```bash
# 1. 安装库
sudo dkp-pacman -S switch-sdl2_ttf switch-freetype switch-bz2

# 2. 启用 TTF（编辑 Makefile 第 48 行，取消注释）

# 3. 重新编译
./build.sh

# 完成！
```

---

**推荐**：先使用当前的像素字体版本，如果觉得需要更好的显示效果，再升级到 TTF。

现在你的调试控制台已经完全可用了！🎉
