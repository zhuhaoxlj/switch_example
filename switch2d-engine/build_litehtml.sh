#!/bin/bash
# ============================================
# Litehtml 预编译脚本
# ============================================
# 这个脚本将 litehtml 源文件编译成对象文件
# 然后创建静态库供主项目使用
# ============================================

set -e

echo "========================================="
echo "  Building Litehtml Library"
echo "========================================="

# 设置环境变量
export DEVKITPRO=/opt/devkitpro
export DEVKITA64=$DEVKITPRO/devkitA64
export PATH=$DEVKITA64/bin:$PATH

# 设置编译器
CXX=$DEVKITA64/bin/aarch64-none-elf-g++
CC=$DEVKITA64/bin/aarch64-none-elf-gcc
AR=$DEVKITA64/bin/aarch64-none-elf-ar

# 设置编译标志
ARCH="-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE"
CXXFLAGS="-g -Wall -O2 -ffunction-sections $ARCH -std=gnu++17 -fexceptions -frtti -DLITEHTML_NO_THREADS"
CFLAGS="-g -Wall -O2 -ffunction-sections $ARCH -std=c99"

# 包含路径
INCLUDES="-Iinclude -Ithird_party/litehtml/include -Ithird_party/litehtml/src -Ithird_party/litehtml/include/litehtml -Ithird_party/litehtml/src/gumbo/include -Ithird_party/litehtml/src/gumbo/include/gumbo"
GUMBO_INCLUDES="$INCLUDES -Ithird_party/litehtml/src/gumbo"

# 创建构建目录
mkdir -p build/litehtml_objs
mkdir -p build/litehtml_lib

echo "Compiling gumbo-parser..."

# 编译 Gumbo Parser (C 文件)
GUMBO_SRC="third_party/litehtml/src/gumbo"
for src in $GUMBO_SRC/*.c; do
    if [ -f "$src" ] && [[ ! "$src" =~ ".rl" ]]; then
        obj="build/litehtml_objs/$(basename ${src%.c}).o"
        echo "  $src"
        $CC $CFLAGS $GUMBO_INCLUDES -c "$src" -o "$obj" || {
            echo "Failed to compile $src"
            exit 1
        }
    fi
done

echo "Compiling litehtml..."

# 编译 Litehtml (C++ 文件)
LITEHTML_SRC="third_party/litehtml/src"
for src in $LITEHTML_SRC/*.cpp; do
    if [ -f "$src" ]; then
        obj="build/litehtml_objs/$(basename ${src%.cpp}).o"
        echo "  $src"
        
        $CXX $CXXFLAGS $INCLUDES -c "$src" -o "$obj" || {
            echo "Failed to compile $src"
            exit 1
        }
    fi
done

echo "Creating static library..."

# 创建静态库
$AR rcs build/litehtml_lib/liblitehtml.a build/litehtml_objs/*.o

echo ""
echo "========================================="
echo "  Litehtml Library Built Successfully!"
echo "========================================="
echo "Library: build/litehtml_lib/liblitehtml.a"
echo ""
