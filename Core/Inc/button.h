/*
 * button.h  --  Đọc nút nhấn, chống dội, nhấn giữ, auto-repeat (phụ trách: B)
 *
 * Bản đồ nút (chốt ở buổi kickoff, sửa nếu nhóm thống nhất khác):
 *   BTN_1    (D7)  : đổi chế độ  AUTO -> MANUAL -> TUNING -> AUTO   (fsm_mode, A)
 *   BTN_2    (D8)  : MANUAL: sang pha kế  |  TUNING: chọn đèn       (B / C)
 *   BTN_3    (D9)  : TUNING: +1 s (giữ để tăng nhanh)               (C)
 *   BTN_4    (D10) : TUNING: -1 s (giữ để giảm nhanh)               (C)
 *   BTN_USER (PC13): TUNING: Lưu                                     (C)
 */
#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

typedef enum {
    BTN_1 = 0,
    BTN_2,
    BTN_3,
    BTN_4,
    BTN_USER,
    BUTTON_COUNT
} button_id_t;

#define BUTTON_TICK_MS          10u     /* chu kỳ task button_read           */
#define BUTTON_HOLD_MS          1000u   /* giữ >= 1 s thì coi là nhấn giữ     */
#define BUTTON_REPEAT_MS        200u    /* khi đang giữ: phát sự kiện mỗi 200 ms */

void    button_init(void);
void    button_read(void);                    /* TASK: gọi mỗi 10 ms                      */

uint8_t button_is_pressed(button_id_t id);    /* 1 lần cho mỗi lần nhấn; đọc xong tự xoá cờ */
uint8_t button_is_repeat(button_id_t id);     /* sự kiện lặp mỗi 200 ms khi giữ >= 1 s     */
uint8_t button_is_held(button_id_t id);       /* trạng thái: đang giữ >= 1 s (không xoá)   */
uint8_t button_is_down(button_id_t id);       /* trạng thái: đang nhấn (đã chống dội)      */
void    button_clear_all(void);               /* xoá mọi cờ chờ -- gọi khi đổi chế độ      */

#endif /* BUTTON_H */
