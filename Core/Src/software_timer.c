/*
 * software_timer.c  --  Bộ đếm thời gian phần mềm (phụ trách: B)
 * Không gọi HAL => test được trên PC.
 */
#include "software_timer.h"

typedef struct {
    uint32_t counter;   /* số tick còn lại          */
    uint8_t  running;   /* 1 = đang đếm             */
    uint8_t  flag;      /* 1 = đã hết hạn           */
} soft_timer_t;

static soft_timer_t timers[TIMER_COUNT];

void timer_init(void)
{
    for (int i = 0; i < TIMER_COUNT; i++) {
        timers[i].counter = 0;
        timers[i].running = 0;
        timers[i].flag    = 0;
    }
}

void timer_set(timer_id_t id, uint32_t duration_ms)
{
    if (id >= TIMER_COUNT) return;
    /* làm tròn LÊN theo tick để không bao giờ ngắn hơn yêu cầu */
    uint32_t ticks = (duration_ms + TIMER_TICK_MS - 1u) / TIMER_TICK_MS;
    timers[id].flag    = 0;
    if (ticks == 0) {               /* thời gian 0 => hết hạn ngay */
        timers[id].counter = 0;
        timers[id].running = 0;
        timers[id].flag    = 1;
    } else {
        timers[id].counter = ticks;
        timers[id].running = 1;
    }
}

void timer_stop(timer_id_t id)
{
    if (id >= TIMER_COUNT) return;
    timers[id].running = 0;
    timers[id].counter = 0;
    timers[id].flag    = 0;
}

uint8_t timer_expired(timer_id_t id)
{
    if (id >= TIMER_COUNT) return 0;
    return timers[id].flag;
}

uint32_t timer_remaining_ms(timer_id_t id)
{
    if (id >= TIMER_COUNT) return 0;
    return timers[id].running ? timers[id].counter * TIMER_TICK_MS : 0;
}

void timer_run(void)
{
    for (int i = 0; i < TIMER_COUNT; i++) {
        if (timers[i].running) {
            if (timers[i].counter > 0) timers[i].counter--;
            if (timers[i].counter == 0) {
                timers[i].running = 0;
                timers[i].flag    = 1;
            }
        }
    }
}
