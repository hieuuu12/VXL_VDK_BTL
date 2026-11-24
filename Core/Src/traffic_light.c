/*
 * traffic_light.c  --  Điều khiển 2 module đèn giao thông (phụ trách: B)
 */
#include "main.h"           /* HAL + User Label: TL1_A_Pin, TL1_B_Pin, TL2_A_Pin, TL2_B_Pin */
#include "traffic_light.h"

/* LED báo lỗi an toàn PC9 (có sẵn trên mạch). Nếu .ioc đặt nhãn LED_ERR thì dùng nhãn đó. */
#ifndef LED_ERR_Pin
#define LED_ERR_Pin        GPIO_PIN_9
#define LED_ERR_GPIO_Port  GPIOC
#endif

/* ====== BẢNG TỔ HỢP TÍN HIỆU -- ĐIỀN THEO KẾT QUẢ LAB 1 (mục 3.2) ======
 * Mỗi dòng: { mức chân A, mức chân B } để module hiện màu tương ứng.
 * Giá trị dưới đây CHỈ LÀ GIẢ ĐỊNH, phải sửa lại sau khi quan sát thực tế.   */
static const uint8_t TL_CODE[4][2] = {
    /* TL_OFF    */ { 0, 0 },   /* TODO lab1 */
    /* TL_RED    */ { 1, 1 },   /* TODO lab1 */
    /* TL_YELLOW */ { 1, 0 },   /* TODO lab1 */
    /* TL_GREEN  */ { 0, 1 },   /* TODO lab1 */
};
/* ====================================================================== */

static tl_color_t cur[2] = { TL_RED, TL_RED };
static uint8_t    fault  = 0;

/* Ghi chân cho 1 tuyến -- static: CHỈ được gọi từ traffic_set_pair() */
static void traffic_write_line(uint8_t line, tl_color_t c)
{
    GPIO_PinState a = TL_CODE[c][0] ? GPIO_PIN_SET : GPIO_PIN_RESET;
    GPIO_PinState b = TL_CODE[c][1] ? GPIO_PIN_SET : GPIO_PIN_RESET;
    if (line == 1) {
        HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, a);
        HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, b);
    } else {
        HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, a);
        HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, b);
    }
}

uint8_t traffic_is_safe(tl_color_t l1, tl_color_t l2)
{
    if (l1 == TL_RED || l2 == TL_RED) return 1;     /* ít nhất 1 tuyến đỏ */
    if (l1 == TL_OFF && l2 == TL_OFF) return 1;     /* cả 2 tắt (nháy)    */
    return 0;
}

uint8_t traffic_set_pair(tl_color_t l1, tl_color_t l2)
{
    uint8_t ok = 1;
    if (l1 > TL_GREEN || l2 > TL_GREEN || !traffic_is_safe(l1, l2)) {
        l1 = TL_RED;                                /* ép ALL RED */
        l2 = TL_RED;
        fault = 1;
        ok = 0;
        HAL_GPIO_WritePin(LED_ERR_GPIO_Port, LED_ERR_Pin, GPIO_PIN_SET);
    }
    /* Thứ tự ghi: tuyến nào chuyển sang ĐỎ thì ghi TRƯỚC, để trong khoảng
     * vài µs giữa 2 lệnh ghi cũng không xuất hiện trạng thái 2 tuyến cùng đi. */
    if (l2 == TL_RED && l1 != TL_RED) {
        traffic_write_line(2, l2);
        traffic_write_line(1, l1);
    } else {
        traffic_write_line(1, l1);
        traffic_write_line(2, l2);
    }
    cur[0] = l1;
    cur[1] = l2;
    return ok;
}

void traffic_init(void)
{
    fault = 0;
    HAL_GPIO_WritePin(LED_ERR_GPIO_Port, LED_ERR_Pin, GPIO_PIN_RESET);
    traffic_set_pair(TL_RED, TL_RED);
}

void traffic_apply_phase(tl_phase_t p)
{
    switch (p) {
    case PHASE_R1G2: traffic_set_pair(TL_RED,    TL_GREEN);  break;
    case PHASE_R1Y2: traffic_set_pair(TL_RED,    TL_YELLOW); break;
    case PHASE_G1R2: traffic_set_pair(TL_GREEN,  TL_RED);    break;
    case PHASE_Y1R2: traffic_set_pair(TL_YELLOW, TL_RED);    break;
    case PHASE_ALL_RED:
    default:         traffic_set_pair(TL_RED,    TL_RED);    break;
    }
}

tl_color_t traffic_get(uint8_t line)
{
    return (line == 2) ? cur[1] : cur[0];
}

uint8_t traffic_fault(void)       { return fault; }

void traffic_clear_fault(void)
{
    fault = 0;
    HAL_GPIO_WritePin(LED_ERR_GPIO_Port, LED_ERR_Pin, GPIO_PIN_RESET);
}

/* Chuỗi pha an toàn: Xanh luôn phải qua Vàng trước khi tuyến kia được Xanh */
tl_phase_t traffic_next_phase(tl_phase_t p)
{
    switch (p) {
    case PHASE_ALL_RED: return PHASE_R1G2;
    case PHASE_R1G2:    return PHASE_R1Y2;
    case PHASE_R1Y2:    return PHASE_G1R2;
    case PHASE_G1R2:    return PHASE_Y1R2;
    case PHASE_Y1R2:    return PHASE_R1G2;
    default:            return PHASE_ALL_RED;
    }
}

const char *traffic_phase_name(tl_phase_t p)
{
    static const char *names[PHASE_COUNT] = { "ALLRED", "R1G2", "R1Y2", "G1R2", "Y1R2" };
    return (p < PHASE_COUNT) ? names[p] : "?";
}
