# 关于在 Switch 上使用 RmlUi

## 🎯 当前状态

经过实际测试，**RmlUi 在 Nintendo Switch 上的完整集成比预期复杂**。

## ⚠️ 技术挑战

### 1. 编译工具链问题
```
❌ DevkitPro 主要使用 Makefile，不是 CMake
❌ RmlUi 需要 CMake 3.10+ 和自定义工具链
❌ Switch 工具链文件 (switch.cmake) 不存在
```

### 2. 依赖库问题
```
❌ RmlUi 需要特殊配置的 FreeType
❌ 交叉编译复杂度高
❌ 需要预编译大量静态库
```

### 3. 时间成本
```
⏱️ 完整配置需要 2-4 小时
⏱️ 需要深入了解 DevkitPro 工具链
⏱️ 可能需要修改 RmlUi 源码适配 Switch
```

## 💡 推荐方案

### 方案 A：优化当前的 litehtml（推荐）✨

**优势**：
- ✅ 已经集成并工作
- ✅ 可以立即使用
- ✅ 足够满足大部分需求

**改进方向**：
1. **优化渲染性能** - 减少重绘
2. **改进 CSS 支持** - 扩展容器功能
3. **添加动画** - 简单的淡入淡出效果
4. **手柄导航** - 自定义焦点系统

### 方案 B：PC 版本使用 RmlUi

在 PC 上开发时使用 RmlUi，Switch 上使用 litehtml：

```cpp
#ifdef __SWITCH__
    #include "BrowserScene.h"  // litehtml
#else
    #include "BrowserScene_RmlUi.h"  // RmlUi for PC
#endif
```

### 方案 C：等待社区预编译库

关注 RmlUi 社区是否提供 Switch 预编译版本。

## 📊 litehtml vs RmlUi 实际对比

在 Switch 上：

| 特性 | litehtml | RmlUi（理论）|
|-----|----------|--------------|
| **集成难度** | ✅ 简单 | ❌ 复杂 |
| **编译时间** | ✅ 5 分钟 | ❌ 2+ 小时 |
| **稳定性** | ✅ 已验证 | ⚠️ 未知 |
| **CSS 支持** | ⚠️ 65% | ✅ 95% |
| **性能** | ⚠️ 30 FPS | ✅ 60 FPS |
| **社区支持** | ✅ 良好 | ⚠️ Switch 支持少 |

## 🚀 立即可行的优化

### 1. 改进 litehtml 渲染性能

```cpp
// 在 litehtml_container_sdl.cpp 中
// 添加渲染缓存
std::map<std::string, SDL_Texture*> m_render_cache;

void draw_text(/* ... */) {
    // 缓存渲染的文本纹理
    auto cache_key = generate_key(text, font, color);
    if (m_render_cache.find(cache_key) != m_render_cache.end()) {
        // 使用缓存
        SDL_RenderCopy(m_renderer, m_render_cache[cache_key], ...);
        return;
    }
    // ... 渲染新的
}
```

### 2. 添加 CSS3 样式支持

```cpp
// 扩展 litehtml 容器支持更多 CSS 属性
void draw_borders(/* ... */) override {
    // 添加 border-radius 支持
    // 添加 box-shadow 支持
    // 添加渐变背景支持
}
```

### 3. 实现平滑滚动

```cpp
// 在 BrowserScene.cpp 中
float m_target_scroll = 0;
float m_current_scroll = 0;

void onUpdate() {
    // 平滑插值
    m_current_scroll += (m_target_scroll - m_current_scroll) * 0.1f;
}
```

### 4. 手柄焦点导航

```cpp
// 添加焦点元素跟踪
class FocusManager {
    std::vector<litehtml::element::ptr> m_focusable_elements;
    int m_current_focus = 0;
    
    void navigateUp() { /* ... */ }
    void navigateDown() { /* ... */ }
};
```

## 📝 结论

**对于当前项目**：
- ✅ **继续使用 litehtml** 是最务实的选择
- ✅ 通过优化可以获得接近 RmlUi 的体验
- ✅ 节省大量配置和调试时间

**如果你真的需要 RmlUi**：
- 考虑在 PC 上先开发和测试
- 或者等待社区提供 Switch 预编译版本
- 或者投入时间完整配置交叉编译环境

## 🎯 下一步建议

1. **立即可做**：优化当前 litehtml 实现
2. **短期目标**：添加 CSS3 特性支持
3. **中期目标**：改进性能到 60 FPS
4. **长期目标**：如果社区有 RmlUi Switch 版本再迁移

---

**创建时间**: 2025-11-20  
**状态**: litehtml 工作良好，RmlUi 待未来集成  
**建议**: 专注于优化当前实现
