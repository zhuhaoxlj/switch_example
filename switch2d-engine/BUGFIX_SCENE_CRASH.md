# 🐛 场景切换崩溃修复

## 问题描述

点击绿色"Gyro Ball"按钮时程序崩溃。

## 崩溃原因

### 问题分析

```
InvalidMemoryRegionException: va=0x00000053454F6C69
```

这是一个**野指针访问**错误。崩溃的根本原因：

1. **点击按钮触发 `onButtonClick()`**
   ```cpp
   if (input->getTouchDown()) {
       onButtonClick();  // 按钮被点击
   }
   ```

2. **`onButtonClick()` 立即切换场景**
   ```cpp
   void loadGyroScene() {
       Engine::getInstance().loadScene(std::make_unique<GyroScene>());
       // ⚠️ 场景立即切换，当前场景和所有对象被销毁！
   }
   ```

3. **但代码还在 `TouchButton::onUpdate()` 中**
   ```cpp
   void onUpdate() override {
       if (input->getTouchDown()) {
           onButtonClick();  // 这里切换了场景
       }
       
       // ❌ 继续执行，但 gameObject 已经被销毁了！
       SpriteRenderer* renderer = gameObject->getComponent<SpriteRenderer>();
       // 💥 崩溃！访问已销毁的对象
   }
   ```

### 时序图

```
时间线：
1. TouchButton::onUpdate() 开始
   ↓
2. 检测到触摸，调用 onButtonClick()
   ↓
3. onButtonClick() 调用 loadScene()
   ↓
4. 当前 DemoScene 被销毁 ← 所有 GameObject 都没了！
   ↓
5. GyroScene 被加载
   ↓
6. ❌ 回到 TouchButton::onUpdate() 继续执行
   ↓
7. 访问 gameObject->getComponent() 
   ↓
8. 💥 崩溃！gameObject 已经是野指针
```

---

## 解决方案

### 延迟场景切换

不要在组件的 `onUpdate()` 执行过程中立即切换场景，而是**设置标记**，在下一帧的开始处切换。

### 实现步骤

#### 1. 添加静态标记变量

```cpp
class TouchButton : public Component {
public:
    // ... 其他成员 ...
    
    static bool sceneChangeRequested;  // 场景切换请求标记
    static int requestedScene;          // 请求的场景ID
};
```

#### 2. 初始化静态变量

```cpp
// 在类外部初始化
bool TouchButton::sceneChangeRequested = false;
int TouchButton::requestedScene = 0;
```

#### 3. 修改按钮点击函数

**之前（立即切换）：**
```cpp
void loadGyroScene() {
    Engine::getInstance().loadScene(std::make_unique<GyroScene>());
    // ❌ 立即切换，导致崩溃
}
```

**之后（延迟切换）：**
```cpp
void loadGyroScene() {
    DEBUG_LOG("Loading Gyro Ball Scene...");
    DEBUG_LOG("Get ready to tilt your device!");
    
    // ✅ 只设置标记，不立即切换
    sceneChangeRequested = true;
    requestedScene = 2;  // 2 = GyroScene
}
```

#### 4. 在场景的 onUpdate 开头检查

```cpp
void DemoScene::onUpdate() {
    // ✅ 在所有逻辑之前检查场景切换请求
    if (TouchButton::sceneChangeRequested) {
        TouchButton::sceneChangeRequested = false;
        
        if (TouchButton::requestedScene == 2) {
            Engine::getInstance().loadScene(std::make_unique<GyroScene>());
            return;  // 立即返回，不再执行后续代码
        }
    }
    
    // 安全：场景切换在这一帧的开始，没有组件还在执行
    // ... 其他更新逻辑 ...
}
```

---

## 修复后的流程

```
时间线（修复后）：

第 N 帧：
1. TouchButton::onUpdate() 开始
   ↓
2. 检测到触摸，调用 onButtonClick()
   ↓
3. onButtonClick() 设置 sceneChangeRequested = true
   ↓
4. TouchButton::onUpdate() 继续执行 ✅ 安全
   ↓
5. 访问 gameObject->getComponent() ✅ 对象还在
   ↓
6. TouchButton::onUpdate() 结束
   ↓
7. 第 N 帧结束

第 N+1 帧：
1. DemoScene::onUpdate() 开始
   ↓
2. 检查 sceneChangeRequested == true
   ↓
3. 切换到 GyroScene ✅ 在帧的最开始
   ↓
4. 立即 return，不执行后续代码 ✅ 安全
```

---

## 技术要点

### 1. 为什么立即切换会崩溃？

```cpp
void onUpdate() {
    doSomething();
    switchScene();  // ❌ 这里销毁了当前对象
    // 继续执行，但对象已经没了
    accessMember();  // 💥 野指针！
}
```

### 2. 为什么延迟切换是安全的？

```cpp
// 第 N 帧
void onUpdate() {
    doSomething();
    requestSceneChange();  // ✅ 只设置标记
    // 继续执行，对象还在
    accessMember();  // ✅ 安全
}  // onUpdate 结束

// 第 N+1 帧开始
void scene::onUpdate() {
    if (sceneChangeRequested) {
        switchScene();  // ✅ 在新帧开始时切换
        return;  // 立即返回
    }
}
```

### 3. 为什么要立即 return？

```cpp
if (TouchButton::sceneChangeRequested) {
    Engine::getInstance().loadScene(...);
    return;  // ✅ 必须！否则会继续执行，访问新场景的数据
}

// 如果不 return，会继续执行：
input->getButtonDown(...);  // 可能崩溃
Physics::checkCollisions(this);  // 访问已销毁的对象
```

---

## 类似问题的通用解决方案

### 原则

**永远不要在对象的成员函数执行过程中销毁该对象！**

### 方法

1. **延迟销毁** - 设置标记，稍后处理
2. **回调队列** - 将操作加入队列，在安全时机执行
3. **引用计数** - 确保对象在使用期间不被销毁
4. **智能指针** - 自动管理生命周期

### 在游戏引擎中的应用

```cpp
// ❌ 危险：立即删除
void GameObject::destroy() {
    delete this;  // 💥 如果在成员函数中调用，立即崩溃
}

// ✅ 安全：标记删除
void GameObject::destroy() {
    markForDeletion = true;  // 设置标记
}

// 在帧的开始或结束统一清理
void Scene::cleanupDestroyedObjects() {
    for (auto& obj : objects) {
        if (obj->markForDeletion) {
            delete obj;  // ✅ 安全时机
        }
    }
}
```

---

## 调试技巧

### 1. 如何发现这类问题？

**症状**：
- 程序突然崩溃
- 日志显示 `InvalidMemoryRegionException`
- 崩溃地址看起来像乱码（如 `0x00000053454F6C69`）

**诊断**：
```cpp
// 添加调试日志
void onUpdate() {
    DEBUG_LOG("onUpdate start");
    
    if (condition) {
        DEBUG_LOG("Before scene change");
        changeScene();
        DEBUG_LOG("After scene change");  // 💥 可能执行不到这里
    }
    
    DEBUG_LOG("onUpdate end");  // 💥 可能执行不到这里
}
```

### 2. 使用断点

```cpp
void loadGyroScene() {
    // 在这里设置断点
    Engine::getInstance().loadScene(...);
    // 场景切换后，查看调用栈
    // 如果还在 TouchButton::onUpdate 中 → 有问题！
}
```

### 3. 检查调用栈

```
崩溃时的调用栈：
GameObject::getComponent()  ← 崩溃点
TouchButton::onUpdate()     ← 还在这个函数里！
Scene::update()
Engine::run()
main()
```

---

## 测试验证

### 修复前

```
操作：点击绿色按钮
结果：💥 崩溃
日志：InvalidMemoryRegionException
```

### 修复后

```
操作：点击绿色按钮
结果：✅ 正常切换到陀螺仪场景
日志：
  Loading Gyro Ball Scene...
  Get ready to tilt your device!
  === Gyro Ball Scene ===
  Tilt your device to move the ball!
```

---

## 总结

### 问题根源
在组件的 `onUpdate()` 中立即切换场景，导致对象在成员函数执行期间被销毁。

### 解决方案
延迟场景切换，使用标记系统，在下一帧的安全时机执行切换。

### 关键代码
1. 添加静态标记 `sceneChangeRequested` 和 `requestedScene`
2. 按钮点击时设置标记，不立即切换
3. 在场景的 `onUpdate()` 开头检查并执行切换
4. 切换后立即 `return`，不执行后续代码

### 适用场景
- 任何可能在对象生命周期内销毁该对象的操作
- 场景切换、对象删除、资源卸载等

---

**修复完成！现在可以安全地点击绿色按钮进入陀螺仪场景了！** ✅🎮

---

*Switch2D Engine - Bug Fix Documentation*
