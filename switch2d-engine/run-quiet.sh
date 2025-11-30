#!/usr/bin/env fish

# 静默模式 - 过滤掉 Player Y 调试信息

set SWITCH_IP "192.168.0.249"
set LOG_FILE "logs/"(date +%Y%m%d_%H%M%S)".log"

mkdir -p logs

echo "🤫 Quiet Mode (filtering debug spam)"
echo "💾 Full log: $LOG_FILE"
echo ""

nxlink -s -a $SWITCH_IP switch2d-demo.nro 2>&1 | \
    tee $LOG_FILE | \
    grep --line-buffered -v "Player Y:"
