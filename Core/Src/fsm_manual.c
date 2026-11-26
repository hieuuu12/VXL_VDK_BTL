/*
 * fsm_manual.c  --  Máy trạng thái chế độ BẰNG TAY (phụ trách: B)
 * KHÔNG có lệnh HAL_ trong file này.
 */
#include "fsm_manual.h"
#include "software_timer.h"
#include "button.h"
#include "global.h"

static tl_phase_t phase  = PHASE_ALL_RED;
static uint8_t    locked = 0;

void fsm_manual_enter(tl_phase_t current_phase)
{
    phase  = (current_phase < PHASE_COUNT) ? current_phase : PHASE_ALL_RED;
    locked = 0;
    timer_stop(TIMER_MANUAL_LOCK);
    g_phase = phase;
    traffic_apply_phase(phase);         /* xuất lại đúng pha cũ -> đèn không đổi */
}

void fsm_manual_run(void)
{
    if (locked && timer_expired(TIMER_MANUAL_LOCK)) {
        locked = 0;
    }

    if (button_is_pressed(BTN_2)) {     /* luôn đọc để xoá cờ, kể cả khi đang khoá */
        if (!locked) {
            phase = traffic_next_phase(phase);
            g_phase = phase;
            traffic_apply_phase(phase);
            locked = 1;
            timer_set(TIMER_MANUAL_LOCK, MANUAL_LOCK_MS);
        }
    }
}

tl_phase_t fsm_manual_get_phase(void) { return phase; }
uint8_t    fsm_manual_is_locked(void) { return locked; }
