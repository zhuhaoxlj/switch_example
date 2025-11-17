#!/bin/bash

# Switch2D Engine - 实时日志监控脚本

echo "=========================================="
echo "  Switch2D Engine - Log Monitor"
echo "=========================================="
echo ""
echo "监控 Ryujinx 日志中..."
echo "提示：运行游戏后，日志会实时显示在这里"
echo ""

# Ryujinx 日志目录（macOS）
# 尝试两个可能的位置
LOG_DIR="$HOME/Library/Logs/Ryujinx"
if [ ! -d "$LOG_DIR" ]; then
    LOG_DIR="$HOME/Library/Application Support/Ryujinx/Logs"
fi

if [ ! -d "$LOG_DIR" ]; then
    echo "错误：找不到 Ryujinx 日志目录"
    echo "请确认 Ryujinx 已安装并运行过"
    exit 1
fi

# 找到最新的日志文件
LATEST_LOG=$(ls -t "$LOG_DIR"/Ryujinx*.log 2>/dev/null | head -1)

if [ -z "$LATEST_LOG" ]; then
    echo "等待日志文件生成..."
    echo "请启动 Ryujinx 并运行游戏"
    echo ""
    
    # 等待新日志文件
    while [ -z "$LATEST_LOG" ]; do
        sleep 1
        LATEST_LOG=$(ls -t "$LOG_DIR"/Ryujinx*.log 2>/dev/null | head -1)
    done
fi

echo "找到日志文件："
echo "$LATEST_LOG"
echo ""
echo "=========================================="
echo "开始监控（按 Ctrl+C 退出）"
echo "=========================================="
echo ""

# 实时监控日志，只显示游戏相关的输出
tail -f "$LATEST_LOG" | grep --line-buffered -E "Player|Jump|Button|IsGrounded|Switch2D|Demo Scene|velocity" --color=auto

# 如果上面的过滤太严格，可以使用下面的命令查看所有内容：
# tail -f "$LATEST_LOG"
