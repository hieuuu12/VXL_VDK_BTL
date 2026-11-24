/* test_globals.c -- Biến chung cho test PC (trên mạch dùng global.c của A) */
#include "global.h"
app_mode_t g_mode = MODE_AUTO;
phase_t    g_phase = PHASE_ALL_RED;
uint8_t    g_time_red   = TIME_RED_DEFAULT;
uint8_t    g_time_green = TIME_GREEN_DEFAULT;
uint8_t    g_time_amber = TIME_AMBER_DEFAULT;
