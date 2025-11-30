#!/usr/bin/env fish

# Switch2D Engine 调试脚本（无缓冲版本）
# 需要安装: brew install expect

set SWITCH_IP "192.168.0.249"
set LOG_DIR "logs"
set LOG_FILE "$LOG_DIR/"(date +%Y%m%d_%H%M%S)".log"

# 确保日志目录存在
mkdir -p $LOG_DIR

echo "🔨 Building..."
make
if test $status -ne 0
    echo "❌ Build failed!"
    exit 1
end

echo "📤 Uploading to Switch ($SWITCH_IP)..."
echo "💾 Log will be saved to: $LOG_FILE"
echo "⏳ Waiting for Switch connection..."
echo "💡 Make sure Switch is in Homebrew Menu!"
echo ""

# 使用 unbuffer 强制实时输出
if command -v unbuffer >/dev/null
    unbuffer nxlink -s -a $SWITCH_IP switch2d-demo.nro 2>&1 | tee $LOG_FILE
else
    echo "⚠️  unbuffer not found, using stdbuf..."
    stdbuf -o0 -e0 nxlink -s -a $SWITCH_IP switch2d-demo.nro 2>&1 | stdbuf -o0 tee $LOG_FILE
end

echo ""
echo "✅ Session ended. Log saved to: $LOG_FILE"
