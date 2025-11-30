#!/usr/bin/env fish

# 测试 Switch 连接

set SWITCH_IP "192.168.0.249"

echo "🔍 Testing Switch connection..."
echo "IP: $SWITCH_IP"
echo ""

echo "1️⃣ Ping test..."
ping -c 3 $SWITCH_IP

echo ""
echo "2️⃣ Port check (nxlink uses port 28771)..."
nc -z -v $SWITCH_IP 28771 2>&1

echo ""
echo "3️⃣ Ready to upload. Make sure:"
echo "   ✓ Switch is powered on"
echo "   ✓ In Homebrew Menu"
echo "   ✓ On same WiFi network"
