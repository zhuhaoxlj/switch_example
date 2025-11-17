# 字体使用说明

## drawText 函数签名

```cpp
void drawText(const char* text, int x, int y, SDL_Color color = {255, 255, 255, 255}, int fontSize = 2)
```

## 参数说明

- **text**: 要显示的文本字符串（支持 `\n` 换行符）
- **x**: 文字起始 X 坐标
- **y**: 文字起始 Y 坐标
- **color**: 文字颜色（RGBA，默认白色）
- **fontSize**: 字号大小（整数倍缩放，默认为 2）

## 使用示例

### 1. 基础用法（使用默认字号 2）

```cpp
drawText("Hello World", 100, 100);
```

### 2. 指定颜色

```cpp
SDL_Color redColor = {255, 0, 0, 255};
drawText("Red Text", 100, 100, redColor);
```

### 3. 使用不同字号

```cpp
// 小字（1x）
SDL_Color grayColor = {150, 150, 150, 255};
drawText("Small text", 100, 100, grayColor, 1);

// 正常字（2x，默认）
SDL_Color whiteColor = {255, 255, 255, 255};
drawText("Normal text", 100, 120, whiteColor, 2);

// 大字（3x）
SDL_Color yellowColor = {255, 255, 0, 255};
drawText("Large text", 100, 150, yellowColor, 3);

// 超大字（4x）
SDL_Color redColor = {255, 0, 0, 255};
drawText("Extra Large", 100, 200, redColor, 4);
```

### 4. 多行文本

```cpp
SDL_Color color = {255, 255, 255, 255};
drawText("Line 1\nLine 2\nLine 3", 100, 100, color, 2);
```

### 5. 综合示例（游戏标题画面）

```cpp
void drawTitleScreen() {
    // 游戏标题（大字 4x）
    SDL_Color titleColor = {100, 255, 100, 255};
    drawText("SNAKE GAME", SCREEN_WIDTH / 2 - 200, 100, titleColor, 4);
    
    // 副标题（正常字 2x）
    SDL_Color subtitleColor = {255, 255, 255, 255};
    drawText("Press A to Start", SCREEN_WIDTH / 2 - 120, 220, subtitleColor, 2);
    
    // 版权信息（小字 1x）
    SDL_Color copyrightColor = {150, 150, 150, 255};
    drawText("Powered by Feizhu @2025", SCREEN_WIDTH / 2 - 92, 680, copyrightColor, 1);
}
```

## 字号对照表

| 字号 | 实际大小 | 字符宽度 | 字符高度 | 推荐用途 |
|------|---------|---------|---------|---------|
| 1x   | 8x7 像素  | 8px     | 7px     | 小字、注释、版权信息 |
| 2x   | 16x14 像素 | 16px    | 14px    | 正常文字、默认字号 |
| 3x   | 24x21 像素 | 24px    | 21px    | 标题、重要提示 |
| 4x   | 32x28 像素 | 32px    | 28px    | 大标题、游戏 LOGO |
| 5x   | 40x35 像素 | 40px    | 35px    | 超大标题 |

## 行高和间距

- **字符间距**: `8 * fontSize` 像素
- **行高**: `7 * fontSize + 4` 像素

例如：
- fontSize = 1: 字符间距 8px，行高 11px
- fontSize = 2: 字符间距 16px，行高 18px
- fontSize = 3: 字符间距 24px，行高 25px

## 支持的字符

### 数字
`0 1 2 3 4 5 6 7 8 9`

### 大写字母
`A B C D E F G H I L M N O P R S T U V W`

### 小写字母
`a b c d e f g h i k l m n o p r s t u v w y z`

### 符号
`@ : 空格`

### 特殊字符
`\n` - 换行符

## 性能提示

1. 字号越大，渲染开销越大
2. 每帧尽量减少文字渲染次数
3. 静态文字可以考虑预渲染到纹理
4. 动态文字（如分数）每帧更新

## 居中文本计算

如果需要让文本居中显示：

```cpp
// 计算文本宽度
int textLength = strlen(text);
int fontSize = 2;
int textWidth = textLength * 8 * fontSize;

// 居中 X 坐标
int centerX = (SCREEN_WIDTH - textWidth) / 2;

// 绘制居中文本
drawText(text, centerX, y, color, fontSize);
```

## 注意事项

1. 不支持的字符会被自动忽略（不显示）
2. 换行符 `\n` 会重置 X 坐标到起始位置
3. 字号必须是正整数（推荐 1-5）
4. 颜色使用 RGBA 格式，Alpha 通道设为 255 为完全不透明
