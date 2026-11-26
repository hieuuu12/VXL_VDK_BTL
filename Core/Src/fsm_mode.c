#include "fsm_mode.h"
#include "global.h"
#include "scheduler.h"
#include "button.h"
#include "traffic_light.h"
#include "fsm_automatic.h"
#include "fsm_manual.h"
#include "fsm_tuning.h"

#define BLINK_TICKS 50

static uint32_t blink_id = NO_TASK_ID;
static uint8_t blink_on = 0;

static void blink_red(void) {
	if (blink_on == 1) {
		traffic_set_pair(TL_OFF, TL_OFF);
		blink_on = 0;
	} else {
		traffic_set_pair(TL_RED, TL_RED);
		blink_on = 1;
	}
}

static void enter_auto(void) {
	g_mode = MODE_AUTO;
	fsm_auto_enter();
}

static void enter_manual(void) {
	g_mode = MODE_MANUAL;
	fsm_manual_enter(fsm_auto_get_phase());
}

static void enter_tuning(void) {
	g_mode = MODE_TUNING;
	fsm_tuning_enter();
	traffic_set_pair(TL_RED, TL_RED);
	blink_on = 1;
	SCH_Delete_Task(blink_id);
	blink_id = SCH_Add_Task(blink_red, BLINK_TICKS, BLINK_TICKS);
}

static void exit_tuning(void) {
	SCH_Delete_Task(blink_id);
	blink_id = NO_TASK_ID;
	blink_on = 0;
}

void fsm_mode_init(void) {
	g_time_red = TIME_RED_DEFAULT;
	g_time_green = TIME_GREEN_DEFAULT;
	g_time_amber = TIME_AMBER_DEFAULT;
	button_clear_all();
	enter_auto();
}

void fsm_mode_run(void) {
	switch (g_mode) {
	case MODE_AUTO:
		if (button_is_pressed(BTN_1)) {
			button_clear_all();
			enter_manual();
		} else {
			fsm_auto_run();
		}
		break;
	case MODE_MANUAL:
		if (button_is_pressed(BTN_1)) {
			button_clear_all();
			enter_tuning();
		} else {
			fsm_manual_run();
		}
		break;
	case MODE_TUNING:
		if (button_is_pressed(BTN_1)) {
			button_clear_all();
			exit_tuning();
			enter_auto();
		} else {
			fsm_tuning_run();
		}
		break;
	default:
		button_clear_all();
		enter_auto();
		break;
	}
}
