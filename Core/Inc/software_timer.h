/*
 * software_timer.h  --  Bộ đếm thời gian phần mềm (phụ trách: B)
 *
 * - timer_run() được đăng ký là 1 task của scheduler, chu kỳ 1 tick = 10 ms.
 * - Các FSM dùng timer_set()/timer_expired() thay cho HAL_Delay.
 * - timer_run() và các FSM đều chạy trong SCH_Dispatch_Tasks() (vòng while(1)),
 *   KHÔNG chạy trong ngắt => không có tranh chấp dữ liệu, không cần tắt ngắt.
 */
#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <stdint.h>

#define TIMER_TICK_MS   10u     /* phải khớp chu kỳ task timer_run (1 tick scheduler) */

/* Danh sách timer dùng chung cả nhóm -- thêm ID mới ở đây, KHÔNG dùng số trần */
typedef enum {
    TIMER_AUTO_PHASE = 0,   /* fsm_automatic: thời gian còn lại của pha hiện tại   */
    TIMER_MANUAL_LOCK,      /* fsm_manual: khoá 1 s sau mỗi lần chuyển pha          */
    TIMER_TUNING,           /* dành cho C (fsm_tuning)                               */
    TIMER_DISPLAY,          /* dành cho C (display)                                  */
    TIMER_SPARE_0,
    TIMER_SPARE_1,
    TIMER_COUNT
} timer_id_t;

void     timer_init(void);
void     timer_set(timer_id_t id, uint32_t duration_ms); /* bắt đầu đếm, xoá cờ hết hạn */
void     timer_stop(timer_id_t id);                       /* dừng, không báo hết hạn      */
uint8_t  timer_expired(timer_id_t id);                    /* 1 nếu đã hết hạn             */
uint32_t timer_remaining_ms(timer_id_t id);               /* số ms còn lại (0 nếu hết)    */
void     timer_run(void);                                 /* TASK: gọi mỗi 10 ms          */

#endif /* SOFTWARE_TIMER_H */
