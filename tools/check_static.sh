#!/usr/bin/env bash
# check_static.sh -- Kiểm tra tĩnh trước khi demo/nộp (Gantt 3.4, phụ trách: B)
# Chạy ở thư mục gốc repo:   bash tools/check_static.sh
# Trên Windows: chạy trong "Git Bash" (cài kèm Git).

ROOT="${1:-.}"
SRC="$ROOT/Core/Src"
fail=0

echo "== 1. Không có lệnh HAL_ trong các file fsm_*.c =="
hits=$(grep -nE "HAL_[A-Za-z_]+[[:space:]]*\\(" "$SRC"/fsm_*.c 2>/dev/null)
if [ -n "$hits" ]; then echo "$hits"; echo "  -> FAIL"; fail=1; else echo "  -> PASS"; fi

echo "== 2. Không còn HAL_Delay (trừ hàm khởi tạo trong lcd/dht20 nếu nhóm cho phép) =="
hits=$(grep -n "HAL_Delay" "$SRC"/*.c 2>/dev/null | grep -v "^.*stm32f1xx_hal" )
if [ -n "$hits" ]; then echo "$hits"; echo "  -> KIỂM TRA LẠI (mỗi dòng trên phải có lý do)"; fail=1; else echo "  -> PASS"; fi

echo "== 3. Lệnh HAL_GPIO/HAL_I2C chỉ nằm trong module phụ trợ + main.c =="
allowed="main.c|button.c|traffic_light.c|lcd.c|dht20.c|stm32f1xx_|system_stm32|syscalls.c|sysmem.c"
hits=$(grep -ln "HAL_GPIO_\|HAL_I2C_" "$SRC"/*.c 2>/dev/null | grep -Ev "$allowed")
if [ -n "$hits" ]; then echo "$hits"; echo "  -> FAIL"; fail=1; else echo "  -> PASS"; fi

echo "== 4. Không ai gọi thẳng traffic_write_line (bỏ qua kiểm tra an toàn) =="
hits=$(grep -n "traffic_write_line" "$SRC"/*.c 2>/dev/null | grep -v "traffic_light.c")
if [ -n "$hits" ]; then echo "$hits"; echo "  -> FAIL"; fail=1; else echo "  -> PASS"; fi

echo "== 5. Không còn TODO lab1 chưa điền =="
hits=$(grep -n "TODO lab1" "$SRC"/*.c 2>/dev/null)
if [ -n "$hits" ]; then echo "$hits"; echo "  -> CHƯA XONG"; fail=1; else echo "  -> PASS"; fi

echo
echo "Ghi chú: mục 'build -Wall 0 warning' kiểm tra trong STM32CubeIDE:"
echo "  Project > Properties > C/C++ Build > Settings > MCU GCC Compiler > Warnings: bật -Wall -Wextra,"
echo "  Project > Clean... rồi Build, xem tab Problems phải 0 warning."
[ $fail -eq 0 ] && echo "TỔNG: PASS" || echo "TỔNG: CÒN MỤC CẦN SỬA"
exit $fail
