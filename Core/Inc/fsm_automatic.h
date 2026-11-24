/*
 * fsm_automatic.h  --  Máy trạng thái chế độ TỰ ĐỘNG (phụ trách: B)
 *
 * Chuỗi pha:  ALL_RED -> R1G2 -> R1Y2 -> G1R2 -> Y1R2 -> R1G2 -> ...
 * Thời gian:  R1G2, G1R2 kéo dài g_time_green; R1Y2, Y1R2 kéo dài g_time_amber;
 *             ALL_RED kéo dài AUTO_ALL_RED_MS (chỉ xuất hiện khi mới vào AUTO).
 * Đèn Đỏ của 1 tuyến = Xanh + Vàng của tuyến kia, nên g_time_red không cần đọc
 * trực tiếp -- ràng buộc R = G + Y luôn đúng "tự động".
 *
 * Cách dùng (fsm_mode của A gọi):
 *   - Khi vào chế độ AUTO:  fsm_auto_enter();
 *   - Mỗi 10 ms khi đang AUTO: fsm_auto_run();
 * File này KHÔNG gọi HAL, KHÔNG đọc nút (nút đổi chế độ do fsm_mode xử lý).
 */
#ifndef FSM_AUTOMATIC_H
#define FSM_AUTOMATIC_H

#include <stdint.h>
#include "traffic_light.h"

#define AUTO_ALL_RED_MS   1000u

void       fsm_auto_enter(void);
void       fsm_auto_run(void);
tl_phase_t fsm_auto_get_phase(void);
uint16_t   fsm_auto_countdown(uint8_t line);   /* số giây còn lại của màu hiện tại, line = 1/2 */

#endif /* FSM_AUTOMATIC_H */
