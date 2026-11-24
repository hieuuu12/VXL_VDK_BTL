/*
 * button.c  --  Đọc nút nhấn, chống dội, nhấn giữ, auto-repeat (phụ trách: B)
 *
 * Chống dội 3 mẫu: mỗi 10 ms đọc 1 mẫu, chỉ khi 3 mẫu liên tiếp GIỐNG NHAU
 * mới cập nhật trạng thái ổn định. Nhấn được ghi nhận ở cạnh thả -> nhấn
 * của trạng thái ổn định, nên dội phím (nảy 0/1 trong vài ms) không tạo
 * thêm lần nhấn.
 */
#include "main.h"      /* HAL + User Label do CubeMX sinh: BTN_1_Pin, BTN_1_GPIO_Port, ... */
#include "button.h"

/* ====== CẤU HÌNH PHẦN CỨNG -- sửa theo kết quả lab 1 (mục 3.3) ====== */
/* Nút USER trên NUCLEO: CubeMX đặt nhãn "B1" cho PC13. Nếu nhóm đặt nhãn khác, sửa ở đây. */
#ifndef B1_Pin
#define B1_Pin        GPIO_PIN_13
#define B1_GPIO_Port  GPIOC
#endif

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    GPIO_PinState pressed_level;   /* mức logic khi NHẤN -- đo ở lab 1 */
} button_hw_t;

static const button_hw_t BTN_HW[BUTTON_COUNT] = {
    { BTN_1_GPIO_Port, BTN_1_Pin, GPIO_PIN_RESET },  /* TODO lab1: nhấn = 0 hay 1? */
    { BTN_2_GPIO_Port, BTN_2_Pin, GPIO_PIN_RESET },
    { BTN_3_GPIO_Port, BTN_3_Pin, GPIO_PIN_RESET },
    { BTN_4_GPIO_Port, BTN_4_Pin, GPIO_PIN_RESET },
    { B1_GPIO_Port,    B1_Pin,    GPIO_PIN_RESET },  /* USER trên NUCLEO: nhấn = 0 */
};
/* ================================================================= */

#define HOLD_TICKS    (BUTTON_HOLD_MS   / BUTTON_TICK_MS)   /* 100 */
#define REPEAT_TICKS  (BUTTON_REPEAT_MS / BUTTON_TICK_MS)   /* 20  */

static uint8_t  reg0[BUTTON_COUNT], reg1[BUTTON_COUNT], reg2[BUTTON_COUNT];
static uint8_t  stable[BUTTON_COUNT];        /* 1 = đang nhấn (đã chống dội) */
static uint16_t hold_cnt[BUTTON_COUNT];      /* số tick đã giữ               */
static uint8_t  flag_pressed[BUTTON_COUNT];
static uint8_t  flag_repeat[BUTTON_COUNT];

/* Đọc thô 1 nút, trả về 1 nếu đang nhấn (đã quy đổi theo mức logic) */
static uint8_t button_read_raw(button_id_t i)
{
    return (HAL_GPIO_ReadPin(BTN_HW[i].port, BTN_HW[i].pin) == BTN_HW[i].pressed_level) ? 1u : 0u;
}

void button_init(void)
{
    for (int i = 0; i < BUTTON_COUNT; i++) {
        reg0[i] = reg1[i] = reg2[i] = 0;
        stable[i] = 0;
        hold_cnt[i] = 0;
        flag_pressed[i] = 0;
        flag_repeat[i] = 0;
    }
}

void button_read(void)
{
    for (int i = 0; i < BUTTON_COUNT; i++) {
        reg2[i] = reg1[i];
        reg1[i] = reg0[i];
        reg0[i] = button_read_raw((button_id_t)i);

        if (reg0[i] == reg1[i] && reg1[i] == reg2[i]) {      /* 3 mẫu giống nhau */
            uint8_t now = reg0[i];
            if (now != stable[i]) {                          /* trạng thái ổn định đổi */
                stable[i] = now;
                if (now) {                                   /* cạnh nhấn */
                    flag_pressed[i] = 1;
                    hold_cnt[i] = 0;
                } else {                                     /* cạnh thả */
                    hold_cnt[i] = 0;
                }
            } else if (now) {                                /* đang giữ */
                if (hold_cnt[i] < 0xFFFFu) hold_cnt[i]++;
                /* sau 1 s: phát sự kiện ngay, rồi mỗi 200 ms một lần */
                if (hold_cnt[i] >= HOLD_TICKS &&
                    ((hold_cnt[i] - HOLD_TICKS) % REPEAT_TICKS) == 0) {
                    flag_repeat[i] = 1;
                }
            }
        }
    }
}

uint8_t button_is_pressed(button_id_t id)
{
    if (id >= BUTTON_COUNT) return 0;
    if (flag_pressed[id]) { flag_pressed[id] = 0; return 1; }
    return 0;
}

uint8_t button_is_repeat(button_id_t id)
{
    if (id >= BUTTON_COUNT) return 0;
    if (flag_repeat[id]) { flag_repeat[id] = 0; return 1; }
    return 0;
}

uint8_t button_is_held(button_id_t id)
{
    if (id >= BUTTON_COUNT) return 0;
    return (stable[id] && hold_cnt[id] >= HOLD_TICKS) ? 1u : 0u;
}

uint8_t button_is_down(button_id_t id)
{
    if (id >= BUTTON_COUNT) return 0;
    return stable[id];
}

void button_clear_all(void)
{
    for (int i = 0; i < BUTTON_COUNT; i++) {
        flag_pressed[i] = 0;
        flag_repeat[i]  = 0;
    }
}
