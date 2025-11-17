#!/bin/bash

echo "=========================================="
echo "  TTF 字体测试脚本"
echo "=========================================="
echo ""

echo "1. 检查 .nro 文件时间..."
ls -lh switch2d-demo.nro
echo ""

echo "2. 运行诊断..."
./diagnose_ttf.sh 2>&1 | grep "✅\|❌"
echo ""

echo "=========================================="
echo "  准备测试"
echo "=========================================="
echo ""
echo "现在请："
echo "1. 在 Ryujinx 中加载 switch2d-demo.nro"
echo "2. 同时在另一个终端运行："
echo ""
echo "   tail -f ~/Library/Logs/Ryujinx/*.log | grep -E '✅|❌|🎨|Loaded|First render|useTTF'"
echo ""
echo "3. 按 A+B 打开调试控制台"
echo "4. 查看字体效果和日志输出"
echo ""
echo "期望的日志输出："
echo "   ✅ Loaded TTF font: romfs:/fonts/JetBrainsMonoNerdFont-Regular.ttf"
echo "   ✅ useTTF = true, font = 0x..."
echo "   🎨 First render - useTTF=true, font=0x..."
echo ""
