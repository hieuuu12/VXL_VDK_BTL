/*
 * test_main.c -- Test các module của B trên PC (không cần mạch).
 * Chạy:  cd test_pc && make
 * Mỗi "tick" giả lập = 10 ms, giống nhịp scheduler trên mạch.
 */
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "software_timer.h"
#include "button.h"
#include "traffic_light.h"
#include "fsm_automatic.h"
#include "fsm_manual.h"
#include "global.h"

static int n_pass = 0, n_fail = 0;
#define CHECK(cond, msg) do { if (cond) { n_pass++; } else { n_fail++; \
    printf("  [FAIL] %s  (dong %d)\n", msg, __LINE__); } } while (0)

/* --- tiện ích giả lập --- */
static void set_btn(button_id_t b, int pressed)   /* nút active-low: nhấn = 0 */
{
    static GPIO_TypeDef *port[] = { BTN_1_GPIO_Port, BTN_2_GPIO_Port, BTN_3_GPIO_Port, BTN_4_GPIO_Port, GPIOC };
    static uint16_t      pin[]  = { BTN_1_Pin, BTN_2_Pin, BTN_3_Pin, BTN_4_Pin, GPIO_PIN_13 };
    mock_set_input(port[b], pin[b], pressed ? 0 : 1);
}
static void tick(void) { timer_run(); button_read(); }

static void reset_all(void)
{
    timer_init(); button_init(); traffic_init();
    for (int i = 0; i < BUTTON_COUNT; i++) set_btn((button_id_t)i, 0);
    for (int i = 0; i < 5; i++) tick();
}

/* ================= 1. software_timer ================= */
static void test_timer(void)
{
    printf("[1] software_timer\n");
    timer_init();
    timer_set(TIMER_SPARE_0, 50);                       /* 5 tick */
    for (int i = 0; i < 4; i++) timer_run();
    CHECK(!timer_expired(TIMER_SPARE_0), "50 ms: chua het han sau 4 tick");
    CHECK(timer_remaining_ms(TIMER_SPARE_0) == 10, "con lai 10 ms");
    timer_run();
    CHECK(timer_expired(TIMER_SPARE_0), "50 ms: het han dung tick thu 5");
    CHECK(timer_remaining_ms(TIMER_SPARE_0) == 0, "het han -> con lai 0");

    timer_set(TIMER_SPARE_0, 15);                       /* làm tròn lên 2 tick */
    timer_run();
    CHECK(!timer_expired(TIMER_SPARE_0), "15 ms lam tron len 2 tick (chua het sau 1 tick)");
    timer_run();
    CHECK(timer_expired(TIMER_SPARE_0), "15 ms het sau 2 tick");

    timer_set(TIMER_SPARE_1, 0);
    CHECK(timer_expired(TIMER_SPARE_1), "set 0 ms -> het han ngay");

    timer_set(TIMER_SPARE_1, 100);
    timer_stop(TIMER_SPARE_1);
    for (int i = 0; i < 20; i++) timer_run();
    CHECK(!timer_expired(TIMER_SPARE_1), "timer_stop -> khong bao het han");

    timer_set(TIMER_SPARE_0, 30); timer_set(TIMER_SPARE_1, 60);
    for (int i = 0; i < 3; i++) timer_run();
    CHECK(timer_expired(TIMER_SPARE_0) && !timer_expired(TIMER_SPARE_1), "2 timer doc lap");
}

/* ================= 2. button ================= */
/* Chạy 1 chuỗi mẫu: ký tự '1' = đang nhấn, '0' = nhả; trả về số lần nhấn ghi nhận */
static int run_pattern(button_id_t b, const char *s)
{
    int presses = 0;
    for (const char *p = s; *p; p++) {
        set_btn(b, *p == '1');
        tick();
        if (button_is_pressed(b)) presses++;
    }
    return presses;
}

static void test_button(void)
{
    printf("[2] button\n");
    reset_all();
    CHECK(run_pattern(BTN_1, "0000111111110000") == 1, "nhan sach -> 1 lan");
    CHECK(run_pattern(BTN_1, "0001010110111111111000") == 1, "nhan co doi phim -> van 1 lan");
    CHECK(run_pattern(BTN_1, "0001101110111111000101000") == 1, "doi ca luc nhan va luc tha -> 1 lan");
    CHECK(run_pattern(BTN_1, "000010000") == 0, "nhieu 1 mau -> khong tinh");
    CHECK(run_pattern(BTN_1, "0000110000") == 0, "nhieu 2 mau -> khong tinh (can 3)");
    CHECK(run_pattern(BTN_1, "000111000111000111000") == 3, "3 lan nhan rieng -> 3 lan");
    CHECK(run_pattern(BTN_2, "0001111100") == 1 && run_pattern(BTN_3, "0000000") == 0, "cac nut doc lap");

    /* Giữ 1,5 s: repeat tại 1,0 s / 1,2 s / 1,4 s => 3 sự kiện */
    reset_all();
    int repeats = 0, held_seen = 0, presses = 0;
    set_btn(BTN_3, 1);
    for (int i = 0; i < 150 + 2; i++) {         /* +2 tick trễ của bộ lọc 3 mẫu */
        tick();
        if (button_is_pressed(BTN_3)) presses++;
        if (button_is_repeat(BTN_3)) repeats++;
        if (button_is_held(BTN_3)) held_seen = 1;
    }
    CHECK(presses == 1, "giu 1,5 s -> chi 1 su kien pressed");
    CHECK(repeats == 3, "giu 1,5 s -> 3 su kien repeat (1,0 / 1,2 / 1,4 s)");
    CHECK(held_seen, "giu >= 1 s -> button_is_held = 1");
    set_btn(BTN_3, 0); for (int i = 0; i < 5; i++) tick();
    CHECK(!button_is_held(BTN_3), "tha ra -> het held");

    /* Giữ 0,9 s: chưa có repeat */
    reset_all(); repeats = 0;
    set_btn(BTN_3, 1);
    for (int i = 0; i < 90; i++) { tick(); if (button_is_repeat(BTN_3)) repeats++; }
    CHECK(repeats == 0, "giu 0,9 s -> chua repeat");

    /* clear_all xoá cờ chờ */
    reset_all();
    run_pattern(BTN_4, "0001111000");       /* run_pattern đã đọc cờ, nhấn lại không đọc: */
    set_btn(BTN_4, 1); for (int i = 0; i < 5; i++) tick();
    button_clear_all();
    CHECK(!button_is_pressed(BTN_4), "button_clear_all xoa co dang cho");
}

/* ================= 3. traffic_light ================= */
static void test_traffic(void)
{
    printf("[3] traffic_light\n");
    const char *nm[] = { "OFF", "RED", "YEL", "GRN" };
    int unsafe_count = 0;
    for (int a = 0; a < 4; a++) for (int b = 0; b < 4; b++) {
        traffic_init();
        uint8_t safe = traffic_is_safe((tl_color_t)a, (tl_color_t)b);
        uint8_t ok   = traffic_set_pair((tl_color_t)a, (tl_color_t)b);
        char msg[80];
        if (safe) {
            snprintf(msg, sizeof msg, "(%s,%s) hop le -> xuat dung", nm[a], nm[b]);
            CHECK(ok && (int)traffic_get(1) == a && (int)traffic_get(2) == b && !traffic_fault()
                  && !mock_get_output(GPIOC, GPIO_PIN_9), msg);
        } else {
            unsafe_count++;
            snprintf(msg, sizeof msg, "(%s,%s) vi pham -> ALL RED + LED PC9", nm[a], nm[b]);
            CHECK(!ok && traffic_get(1) == TL_RED && traffic_get(2) == TL_RED && traffic_fault()
                  && mock_get_output(GPIOC, GPIO_PIN_9), msg);
        }
    }
    CHECK(unsafe_count == 8, "co dung 8 to hop bi cam (G/Y/OFF ghep nhau tru OFF-OFF)");
    traffic_set_pair(TL_GREEN, TL_GREEN);
    traffic_clear_fault();
    CHECK(!traffic_fault() && !mock_get_output(GPIOC, GPIO_PIN_9), "clear_fault tat LED PC9");

    /* chuỗi pha luôn đi qua vàng */
    tl_phase_t p = PHASE_ALL_RED; int ok_chain = 1;
    for (int i = 0; i < 12; i++) {
        tl_phase_t n = traffic_next_phase(p);
        if ((p == PHASE_R1G2 && n != PHASE_R1Y2) || (p == PHASE_G1R2 && n != PHASE_Y1R2)) ok_chain = 0;
        p = n;
    }
    CHECK(ok_chain, "Xanh luon chuyen sang Vang truoc");
}

/* ================= 4. fsm_automatic ================= */
static void test_fsm_auto(void)
{
    printf("[4] fsm_automatic (D5-X3-V2, 3 chu ky)\n");
    reset_all();
    g_time_red = 5; g_time_green = 3; g_time_amber = 2;
    fsm_auto_enter();

    tl_phase_t expect[] = { PHASE_R1G2, PHASE_R1Y2, PHASE_G1R2, PHASE_Y1R2 };
    int        dur_ms[] = { 3000, 2000, 3000, 2000 };
    tl_phase_t last = fsm_auto_get_phase();
    int t_last = 0, idx = 0, seq_ok = 1, dur_ok = 1, safe_ok = 1, first = 1;
    CHECK(last == PHASE_ALL_RED, "vao AUTO bat dau bang ALL_RED");

    for (int t = 10; t <= 1000 + 3 * 10000 + 10; t += 10) {
        tick();
        fsm_auto_run();
        if (!traffic_is_safe(traffic_get(1), traffic_get(2))) safe_ok = 0;
        tl_phase_t now = fsm_auto_get_phase();
        if (now != last) {
            int d = t - t_last;
            if (first) { if (d != 1000) dur_ok = 0; first = 0; }
            else if (d != dur_ms[(idx + 3) % 4]) dur_ok = 0;
            if (now != expect[idx % 4]) seq_ok = 0;
            if (now == PHASE_R1G2 && idx < 4) {
                CHECK(fsm_auto_countdown(1) == 5 && fsm_auto_countdown(2) == 3,
                      "dau R1G2: tuyen1 dem 5 (do), tuyen2 dem 3 (xanh)");
            }
            if (now == PHASE_G1R2 && idx < 4) {
                CHECK(fsm_auto_countdown(1) == 3 && fsm_auto_countdown(2) == 5,
                      "dau G1R2: tuyen1 dem 3 (xanh), tuyen2 dem 5 (do)");
            }
            idx++; last = now; t_last = t;
        }
    }
    CHECK(seq_ok, "thu tu pha R1G2 -> R1Y2 -> G1R2 -> Y1R2 lap lai");
    CHECK(dur_ok, "thoi gian moi pha dung (ALL_RED 1 s, xanh 3 s, vang 2 s)");
    CHECK(idx >= 12, "chay du >= 3 chu ky");
    CHECK(safe_ok, "khong luc nao vi pham bat bien an toan");
    CHECK(!traffic_fault(), "khong kich hoat LED loi");
    CHECK(g_phase == fsm_auto_get_phase(), "g_phase cua A duoc cap nhat dung");
}

/* ================= 5. fsm_manual ================= */
static void press_b2(void)   /* nhấn rồi thả, tổng 80 ms */
{
    set_btn(BTN_2, 1); for (int i = 0; i < 5; i++) { tick(); fsm_manual_run(); }
    set_btn(BTN_2, 0); for (int i = 0; i < 3; i++) { tick(); fsm_manual_run(); }
}
static void wait_ms(int ms) { for (int i = 0; i < ms / 10; i++) { tick(); fsm_manual_run(); } }

static void test_fsm_manual(void)
{
    printf("[5] fsm_manual\n");
    reset_all();
    traffic_apply_phase(PHASE_G1R2);                /* đang AUTO ở G1R2 */
    fsm_manual_enter(PHASE_G1R2);
    CHECK(fsm_manual_get_phase() == PHASE_G1R2 && traffic_get(1) == TL_GREEN,
          "vao MANUAL giu nguyen pha G1R2");
    press_b2();
    CHECK(fsm_manual_get_phase() == PHASE_Y1R2, "B2 -> Y1R2 (qua vang)");
    press_b2();
    CHECK(fsm_manual_get_phase() == PHASE_Y1R2, "nhan lai trong 1 s -> bi khoa, giu Y1R2");
    wait_ms(1000);
    press_b2();
    CHECK(fsm_manual_get_phase() == PHASE_R1G2, "sau 1 s -> R1G2");

    /* nhấn dồn dập 10 s: luôn an toàn, mỗi pha >= 1 s */
    reset_all();
    fsm_manual_enter(PHASE_R1G2);
    int safe_ok = 1, min_ok = 1, changes = 0, t = 0, t_last = 0;
    tl_phase_t last = fsm_manual_get_phase();
    for (int k = 0; k < 125; k++) {               /* 125 lần x 80 ms = 10 s */
        set_btn(BTN_2, 1);
        for (int i = 0; i < 8; i++) {
            if (i == 5) set_btn(BTN_2, 0);
            tick(); fsm_manual_run(); t += 10;
            if (!traffic_is_safe(traffic_get(1), traffic_get(2))) safe_ok = 0;
            if (fsm_manual_get_phase() != last) {
                if (changes > 0 && t - t_last < 1000) min_ok = 0;
                last = fsm_manual_get_phase(); t_last = t; changes++;
            }
        }
    }
    CHECK(safe_ok, "nhan don dap: khong bao gio 2 tuyen cung di");
    CHECK(min_ok,  "nhan don dap: moi pha sang toi thieu 1 s");
    CHECK(changes >= 8 && changes <= 11, "nhan don dap 10 s: ~1 lan chuyen/giay");
    CHECK(!traffic_fault(), "khong kich hoat LED loi");
}

int main(void)
{
    test_timer();
    test_button();
    test_traffic();
    test_fsm_auto();
    test_fsm_manual();
    printf("\nKET QUA: %d PASS, %d FAIL\n", n_pass, n_fail);
    return n_fail ? 1 : 0;
}
