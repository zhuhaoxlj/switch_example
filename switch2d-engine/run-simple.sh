#!/usr/bin/env fish

# 最简单的运行脚本 - 使用你之前成功的命令

set SWITCH_IP "192.168.0.249"
set LOG_FILE "logs/"(date +%Y%m%d_%H%M%S)".log"

mkdir -p logs

echo "Building and uploading..."
echo "Log: $LOG_FILE"
echo ""

make && nxlink -s -a $SWITCH_IP switch2d-demo.nro 2>&1 | tee $LOG_FILE
