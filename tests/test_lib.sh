if [ "$(cat /proc/sys/kernel/core_pattern)" != "core" ]; then
    echo "set /proc/sys/kernel/core_pattern to 'core'"
    exit 1
fi
if [ "$(cat /proc/sys/kernel/core_uses_pid)" != "1" ]; then
    echo "set /proc/sys/kernel/core_uses_pid to 1"
    exit 1
fi
ulimit -c unlimited
