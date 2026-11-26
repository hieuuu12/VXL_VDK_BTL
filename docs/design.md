# Thiết kế chung – BTL CO3009 (phần A giữ file này)

Mọi thay đổi trong file này ảnh hưởng tới code của cả nhóm, đổi gì phải báo trong nhóm trước.

## 1. Cấu hình project CubeIDE (.ioc) – chỉ A sửa

1. `File → New → STM32 Project` → thẻ **Board Selector** → **NUCLEO-F103RB** → chọn *No* khi hỏi khởi tạo mặc định tất cả ngoại vi (để chân sạch).
2. `System Core → SYS → Debug` = **Serial Wire**. Bắt buộc, sai là mạch chỉ nạp được 1 lần. Nhờ bước này PB3, PB4 (chân JTAG) mới dùng làm GPIO được.
3. GPIO (B đối chiếu lại schematic XNUCLEO, bảng dưới theo chuẩn Arduino của Nucleo-64):

| Chân Arduino | Chân STM32 | Chế độ | User Label |
| --- | --- | --- | --- |
| D3 | PB3 | Output Push-Pull | `TL1_A` |
| D4 | PB5 | Output Push-Pull | `TL1_B` |
| D5 | PB4 | Output Push-Pull | `TL2_A` |
| D6 | PB10 | Output Push-Pull | `TL2_B` |
| D7 | PA8 | Input, Pull-up | `BTN_1` |
| D8 | PA9 | Input, Pull-up | `BTN_2` |
| D9 | PC7 | Input, Pull-up | `BTN_3` |
| D10 | PB6 | Input, Pull-up | `BTN_4` |
| – | PA5 | Output Push-Pull | `LED_HB` |
| – | PC9 | Output Push-Pull | `LED_ERR` |
| – | PC13 | Input | `BTN_USER` |

4. **I2C1**: bật I2C (Standard mode 100 kHz), rồi đưa chân về **PB8 = I2C1_SCL, PB9 = I2C1_SDA** (bấm thẳng vào chân PB8/PB9 trên hình chip để chọn). Mặc định CubeMX đặt I2C1 ở PB6/PB7, mà PB6 là D10 (nút 4) → phải đổi. Ở lab, C chạy I2C scan để xác nhận.
5. **TIM2**: Clock Source = Internal Clock, bật `TIM2 global interrupt` trong NVIC. Chu kỳ 10 ms: T = (PSC + 1)(ARR + 1) / f_TIM.
   - Xem f_TIM ở thẻ Clock Configuration, dòng *APB1 Timer clocks*.
   - 64 MHz → PSC = 6399, ARR = 99. 72 MHz → PSC = 7199, ARR = 99. 8 MHz → PSC = 7999, ARR = 9.
6. Generate code, build không lỗi rồi mới commit.

## 2. Bản đồ nút nhấn

| Nút | AUTO | MANUAL | TUNING |
| --- | --- | --- | --- |
| B1 (D7) – `BTN_1` | → MANUAL | → TUNING | → AUTO (ĐỎ–ĐỎ 1 s trước) |
| B2 (D8) – `BTN_2` | – | Sang pha kế (khóa 1 s) | Chọn đèn cần chỉnh Đỏ → Xanh → Vàng |
| B3 (D9) – `BTN_3` | – | – | +1 s, giữ ≥ 1 s tăng liên tục |
| B4 (D10) – `BTN_4` | – | – | −1 s, giữ ≥ 1 s giảm liên tục |
| USER (PC13) – `BTN_USER` | Đổi trang LCD | Đổi trang LCD | Lưu (kiểm tra R = G + Y) |

`BTN_1` **chỉ** được xử lý trong `fsm_mode.c`. Các FSM con không đọc nút này.

## 3. Máy trạng thái chuyển chế độ (`fsm_mode`)

```
 reset           B1                  B1
 ─────► AUTO ─────────► MANUAL ─────────► TUNING
         ▲                                  │
         └──────────────── B1 ──────────────┘
```

- Mỗi lần vào AUTO đều gọi `fsm_auto_enter()`: 2 tuyến ĐỎ 1 s (`AUTO_ALL_RED_MS`), sau đó mới R1G2. Vì vậy cả lúc khởi động lẫn lúc từ TUNING về đều có khoảng xả ĐỎ–ĐỎ.
- Trong TUNING, `fsm_mode` thêm task `blink_red` nháy ĐỎ–ĐỎ / TẮT–TẮT 1 Hz. Cả 2 cặp này đều được `traffic_is_safe()` của B chấp nhận. Không dùng VÀNG–VÀNG vì `traffic_is_safe()` sẽ ép ĐỎ và bật `LED_ERR`.

| Cung | Đèn ngay sau chuyển | Task / timer | LCD (C) | Vì sao an toàn |
| --- | --- | --- | --- | --- |
| Reset → AUTO | ĐỎ–ĐỎ 1 s rồi R1G2 | `fsm_mode_init()` nạp giờ mặc định 5/3/2, gọi `fsm_auto_enter()` | Màn AUTO | Chỉ cấp Xanh sau khi cả 2 tuyến đã Đỏ |
| AUTO → MANUAL (B1) | Giữ nguyên pha | `button_clear_all()`; `fsm_manual_enter(fsm_auto_get_phase())` | Màn MANUAL | Pha đang chạy vốn hợp lệ, không nhảy đột ngột |
| MANUAL → TUNING (B1) | ĐỎ–ĐỎ ngay, nháy 1 Hz | `fsm_tuning_enter()`; `SCH_Add_Task(blink_red, 50, 50)`, lưu ID | Màn TUNING | Không còn tuyến nào Xanh |
| TUNING → AUTO (B1) | ĐỎ–ĐỎ 1 s rồi R1G2 | `fsm_tuning_exit()`; `SCH_Delete_Task(blink_id)`; `fsm_auto_enter()` đọc giờ mới | Màn AUTO | Xả ĐỎ–ĐỎ trước khi cấp Xanh với giờ mới |

Bất biến: nếu một tuyến Xanh thì tuyến kia phải Đỏ. Lớp 1 là FSM (chỉ dùng các cặp hợp lệ), lớp 2 là `traffic_set_pair()` của B (cặp sai → ép ĐỎ–ĐỎ, sáng `LED_ERR`).

## 4. Task trong scheduler (tick = 10 ms)

| Task | Delay | Period | File | Ai |
| --- | --- | --- | --- | --- |
| `timer_run` | 0 | 1 | software_timer.c | B |
| `button_read` | 0 | 1 | button.c | B |
| `fsm_mode_run` | 1 | 1 | fsm_mode.c | A |
| `display_update` | 3 | 25 | display.c | C |
| `lcd_flush` | 5 | 1 | lcd.c | C |
| `dht20_trigger` | 100 | 200 | dht20.c | C |
| `heartbeat` (PA5) | 0 | 50 | main.c | A |
| `blink_red` (chỉ có trong TUNING) | 50 | 50 | fsm_mode.c | A |
| `dht20_read` (one-shot) | 8 | 0 | dht20.c | C |
| `msg_clear` (one-shot) | 200 | 0 | display.c | C |

Tối đa khoảng 10 task cùng lúc, `SCH_MAX_TASKS = 16`.

## 5. API – hợp đồng giữa các file

Phần A (`scheduler.h`, `global.h`, `fsm_mode.h`):

```c
void     SCH_Init(void);
uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD);
uint8_t  SCH_Delete_Task(uint32_t taskID);
void     SCH_Update(void);
void     SCH_Dispatch_Tasks(void);

typedef enum { MODE_INIT, MODE_AUTO, MODE_MANUAL, MODE_TUNING } app_mode_t;
typedef enum { PHASE_ALL_RED, PHASE_R1G2, PHASE_R1Y2, PHASE_G1R2, PHASE_Y1R2 } phase_t;
extern app_mode_t g_mode;
extern phase_t    g_phase;
extern uint8_t    g_time_red, g_time_green, g_time_amber;

void fsm_mode_init(void);
void fsm_mode_run(void);
```

Các hàm `fsm_mode.c` gọi sang phần của B và C:

| File | Hàm | Ghi chú |
| --- | --- | --- |
| button.h (B) | `button_is_pressed(BTN_1)`, `button_clear_all()` | Đã có |
| traffic_light.h (B) | `traffic_set_pair(TL_RED / TL_OFF, ...)` | Đã có |
| fsm_automatic.h (B) | `fsm_auto_enter()`, `fsm_auto_run()`, `fsm_auto_get_phase()` | Đã có |
| fsm_manual.h (B) | `fsm_manual_enter(tl_phase_t)`, `fsm_manual_run()` | Đã có |
| fsm_tuning.h (C) | `fsm_tuning_enter()`, `fsm_tuning_run()`, `fsm_tuning_exit()` | Chỉnh trên bản tạm; **không** gọi `traffic_set_pair` (đèn do `blink_red` lo); Lưu hợp lệ thì ghi vào `g_time_*` |
| display.h (C) | `display_next_page()` | `fsm_mode` gọi khi nhấn USER ở AUTO hoặc MANUAL |

Tên hàm của B mà code C phải dùng: `button_is_pressed()`, `button_is_repeat()` (giữ ≥ 1 s, lặp mỗi 200 ms), `tl_color_t` với `TL_RED / TL_YELLOW / TL_GREEN / TL_OFF`, `TIMER_AUTO_PHASE`, `traffic_fault()`.

Luật chung: không gọi `HAL_` trong `fsm_*.c`; không `HAL_Delay` sau khi scheduler chạy; `display` chọn màn hình theo `g_mode`.

## 6. Ghép vào `main.c` (khi B, C xong)

1. Bỏ Exclude from Build của `fsm_mode.c`: chuột phải file → Resource Configurations → Exclude from Build → bỏ chọn hết.
2. Thay các đoạn `USER CODE` như sau.

```c
/* USER CODE BEGIN Includes */
#include "global.h"
#include "scheduler.h"
#include "software_timer.h"
#include "button.h"
#include "traffic_light.h"
#include "i2c_bus.h"
#include "lcd.h"
#include "dht20.h"
#include "display.h"
#include "fsm_mode.h"
/* USER CODE END Includes */
```

```c
/* USER CODE BEGIN 0 */
void heartbeat(void) {
	HAL_GPIO_TogglePin(LED_HB_GPIO_Port, LED_HB_Pin);
}
/* USER CODE END 0 */
```

```c
  /* USER CODE BEGIN 2 */
  i2c_bus_recover();
  lcd_init();
  dht20_init();
  display_init();
  lcd_flush_all();
  timer_init();
  button_init();
  traffic_init();
  SCH_Init();
  SCH_Add_Task(timer_run, 0, 1);
  SCH_Add_Task(button_read, 0, 1);
  SCH_Add_Task(fsm_mode_run, 1, 1);
  SCH_Add_Task(display_update, 3, 25);
  SCH_Add_Task(lcd_flush, 5, 1);
  SCH_Add_Task(dht20_trigger, 100, 200);
  SCH_Add_Task(heartbeat, 0, 50);
  fsm_mode_init();
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */
```

```c
    /* USER CODE BEGIN 3 */
	  SCH_Dispatch_Tasks();
  }
  /* USER CODE END 3 */
```

```c
/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		SCH_Update();
	}
}
/* USER CODE END 4 */
```

