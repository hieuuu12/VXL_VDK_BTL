#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>

#define TIME_RED_DEFAULT 5
#define TIME_GREEN_DEFAULT 3
#define TIME_AMBER_DEFAULT 2
#define TIME_MIN 1
#define TIME_MAX 99

typedef enum { MODE_INIT, MODE_AUTO, MODE_MANUAL, MODE_TUNING } app_mode_t;
typedef enum { PHASE_ALL_RED, PHASE_R1G2, PHASE_R1Y2, PHASE_G1R2, PHASE_Y1R2 } phase_t;

extern app_mode_t g_mode;
extern phase_t g_phase;
extern uint8_t g_time_red;
extern uint8_t g_time_green;
extern uint8_t g_time_amber;

#endif
