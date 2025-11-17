#!/bin/bash

# Switch2D Engine - 字体设置脚本

echo "=========================================="
echo "  Switch2D Engine - Font Setup"
echo "=========================================="
echo ""

# 创建字体目录
mkdir -p romfs/fonts
cd romfs/fonts

echo "选择要安装的字体："
echo ""
echo "1) JetBrains Mono (推荐，适合编程)"
echo "2) 使用 macOS 系统字体 Monaco"
echo "3) 跳过（使用像素字体）"
echo ""
read -p "请选择 [1-3]: " choice

case $choice in
    1)
        echo ""
        echo "下载 JetBrains Mono..."
        
        # 检查是否已存在
        if [ -f "JetBrainsMono-Regular.ttf" ]; then
            echo "✓ JetBrainsMono-Regular.ttf 已存在"
        else
            # 下载字体
            curl -L "https://github.com/JetBrains/JetBrainsMono/releases/download/v2.304/JetBrainsMono-2.304.zip" -o jb.zip
            
            if [ $? -eq 0 ]; then
                echo "解压中..."
                unzip -q jb.zip
                
                # 复制需要的字体文件
                if [ -f "fonts/ttf/JetBrainsMono-Regular.ttf" ]; then
                    cp fonts/ttf/JetBrainsMono-Regular.ttf ./
                    echo "✓ JetBrains Mono 安装成功"
                else
                    echo "✗ 解压失败"
                    exit 1
                fi
                
                # 清理
                rm -rf fonts/ variable/ jb.zip
            else
                echo "✗ 下载失败，请检查网络连接"
                exit 1
            fi
        fi
        
        FONT_PATH="romfs:/fonts/JetBrainsMono-Regular.ttf"
        ;;
        
    2)
        echo ""
        echo "使用 macOS 系统字体 Monaco..."
        
        if [ ! -f "/System/Library/Fonts/Monaco.ttf" ]; then
            echo "✗ 未找到 Monaco.ttf"
            echo "你的系统可能不是 macOS 或字体路径不同"
            exit 1
        fi
        
        cp /System/Library/Fonts/Monaco.ttf ./
        echo "✓ Monaco 字体复制成功"
        
        FONT_PATH="romfs:/fonts/Monaco.ttf"
        ;;
        
    3)
        echo ""
        echo "跳过字体安装，将使用像素字体"
        cd ../..
        exit 0
        ;;
        
    *)
        echo "无效选择"
        exit 1
        ;;
esac

cd ../..

echo ""
echo "=========================================="
echo "  字体安装完成！"
echo "=========================================="
echo ""
echo "字体路径: $FONT_PATH"
echo ""
echo "下一步："
echo "1. 编译项目: ./build.sh"
echo "2. 在 Ryujinx 中运行 switch2d-demo.nro"
echo "3. 按 A+B 查看使用新字体的调试控制台"
echo ""
echo "提示: 字体默认已在代码中配置，无需手动修改"
echo ""
