#!/bin/bash

# TTF 字体诊断脚本

echo "=========================================="
echo "  TTF Font Diagnosis"
echo "=========================================="
echo ""

echo "1️⃣  检查字体文件..."
if [ -f "romfs/fonts/JetBrainsMonoNerdFont-Regular.ttf" ]; then
    size=$(stat -f%z "romfs/fonts/JetBrainsMonoNerdFont-Regular.ttf" 2>/dev/null || stat -c%s "romfs/fonts/JetBrainsMonoNerdFont-Regular.ttf" 2>/dev/null)
    echo "✅ 字体文件存在"
    echo "   路径: romfs/fonts/JetBrainsMonoNerdFont-Regular.ttf"
    echo "   大小: $size bytes"
else
    echo "❌ 字体文件不存在！"
    echo "   预期路径: romfs/fonts/JetBrainsMonoNerdFont-Regular.ttf"
    exit 1
fi

echo ""
echo "2️⃣  检查 SDL2_ttf 库..."
if [ -f "$DEVKITPRO/portlibs/switch/lib/libSDL2_ttf.a" ]; then
    echo "✅ SDL2_ttf 已安装"
    echo "   路径: $DEVKITPRO/portlibs/switch/lib/libSDL2_ttf.a"
else
    echo "❌ SDL2_ttf 未安装"
    exit 1
fi

echo ""
echo "3️⃣  检查 HAS_SDL_TTF 宏定义..."
if grep -q "#define HAS_SDL_TTF" include/Switch2D/DebugConsole.h; then
    echo "✅ HAS_SDL_TTF 已定义"
    grep "define HAS_SDL_TTF" include/Switch2D/DebugConsole.h
else
    echo "❌ HAS_SDL_TTF 未定义"
    exit 1
fi

echo ""
echo "4️⃣  检查 Makefile 链接库..."
if grep -q "lSDL2_ttf" Makefile; then
    echo "✅ SDL2_ttf 已添加到链接库"
    grep "LIBS.*SDL2_ttf" Makefile | head -1
else
    echo "❌ SDL2_ttf 未添加到 Makefile"
    exit 1
fi

echo ""
echo "5️⃣  检查字体路径配置..."
if grep -q "romfs:/fonts/JetBrainsMonoNerdFont-Regular.ttf" src/Core.cpp; then
    echo "✅ 字体路径配置正确"
    grep "fontPath.*romfs" src/Core.cpp | head -1
else
    echo "⚠️  字体路径可能不正确"
    grep "fontPath" src/Core.cpp | head -1
fi

echo ""
echo "=========================================="
echo "  诊断完成"
echo "=========================================="
echo ""
echo "✅ 所有检查通过！"
echo ""
echo "下一步："
echo "1. 运行游戏: switch2d-demo.nro"
echo "2. 查看 Ryujinx 日志中的调试信息："
echo "   - ✅ Loaded TTF font: ..."
echo "   - ✅ useTTF = true, font = 0x..."
echo "   - 🎨 First render - useTTF=true, font=0x..."
echo ""
echo "如果仍然是像素字体，请发送 Ryujinx 日志内容"
echo ""
