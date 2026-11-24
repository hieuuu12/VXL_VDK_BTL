/*
 * fsm_automatic.c  --  Máy trạng thái chế độ TỰ ĐỘNG (phụ trách: B)
 * KHÔNG có lệnh HAL_ trong file này (kiểm tra bằng tools/check_static.sh).
 */
#include "fsm_automatic.h"
#include "software_timer.h"
#include "global.h"

static tl_phase_t phase = PHASE_ALL_RED;
/* chụp lại thời gian lúc bắt đầu pha, để đếm ngược nhất quán trong cả chu kỳ */
static uint32_t green_ms  = TIME_GREEN_DEFAULT  * 1000u;
static uint32_t yellow_ms = TIME_AMBER_DEFAULT * 1000u;

static uint32_t phase_duration_ms(tl_phase_t p)
{
    switch (p) {
    case PHASE_R1G2:
    case PHASE_G1R2: return green_ms;
    case PHASE_R1Y2:
    case PHASE_Y1R2: return yellow_ms;
    default:         return AUTO_ALL_RED_MS;
    }
}

static void enter_phase(tl_phase_t p)
{
    phase = p;
    g_phase = p;                      /* biến chung của A: fsm_mode, display đọc được */
    traffic_apply_phase(p);
    timer_set(TIMER_AUTO_PHASE, phase_duration_ms(p));
}

void fsm_auto_enter(void)
{
    green_ms  = (uint32_t)g_time_green  * 1000u;
    yellow_ms = (uint32_t)g_time_amber * 1000u;
    enter_phase(PHASE_ALL_RED);
}

void fsm_auto_run(void)
{
    if (!timer_expired(TIMER_AUTO_PHASE)) return;

    switch (phase) {
    case PHASE_ALL_RED: enter_phase(PHASE_R1G2); break;
    case PHASE_R1G2:    enter_phase(PHASE_R1Y2); break;
    case PHASE_R1Y2:    enter_phase(PHASE_G1R2); break;
    case PHASE_G1R2:    enter_phase(PHASE_Y1R2); break;
    case PHASE_Y1R2:
        /* hết 1 chu kỳ: cập nhật thời gian mới (nếu vừa chỉnh) rồi lặp lại */
        green_ms  = (uint32_t)g_time_green  * 1000u;
        yellow_ms = (uint32_t)g_time_amber * 1000u;
        enter_phase(PHASE_R1G2);
        break;
    default:            enter_phase(PHASE_ALL_RED); break;   /* phòng thủ */
    }
}

tl_phase_t fsm_auto_get_phase(void) { return phase; }

/* Đếm ngược theo giây, làm tròn LÊN (còn 2,3 s hiển thị 3) */
uint16_t fsm_auto_countdown(uint8_t line)
{
    uint32_t rem = timer_remaining_ms(TIMER_AUTO_PHASE);
    uint32_t ms;
    switch (phase) {
    case PHASE_R1G2: ms = (line == 1) ? rem + yellow_ms : rem; break; /* tuyến 1 đỏ còn thêm pha vàng của tuyến 2 */
    case PHASE_G1R2: ms = (line == 2) ? rem + yellow_ms : rem; break;
    default:         ms = rem; break;                                  /* R1Y2, Y1R2, ALL_RED */
    }
    return (uint16_t)((ms + 999u) / 1000u);
}
