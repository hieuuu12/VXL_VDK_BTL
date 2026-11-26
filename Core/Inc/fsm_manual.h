/*
 * fsm_manual.h  --  Máy trạng thái chế độ BẰNG TAY (phụ trách: B)
 *
 * - Vào từ AUTO: GIỮ NGUYÊN pha đèn đang sáng (không nhảy đèn đột ngột).
 * - Nhấn BTN_2: chuyển sang pha kế theo CHUỖI AN TOÀN
 *       R1G2 -> R1Y2 -> G1R2 -> Y1R2 -> R1G2   (ALL_RED -> R1G2)
 *   => không thể nhảy thẳng từ Xanh sang Đỏ, luôn qua Vàng.
 * - Khoá 1 s sau mỗi lần chuyển: nhấn trong lúc khoá bị BỎ QUA
 *   => pha Vàng luôn sáng tối thiểu 1 s, nhấn liên tục không làm rối đèn.
 * File này KHÔNG gọi HAL.
 */
#ifndef FSM_MANUAL_H
#define FSM_MANUAL_H

#include <stdint.h>
#include "traffic_light.h"

#define MANUAL_LOCK_MS   1000u

void       fsm_manual_enter(tl_phase_t current_phase);
void       fsm_manual_run(void);
tl_phase_t fsm_manual_get_phase(void);
uint8_t    fsm_manual_is_locked(void);   /* để LCD hiện "WAIT" nếu muốn */

#endif /* FSM_MANUAL_H */
