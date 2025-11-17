# 👆 触控按钮功能说明

Switch2D Engine - 触摸屏测试功能

---

## ✨ 新增功能

已添加 **4 个可点击的触控按钮** 和 **触摸指示器**！

---

## 🎮 功能演示

### 屏幕布局

```
┌─────────────────────────────────────┐
│                                     │
│         游戏区域                     │
│    (跳跃方块、平台等)                │
│                                     │
│                                     │
├─────────────────────────────────────┤
│  [Touch Test] [Network] [Color] [Info]  │  ← 触控按钮
└─────────────────────────────────────┘
```

### 4 个触控按钮

| 按钮 | 颜色 | 位置 | 功能 |
|------|------|------|------|
| **Touch Test** | 🔵 蓝色 | 左侧 | 测试触摸输入 |
| **Network** | 🟢 绿色 | 中左 | 快速网络测试 |
| **Color** | 🟠 橙色 | 中右 | 随机改变按钮颜色 |
| **Info** | 🟣 紫色 | 右侧 | 显示系统信息 |

---

## 📱 如何使用

### 1. 运行游戏

在 Ryujinx 中加载 `switch2d-demo.nro`

### 2. 打开调试控制台

按 **A + B** 键打开调试控制台（查看反馈信息）

### 3. 触摸按钮

- **鼠标点击**屏幕底部的彩色按钮
- 按钮会在按下时变暗
- 控制台会显示详细信息

### 4. 观察反馈

- 红色圆点跟随触摸位置
- 控制台显示触摸坐标
- 按钮点击后执行相应操作

---

## 🔧 按钮功能详解

### 1. Touch Test（蓝色）

**功能**: 测试触摸输入是否正常工作

**点击后显示**:
```
=== Button 1 Clicked! ===
Label: Touch Test
Position: (110, 620)
Action: Test Button 1
Touch input is working!
```

---

### 2. Network（绿色）

**功能**: 快速测试网络连接

**点击后显示**:
```
=== Button 2 Clicked! ===
Label: Network
Position: (360, 620)
Action: Network Test
Fetching example.com...
Success! Size: 1256 bytes
```

---

### 3. Color（橙色）

**功能**: 随机改变按钮颜色

**点击后显示**:
```
=== Button 3 Clicked! ===
Label: Color
Position: (610, 620)
Action: Change Color
New color: RGB(234,156,89)
```

按钮会变成新的随机颜色！

---

### 4. Info（紫色）

**功能**: 显示系统信息

**点击后显示**:
```
=== Button 4 Clicked! ===
Label: Info
Position: (860, 620)
Action: Show Info
=== System Info ===
Screen: 1280x720
FPS: 60.0
Frame: 3542
Objects: 23
```

---

## 🎯 触摸指示器

### 红色圆点

- **作用**: 实时显示触摸位置
- **颜色**: 红色（半透明）
- **大小**: 20x20 像素
- **行为**: 
  - 触摸时出现
  - 跟随触摸移动
  - 松开后消失

### 触摸坐标显示

在调试控制台中实时显示触摸坐标：

```
Touch at: (456, 623)
Touch at: (457, 624)
Touch at: (458, 625)
```

---

## 💻 技术实现

### TouchButton 组件

```cpp
class TouchButton : public Component {
public:
    Rectangle bounds;           // 按钮边界
    Color normalColor;          // 正常颜色
    Color pressedColor;         // 按下颜色
    std::string label;          // 按钮标签
    int buttonId;              // 按钮ID
    bool isPressed;            // 是否按下
    
    void onUpdate() override {
        // 检查触摸碰撞
        if (input->isTouching()) {
            Vector2 touchPos = input->getTouchPosition();
            if (pointInBounds(touchPos, bounds)) {
                isPressed = true;
                if (input->getTouchDown()) {
                    onButtonClick();
                }
            }
        }
        
        // 更新颜色
        updateColor();
    }
    
    void onButtonClick() {
        // 执行按钮操作
        switch (buttonId) {
            case 1: testTouch(); break;
            case 2: testNetwork(); break;
            case 3: changeColor(); break;
            case 4: showInfo(); break;
        }
    }
};
```

### 创建按钮

```cpp
// 在 DemoScene::onLoad() 中
ButtonConfig buttons[] = {
    {"Touch Test", 200, 650, Color(80, 120, 200), 1},
    {"Network", 450, 650, Color(120, 180, 80), 2},
    {"Color", 700, 650, Color(200, 120, 80), 3},
    {"Info", 950, 650, Color(180, 80, 200), 4}
};

for (int i = 0; i < 4; i++) {
    GameObject* button = createGameObject(buttons[i].label);
    button->transform->position = {buttons[i].x, buttons[i].y};
    button->transform->scale = {180, 60};
    
    SpriteRenderer* renderer = button->addComponent<SpriteRenderer>();
    renderer->tint = buttons[i].color;
    
    TouchButton* touchBtn = button->addComponent<TouchButton>();
    touchBtn->label = buttons[i].label;
    touchBtn->buttonId = buttons[i].id;
    touchBtn->normalColor = buttons[i].color;
}
```

### 触摸检测

```cpp
// 在 DemoScene::onUpdate() 中
if (input->isTouching()) {
    Vector2 touchPos = input->getTouchPosition();
    
    // 更新触摸指示器
    touchIndicator->active = true;
    touchIndicator->transform->position = touchPos;
    
    // 显示坐标
    DEBUG_LOG("Touch at: (%.0f, %.0f)", touchPos.x, touchPos.y);
} else {
    touchIndicator->active = false;
}
```

---

## 📊 InputManager 触控 API

### 可用方法

```cpp
InputManager* input = Engine::getInstance().getInput();

// 检查是否正在触摸
bool isTouching = input->isTouching();

// 获取触摸位置 (Vector2)
Vector2 pos = input->getTouchPosition();

// 检查是否刚按下
bool justPressed = input->getTouchDown();

// 检查是否刚松开
bool justReleased = input->getTouchUp();
```

### 使用示例

```cpp
// 检测点击某个区域
if (input->getTouchDown()) {
    Vector2 touchPos = input->getTouchPosition();
    
    if (touchPos.x >= 100 && touchPos.x <= 200 &&
        touchPos.y >= 100 && touchPos.y <= 200) {
        DEBUG_LOG("Clicked button area!");
    }
}
```

---

## 🎨 视觉反馈

### 按钮状态

| 状态 | 颜色 | 说明 |
|------|------|------|
| 正常 | 原色 | 未触摸 |
| 按下 | 变暗 60% | 正在触摸 |

### 颜色变化

```cpp
// 按下时颜色变暗
pressedColor = Color(
    normalColor.r * 0.6f,
    normalColor.g * 0.6f,
    normalColor.b * 0.6f
);
```

---

## 🎯 测试步骤

### 完整测试流程

1. **启动游戏**
   ```
   加载 switch2d-demo.nro
   ```

2. **打开控制台**
   ```
   按 A + B 键
   ```

3. **测试基础触摸**
   ```
   点击 "Touch Test" 按钮（蓝色）
   检查控制台是否显示成功信息
   ```

4. **测试网络功能**
   ```
   点击 "Network" 按钮（绿色）
   等待网络请求完成
   查看结果
   ```

5. **测试颜色变化**
   ```
   点击 "Color" 按钮（橙色）
   观察按钮颜色是否改变
   可以多次点击
   ```

6. **测试信息显示**
   ```
   点击 "Info" 按钮（紫色）
   查看系统信息
   ```

7. **测试触摸指示器**
   ```
   在屏幕任意位置触摸
   观察红色圆点是否跟随
   ```

---

## 🐛 故障排除

### 问题 1：点击无反应

**解决方案**：
- 确保已打开调试控制台（A+B）
- 检查鼠标是否正确模拟触摸
- 确认点击的是屏幕底部的彩色按钮

### 问题 2：触摸指示器不显示

**解决方案**：
- 触摸指示器默认隐藏
- 只在触摸时才显示
- 确认触摸坐标在屏幕范围内

### 问题 3：按钮颜色不变

**解决方案**：
- 颜色变化是随机的
- 可能随机到相近颜色
- 多点击几次观察

### 问题 4：网络测试失败

**解决方案**：
- 检查 Switch 网络连接
- 确认 Ryujinx 网络设置
- 查看详细错误信息

---

## 💡 扩展建议

### 添加更多按钮

```cpp
// 创建第5个按钮
GameObject* btn5 = createGameObject("New Button");
btn5->transform->position = {1200, 650};
btn5->transform->scale = {180, 60};

TouchButton* touch5 = btn5->addComponent<TouchButton>();
touch5->buttonId = 5;
touch5->label = "New Feature";
```

### 自定义按钮操作

```cpp
void onButtonClick() {
    switch (buttonId) {
        case 5:
            DEBUG_LOG("Custom action!");
            // 你的自定义代码
            break;
    }
}
```

### 添加文字标签

```cpp
// 可以使用 DebugConsole 或未来的文字渲染系统
// 在按钮上显示文字
```

---

## 📚 相关文档

- `NETWORK_GUIDE.md` - 网络功能详解
- `X_BUTTON_FEATURE.md` - X键网络测试
- `DEBUG_CONSOLE_GUIDE.md` - 调试控制台

---

## 🎉 功能特点

✅ **4个可交互按钮** - 不同颜色和功能  
✅ **实时触摸反馈** - 视觉和日志反馈  
✅ **触摸位置指示器** - 红色圆点跟随  
✅ **坐标实时显示** - 精确到像素  
✅ **完整的碰撞检测** - 准确识别点击  
✅ **多种测试功能** - 触摸、网络、颜色、信息  
✅ **易于扩展** - 可添加更多按钮  

---

**现在就试试触控功能吧！用鼠标点击屏幕底部的彩色按钮！** 👆✨

---

*Switch2D Engine - Touch-enabled gaming on Nintendo Switch*
