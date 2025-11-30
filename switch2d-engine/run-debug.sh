#!/usr/bin/env fish

# Switch2D Engine 调试脚本
# 自动编译、上传并保存日志

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
echo ""

# 实时显示并保存日志（强制无缓冲）
stdbuf -o0 -e0 nxlink -s -a $SWITCH_IP switch2d-demo.nro 2>&1 | stdbuf -o0 tee $LOG_FILE

echo ""
echo "✅ Session ended. Log saved to: $LOG_FILE"
