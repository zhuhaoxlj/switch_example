#!/bin/bash

# ============================================
# RmlUi 设置脚本
# ============================================
# 此脚本会下载并编译 RmlUi 库用于 Nintendo Switch
# ============================================

set -e

echo "=========================================="
echo " RmlUi Setup for Nintendo Switch"
echo "=========================================="
echo ""

# 检查 DevkitPro 环境
if [ -z "$DEVKITPRO" ]; then
    echo "ERROR: DEVKITPRO is not set!"
    echo "Please set DEVKITPRO environment variable"
    exit 1
fi

echo "✓ DevkitPro found: $DEVKITPRO"
echo ""

# 创建 third_party 目录
mkdir -p third_party
cd third_party

# 下载 RmlUi（如果不存在）
if [ ! -d "RmlUi" ]; then
    echo "📥 Downloading RmlUi..."
    git clone --depth 1 https://github.com/mikke89/RmlUi.git
    echo "✓ RmlUi downloaded"
else
    echo "✓ RmlUi already exists, skipping download"
fi

cd RmlUi

# 创建 build 目录
mkdir -p build-switch
cd build-switch

echo ""
echo "🔨 Configuring RmlUi for Switch..."

# 配置 CMake
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/switch.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_SAMPLES=OFF \
    -DENABLE_LOTTIE_PLUGIN=OFF \
    -DENABLE_SVG_PLUGIN=OFF \
    -DENABLE_TRACY_PROFILING=OFF \
    -DNO_FONT_INTERFACE_DEFAULT=OFF \
    -DNO_THIRDPARTY_CONTAINERS=OFF \
    -DCMAKE_INSTALL_PREFIX=../install

echo ""
echo "🔨 Building RmlUi..."

# 编译
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "📦 Installing RmlUi..."

# 安装
make install

echo ""
echo "✓ RmlUi built successfully!"

# 复制库文件到项目根目录
cd ../../..
mkdir -p lib
cp third_party/RmlUi/build-switch/libRmlCore.a lib/
echo "✓ Libraries copied to lib/"

echo ""
echo "=========================================="
echo " ✓ RmlUi Setup Complete!"
echo "=========================================="
echo ""
echo "Next steps:"
echo "1. Update Makefile to link RmlUi"
echo "2. Uncomment BrowserScene_RmlUi.h in main.cpp"
echo "3. Run 'make' to build the project"
echo ""
