/* STUB theo hop dong API muc 2.6 (file that cua B). */
#ifndef TRAFFIC_LIGHT_H_
#define TRAFFIC_LIGHT_H_
#include <stdint.h>
typedef enum { TL_OFF, TL_RED, TL_YELLOW, TL_GREEN } tl_color_t;
uint8_t traffic_set_pair(tl_color_t r1, tl_color_t r2);
tl_color_t traffic_get(uint8_t route);
#endif
