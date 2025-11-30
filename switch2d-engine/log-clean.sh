#!/usr/bin/env fish

# 日志管理脚本

set LOG_DIR "logs"

if not test -d $LOG_DIR
    echo "No logs directory found."
    exit 0
end

set log_count (count $LOG_DIR/*.log)

if test $log_count -eq 0
    echo "No log files found."
    exit 0
end

echo "📊 Log files: $log_count"
echo ""

# 显示最新 5 个日志
echo "📝 Latest logs:"
ls -lht $LOG_DIR/*.log | head -5

echo ""
echo "Options:"
echo "  1) View latest log"
echo "  2) Delete old logs (keep latest 10)"
echo "  3) Delete all logs"
echo "  4) Exit"
echo ""

read -P "Choice: " choice

switch $choice
    case 1
        set latest (ls -t $LOG_DIR/*.log | head -1)
        echo "Viewing: $latest"
        echo ""
        less $latest
    case 2
        ls -t $LOG_DIR/*.log | tail -n +11 | xargs rm -f
        echo "✅ Cleaned! Kept latest 10 logs."
    case 3
        rm -f $LOG_DIR/*.log
        echo "✅ All logs deleted."
    case 4
        echo "Bye!"
    case '*'
        echo "Invalid choice."
end
