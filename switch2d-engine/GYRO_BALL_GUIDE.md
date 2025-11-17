# 🎮 陀螺仪小球游戏

Switch2D Engine - 陀螺仪控制小球关卡

---

## ✨ 新增功能

**第二个按钮（绿色）现在可以启动陀螺仪控制小球的新关卡！**

---

## 🎯 游戏玩法

### 游戏概述

- **场景**: 一个正方形的边界框
- **小球**: 金黄色的小球
- **控制**: 通过**陀螺仪倾斜设备**来移动小球
- **物理**: 真实的物理碰撞和惯性

### 游戏目标

体验陀螺仪控制的乐趣！倾斜你的设备，让小球在框内滚动。

---

## 🎮 如何玩

### 1. 进入游戏

**方法 1: 触摸按钮**
- 点击屏幕底部的 **🟢 Gyro Ball** 按钮（绿色）

**方法 2: 在模拟器中**
- 打开调试控制台（A+B）
- 点击绿色按钮

### 2. 控制小球

- **倾斜设备向左** - 小球向左滚动
- **倾斜设备向右** - 小球向右滚动
- **倾斜设备向前** - 小球向上滚动
- **倾斜设备向后** - 小球向下滚动

### 3. 返回主菜单

- 按 **B 键**返回主菜单

---

## 🎨 场景布局

```
┌────────────────────────────────┐
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ← 上边界（灰色）
│ ▓                            ▓ │
│ ▓                            ▓ │
│ ▓          🟡               ▓ │ ← 小球（金黄色）
│ ▓        (小球)             ▓ │
│ ▓                            ▓ │
│ ▓                            ▓ │
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ← 下边界（灰色）
└────────────────────────────────┘
  ↑                            ↑
  左边界                      右边界
```

### 场景元素

| 元素 | 描述 | 尺寸 |
|------|------|------|
| 边界框 | 灰色正方形 | 600x600 像素 |
| 上下墙壁 | 灰色横条 | 600x20 像素 |
| 左右墙壁 | 灰色竖条 | 20x600 像素 |
| 小球 | 金黄色圆球 | 直径 40 像素 |

---

## 🔧 技术细节

### GyroScene 场景

```cpp
class GyroScene : public Scene {
    // 创建正方形边界
    // 创建小球
    // 添加陀螺仪控制
    // 处理返回
};
```

### GyroController 组件

```cpp
class GyroController : public Component {
public:
    float sensitivity = 500.0f;  // 灵敏度
    
    void onUpdate() override {
        // 获取陀螺仪数据
        Vector2 gyro = input->getGyro();
        
        // 转换为力
        Vector2 force = {
            gyro.x * sensitivity,
            gyro.y * sensitivity
        };
        
        // 应用到刚体
        rb->velocity += force * Time::deltaTime;
    }
};
```

### 物理系统

- **Rigidbody** - 刚体物理
- **CircleCollider** - 圆形碰撞体
- **BoxCollider** - 矩形碰撞体（边界）
- **drag** - 阻力 0.3（模拟摩擦）
- **mass** - 质量 1.0

---

## 📊 陀螺仪 API

### InputManager

```cpp
InputManager* input = Engine::getInstance().getInput();

// 获取陀螺仪数据
Vector2 gyro = input->getGyro();

// gyro.x - 左右倾斜 (-1.0 到 1.0)
// gyro.y - 前后倾斜 (-1.0 到 1.0)
```

### 数据转换

```cpp
// 陀螺仪数据 → 力
Vector2 force = {
    gyro.x * sensitivity,  // X轴力
    gyro.y * sensitivity   // Y轴力
};

// 施加力到刚体
rb->velocity.x += force.x * Time::deltaTime;
rb->velocity.y += force.y * Time::deltaTime;
```

---

## 🎯 调试信息

### 陀螺仪数据（每秒显示）

```
Gyro: (0.234, -0.156) | Force: (117.0, -78.0)
```

- **Gyro** - 陀螺仪原始数据
- **Force** - 计算出的力

### 小球位置（每2秒显示）

```
Ball pos: (640, 360) | Vel: (45.2, -23.1)
```

- **Ball pos** - 小球位置
- **Vel** - 速度向量

---

## 💡 游戏特性

### ✅ 已实现

1. **陀螺仪控制** - 实时响应设备倾斜
2. **物理模拟** - 真实的惯性和碰撞
3. **边界碰撞** - 小球不会飞出边界
4. **阻力系统** - 自然的减速
5. **调试信息** - 实时显示数据
6. **场景切换** - 按B返回主菜单

### 🎨 视觉效果

- **深蓝色背景** - RGB(30, 30, 50)
- **灰色边界** - RGB(100, 100, 100)
- **金黄色小球** - RGB(255, 200, 0)

---

## 🎮 控制总结

| 输入 | 功能 |
|------|------|
| 倾斜设备 | 移动小球 |
| B 键 | 返回主菜单 |
| A + B | 调试控制台 |

---

## 📝 代码实现

### 创建边界

```cpp
// 上边界
GameObject* topWall = createGameObject("Top Wall");
topWall->transform->position = {640, 60};
topWall->transform->scale = {600, 20};
BoxCollider* topCol = topWall->addComponent<BoxCollider>();
topCol->size = {600, 20};
```

### 创建小球

```cpp
GameObject* ball = createGameObject("Ball");
ball->transform->position = {640, 360};
ball->transform->scale = {40, 40};

// 刚体
Rigidbody* rb = ball->addComponent<Rigidbody>();
rb->mass = 1.0f;
rb->drag = 0.3f;
rb->useGravity = false;

// 碰撞体
CircleCollider* col = ball->addComponent<CircleCollider>();
col->radius = 20.0f;

// 陀螺仪控制
GyroController* gyro = ball->addComponent<GyroController>();
gyro->sensitivity = 500.0f;
```

### 场景切换

```cpp
// 加载陀螺仪场景
Engine::getInstance().loadScene(std::make_unique<GyroScene>());

// 返回主菜单
Engine::getInstance().loadScene(std::make_unique<DemoScene>());
```

---

## 🎓 学习要点

### 1. 陀螺仪输入

```cpp
Vector2 gyro = input->getGyro();
// 返回设备的倾斜角度
```

### 2. 力的应用

```cpp
rb->velocity += force * Time::deltaTime;
// deltaTime 确保帧率无关的物理
```

### 3. 组件系统

```cpp
GyroController* gyro = ball->addComponent<GyroController>();
// 添加自定义组件
```

### 4. 场景管理

```cpp
Engine::getInstance().loadScene(std::make_unique<Scene>());
// 切换场景
```

---

## 🐛 故障排除

### 问题 1：陀螺仪不工作

**症状**: 小球不移动

**解决方案**:
- Ryujinx 模拟器可能不支持陀螺仪
- 在真实 Switch 上测试
- 使用右摇杆模拟（如果实现）

### 问题 2：小球飞出边界

**症状**: 小球穿墙

**解决方案**:
- 检查碰撞体设置
- 确保 BoxCollider 和 CircleCollider 正确
- 查看碰撞检测是否启用

### 问题 3：小球不停

**症状**: 小球一直滑动

**解决方案**:
- 增加 `drag` 值（当前 0.3）
- 减少 `sensitivity`（当前 500）

### 问题 4：按B返回失败

**症状**: 无法返回主菜单

**解决方案**:
- 确保在陀螺仪场景中
- 检查输入系统
- 查看调试日志

---

## 🎨 自定义参数

### 修改灵敏度

```cpp
GyroController* gyro = ball->addComponent<GyroController>();
gyro->sensitivity = 300.0f;  // 降低灵敏度
gyro->sensitivity = 800.0f;  // 增加灵敏度
```

### 修改阻力

```cpp
rb->drag = 0.5f;  // 更大阻力（更快停止）
rb->drag = 0.1f;  // 更小阻力（更滑）
```

### 修改边界大小

```cpp
float boxSize = 800.0f;  // 更大的边界
float boxSize = 400.0f;  // 更小的边界
```

### 修改小球大小

```cpp
ball->transform->scale = {60, 60};  // 更大的球
ballCol->radius = 30.0f;

ball->transform->scale = {20, 20};  // 更小的球
ballCol->radius = 10.0f;
```

---

## 🚀 扩展建议

### 1. 添加目标点

```cpp
// 创建目标
GameObject* target = createGameObject("Target");
target->transform->position = {800, 200};
// 检测小球是否到达目标
```

### 2. 添加障碍物

```cpp
// 创建障碍
GameObject* obstacle = createGameObject("Obstacle");
obstacle->addComponent<BoxCollider>();
// 小球碰到障碍会反弹
```

### 3. 计分系统

```cpp
int score = 0;
// 小球到达目标时增加分数
if (distance(ball, target) < 50) {
    score++;
}
```

### 4. 多个小球

```cpp
for (int i = 0; i < 5; i++) {
    GameObject* ball = createGameObject("Ball" + std::to_string(i));
    // 创建多个小球
}
```

---

## 📚 相关文档

- `TOUCHSCREEN_GUIDE.md` - 触控功能
- `NETWORK_GUIDE.md` - 网络功能
- `API_REFERENCE.md` - API 文档

---

## 🎉 功能总结

✅ **陀螺仪控制系统**  
✅ **物理碰撞系统**  
✅ **正方形边界**  
✅ **实时调试信息**  
✅ **场景切换**  
✅ **按钮启动**  
✅ **完整的游戏循环**  

---

**现在就试试！点击绿色 Gyro Ball 按钮，倾斜设备控制小球！** 🎮🟡✨

---

*Switch2D Engine - Motion-controlled gaming on Nintendo Switch*
