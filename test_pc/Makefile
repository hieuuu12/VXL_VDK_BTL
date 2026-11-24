# Test các module của B trên PC.  Chạy:  make
# Thư mục test_pc/ đứng trước Core/Inc để dùng main.h GIẢ LẬP thay cho HAL.
SRC = ../Core/Src/software_timer.c ../Core/Src/button.c ../Core/Src/traffic_light.c \
      ../Core/Src/fsm_automatic.c ../Core/Src/fsm_manual.c test_globals.c \
      mock_hal.c test_main.c

all: run

test_b: $(SRC)
	gcc -std=c99 -Wall -Wextra -I. -I../Core/Inc $(SRC) -o test_b

run: test_b
	./test_b

clean:
	rm -f test_b test_b.exe

.PHONY: all run clean
