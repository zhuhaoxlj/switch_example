#!/usr/bin/env fish

# 性能监控模式 - 只显示 FPS 和错误

set SWITCH_IP "192.168.0.249"

echo "🎮 Performance Monitoring Mode"
echo "Only showing FPS and errors..."
echo ""

nxlink -s -a $SWITCH_IP switch2d-demo.nro 2>&1 | \
    grep --line-buffered -E "FPS|ERROR|WARNING|failed|assertion"
