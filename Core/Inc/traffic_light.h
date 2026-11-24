/*
 * traffic_light.h  --  Điều khiển 2 module đèn giao thông (phụ trách: B)
 *
 * BẤT BIẾN AN TOÀN (safety invariant):
 *   Tại mọi thời điểm, ÍT NHẤT MỘT tuyến phải ĐỎ, hoặc cả hai cùng TẮT.
 *   => không bao giờ có 2 tuyến cùng Xanh (hay Xanh-Vàng, Vàng-Vàng...).
 *
 * Mọi lệnh xuất đèn ĐỀU phải đi qua traffic_set_pair() / traffic_apply_phase().
 * Hàm ghi chân cho từng tuyến là static (riêng của file .c) nên không ai
 * bỏ qua được bước kiểm tra này.
 */
#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <stdint.h>
#include "global.h"

typedef enum {
    TL_OFF = 0,
    TL_RED,
    TL_YELLOW,
    TL_GREEN
} tl_color_t;

/* Pha kết hợp của 2 tuyến: DÙNG KIỂU phase_t CỦA A (global.h)
 *   PHASE_ALL_RED, PHASE_R1G2, PHASE_R1Y2, PHASE_G1R2, PHASE_Y1R2   */
typedef phase_t tl_phase_t;
#define PHASE_COUNT 5

void       traffic_init(void);                                /* cả 2 tuyến ĐỎ           */
uint8_t    traffic_is_safe(tl_color_t line1, tl_color_t line2);
uint8_t    traffic_set_pair(tl_color_t line1, tl_color_t line2); /* 1 = OK, 0 = vi phạm -> ALL RED */
void       traffic_apply_phase(tl_phase_t phase);
tl_color_t traffic_get(uint8_t line);                         /* line = 1 hoặc 2          */
uint8_t    traffic_fault(void);                               /* 1 nếu đã từng vi phạm    */
void       traffic_clear_fault(void);

tl_phase_t traffic_next_phase(tl_phase_t phase);              /* chuỗi pha an toàn        */
const char *traffic_phase_name(tl_phase_t phase);             /* "R1G2"... cho LCD/debug  */

#endif /* TRAFFIC_LIGHT_H */
