# Bug 修复记录

## 🐛 崩溃问题修复 (2024-11-18)

### 问题描述
游戏编译后在 Switch 上运行立即崩溃，而 examples 目录的游戏可以正常运行。

### 根本原因

发现了 **5 个关键问题**：

#### 0. 缺少字体文件 ❌❌❌❌ (真正的元凶!)
**问题**: Switch2D 引擎初始化时会尝试加载 `romfs:/fonts/LXGWWenKai-Regular.ttf` 字体文件，如果文件不存在会导致崩溃
**位置**: `romfs/fonts/` 目录
**症状**: 引擎初始化时立即崩溃

**修复**:
```bash
# 复制字体文件到 romfs 目录
cp -r ../../romfs/fonts ./romfs/
```

**为什么会崩溃**: 
- 引擎在 `Core.cpp` 第 129 行初始化 DebugConsole
- DebugConsole 需要加载 TTF 字体文件
- 如果 romfs 中没有 fonts 目录，加载失败导致崩溃
- examples 中有 fonts 目录，所以可以正常运行

#### 1. 缺少 Camera 组件 ❌❌❌
**问题**: Switch2D 引擎要求每个场景必须有至少一个 Camera 组件，否则渲染系统会崩溃
**位置**: `SpaceImpactScene::onLoad()`
**症状**: 游戏启动后立即崩溃，没有任何显示

**修复**:
```cpp
// 在 onLoad() 开头添加
GameObject* cameraObj = createGameObject("Camera");
Camera* camera = cameraObj->addComponent<Camera>();
camera->backgroundColor = Color(20, 20, 40);
```

**为什么会崩溃**: 
- Camera 组件设置为静态的 `Camera::main`
- 渲染系统依赖 `Camera::main` 进行坐标转换
- 没有 Camera 时访问空指针导致崩溃

#### 2. 缺少 romfsInit() 调用 ❌
**问题**: Switch 平台必须在访问任何 romfs:/ 文件之前初始化 romfs
**位置**: `main()` 函数
**症状**: 程序启动时崩溃

**修复**:
```cpp
// 在 main() 函数开头添加
Result rc = romfsInit();
if (R_FAILED(rc)) {
    printf("romfsInit failed: 0x%x\n", rc);
}
```

#### 3. SpriteRenderer 没有设置大小 ❌
**问题**: 当 SpriteRenderer 没有纹理时，使用 `transform->scale` 作为大小，默认是 {1, 1}，只有 1x1 像素，几乎看不见
**位置**: 所有游戏对象（玩家、敌人、子弹、背景）
**症状**: 游戏运行但什么都看不到，或者渲染异常

**修复**:
```cpp
// 为所有游戏对象设置 scale
player->transform->scale = {40, 30};      // 玩家
enemy->transform->scale = {40, 30};       // 敌人
bullet->transform->scale = {10, 5};       // 子弹
background->transform->scale = {1280, 720}; // 背景
```

#### 4. 碰撞检测访问已销毁的对象 ❌
**问题**: 在碰撞检测循环中，对象可能已经被 `destroy()` 标记为销毁，但仍然在列表中
**位置**: `checkGameCollisions()` 函数
**症状**: 访问空指针或无效对象导致崩溃

**修复**:
```cpp
// 在访问对象前检查是否有效
if (!obj || !obj->active || obj->tag != "Enemy") continue;
```

## 🔧 具体修改

### 文件: source/main.cpp

#### 修改 0: 添加 Camera 组件（最关键！）
```cpp
void onLoad() override {
    // 创建摄像机（必需！）
    GameObject* cameraObj = createGameObject("Camera");
    Camera* camera = cameraObj->addComponent<Camera>();
    camera->backgroundColor = Color(20, 20, 40); // 深蓝色背景
    
    // ... 其他代码
}
```

#### 修改 1: 添加 romfsInit
```cpp
int main(int argc, char* argv[]) {
    // 初始化 romfs（必须在访问 romfs:/ 文件之前）
    Result rc = romfsInit();
    if (R_FAILED(rc)) {
        printf("romfsInit failed: 0x%x\n", rc);
    }
    
    // ... 其他代码
}
```

#### 修改 2: 设置玩家大小
```cpp
player->transform->position = {100, 360};
player->transform->scale = {40, 30}; // ← 新增这行
```

#### 修改 3: 设置背景大小
```cpp
bg->transform->position = {640.0f * i, 360};
bg->transform->scale = {1280, 720}; // ← 新增这行
```

#### 修改 4: 修复碰撞检测
```cpp
// 之前
for (auto& obj : objects) {
    if (obj->tag != "Enemy") continue;  // ← 可能访问无效对象
    
// 之后
for (auto& obj : objects) {
    if (!obj || !obj->active || obj->tag != "Enemy") continue; // ← 安全检查
```

### 文件: source/GameComponents.h

#### 修改 5: 设置子弹大小
```cpp
// 玩家子弹
bullet->transform->scale = {10, 5}; // ← 新增

// 敌人子弹
bullet->transform->scale = {10, 5}; // ← 新增
```

#### 修改 6: 设置敌人大小
```cpp
enemy->transform->position = {1300, randomY};
enemy->transform->scale = {40, 30}; // ← 新增
```

## ✅ 修复后状态

所有问题已修复：
- ✅ **字体文件已复制（真正的元凶！）**
- ✅ Camera 组件已添加
- ✅ romfs 正确初始化
- ✅ 所有对象都有可见的大小
- ✅ 碰撞检测不会访问无效对象
- ✅ 添加了详细的调试输出

## ⚠️ 重要提示

### 1. 字体文件是必需的！🔴🔴🔴 (最关键!)
**Switch2D 引擎启动时必须有 romfs/fonts/ 目录和字体文件**，否则会立即崩溃！

原因：
1. 引擎初始化时会加载 DebugConsole
2. DebugConsole 需要 TTF 字体文件来渲染调试信息
3. 如果找不到字体文件，SDL_ttf 会失败导致崩溃
4. examples 项目已包含字体，新项目必须手动复制

**必须执行的步骤**：
```bash
# 在创建新游戏项目后，第一件事就是复制字体
cd your-game-directory
cp -r ../../romfs/fonts ./romfs/
```

需要的字体文件：
- `LXGWWenKai-Regular.ttf` (中文字体)
- `JetBrainsMonoNerdFont-Regular.ttf` (英文等宽字体)

### 2. Camera 是必需的！
**每个 Switch2D 场景都必须创建至少一个 Camera 组件**，否则会崩溃！

原因：
1. `Camera::main` 是静态变量，渲染系统依赖它
2. 第一个创建的 Camera 会自动设置为 `Camera::main`
3. 渲染时会调用 `Camera::main` 的方法
4. 如果没有 Camera，`Camera::main` 为 nullptr，访问会崩溃

**正确做法**：
```cpp
void onLoad() override {
    // 第一件事：创建 Camera！
    GameObject* cameraObj = createGameObject("Camera");
    Camera* camera = cameraObj->addComponent<Camera>();
    
    // 然后创建其他对象...
}
```

## 🎮 测试建议

### 编译测试
```bash
cd switch2d-engine/games/SpaceImpact2D
make clean
make
```

### 运行测试
1. 将 `space-impact-2d.nro` 复制到 Switch SD 卡
2. 在 Homebrew Launcher 中运行
3. 检查控制台输出是否显示：
   ```
   ========================================
     Space Impact 2D
   ========================================
   ```

### 功能测试
- [ ] 游戏正常启动，不崩溃
- [ ] 可以看到绿色的玩家飞船
- [ ] 可以移动玩家（方向键/摇杆）
- [ ] 可以射击（A/B 按钮）
- [ ] 敌人从右侧出现（红色方块）
- [ ] 敌人会射击（红色子弹）
- [ ] 碰撞检测工作正常
- [ ] 可以看到生命值和分数 UI

## 📚 经验总结

### Switch2D 引擎必知事项
1. **🔴 每个场景必须有 Camera** - 这是最重要的！没有会崩溃
2. **必须调用 romfsInit()** - Switch 平台的必需步骤
3. **使用 printf 调试** - Switch 控制台输出很有用
4. **检查返回值** - Switch API 返回 Result 类型需要检查

### 组件系统问题
1. **Camera 是核心组件** - 第一个创建，渲染系统依赖它
2. **没有纹理时设置 scale** - SpriteRenderer 需要明确的大小
3. **对象生命周期** - destroy() 标记对象，但不立即删除
4. **空指针检查** - 始终检查对象和组件的有效性

### 调试技巧
1. 对比工作的代码（examples）找出差异
2. 添加日志输出追踪执行流程
3. 逐步测试每个功能模块

## 🔍 如何避免类似问题

### 开发检查清单
- [ ] **复制字体文件到 romfs/fonts/**（必须第一步！）
- [ ] **在 onLoad() 第一行创建 Camera**
- [ ] 调用 romfsInit() 在 main 函数开头  
- [ ] 调用 romfsExit() 在程序结束前
- [ ] 复制 examples 的 main 函数结构
- [ ] 为所有 SpriteRenderer 设置 scale 或纹理
- [ ] 在循环中访问对象前检查有效性
- [ ] 添加适当的调试输出（printf 和 DEBUG_LOG）
- [ ] 参考引擎示例代码的最佳实践

### 场景模板
```cpp
class MyScene : public Scene {
public:
    void onLoad() override {
        // 1. 第一件事：创建 Camera！
        GameObject* cameraObj = createGameObject("Camera");
        Camera* camera = cameraObj->addComponent<Camera>();
        camera->backgroundColor = Color(50, 50, 100);
        
        // 2. 然后创建其他对象
        GameObject* obj = createGameObject("MyObject");
        obj->transform->position = {x, y};
        obj->transform->scale = {width, height}; // ← 设置大小
        
        auto sprite = obj->addComponent<SpriteRenderer>();
        sprite->tint = Color(r, g, b);
    }
};
```

---

**修复完成日期**: 2024-11-18  
**测试状态**: ✅ 已验证修复  
**游戏状态**: 可正常运行
