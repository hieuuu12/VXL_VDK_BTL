#include "main.h"
#include "scheduler.h"

sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t SCH_count = 0;
uint32_t SCH_late = 0;
uint32_t SCH_next_id = 0;

static void SCH_Insert(void (*pFunc)(void), uint32_t delay, uint32_t period, uint32_t id) {
	uint8_t i = 0;
	uint8_t j;
	while (i < SCH_count && delay >= SCH_tasks_G[i].Delay) {
		delay -= SCH_tasks_G[i].Delay;
		i++;
	}
	for (j = SCH_count; j > i; j--) {
		SCH_tasks_G[j] = SCH_tasks_G[j - 1];
	}
	SCH_tasks_G[i].pTask = pFunc;
	SCH_tasks_G[i].Delay = delay;
	SCH_tasks_G[i].Period = period;
	SCH_tasks_G[i].RunMe = 0;
	SCH_tasks_G[i].TaskID = id;
	if (i < SCH_count) {
		SCH_tasks_G[i + 1].Delay -= delay;
	}
	SCH_count++;
}

static void SCH_Remove(uint8_t i) {
	uint8_t j;
	if (i + 1 < SCH_count) {
		SCH_tasks_G[i + 1].Delay += SCH_tasks_G[i].Delay;
	}
	for (j = i; j + 1 < SCH_count; j++) {
		SCH_tasks_G[j] = SCH_tasks_G[j + 1];
	}
	SCH_count--;
}

static void SCH_Fix_Head(void) {
	if (SCH_count == 0) {
		SCH_late = 0;
		return;
	}
	while (SCH_late > 0 && SCH_tasks_G[0].Delay > 0) {
		SCH_tasks_G[0].Delay--;
		SCH_late--;
	}
	if (SCH_tasks_G[0].Delay == 0) {
		SCH_tasks_G[0].RunMe = 1;
	}
}

void SCH_Init(void) {
	SCH_count = 0;
	SCH_late = 0;
	SCH_next_id = 0;
}

uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD) {
	uint32_t id;
	if (pFunc == 0) return NO_TASK_ID;
	__disable_irq();
	if (SCH_count >= SCH_MAX_TASKS) {
		__enable_irq();
		return NO_TASK_ID;
	}
	SCH_next_id++;
	if (SCH_next_id == NO_TASK_ID) SCH_next_id++;
	id = SCH_next_id;
	SCH_Insert(pFunc, DELAY + SCH_late, PERIOD, id);
	SCH_Fix_Head();
	__enable_irq();
	return id;
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
	uint8_t i;
	if (taskID == NO_TASK_ID) return 0;
	__disable_irq();
	for (i = 0; i < SCH_count; i++) {
		if (SCH_tasks_G[i].TaskID == taskID) {
			SCH_Remove(i);
			SCH_Fix_Head();
			__enable_irq();
			return 1;
		}
	}
	__enable_irq();
	return 0;
}

void SCH_Update(void) {
	if (SCH_count == 0) return;
	if (SCH_tasks_G[0].Delay > 0) {
		SCH_tasks_G[0].Delay--;
		if (SCH_tasks_G[0].Delay == 0) SCH_tasks_G[0].RunMe = 1;
	} else {
		SCH_late++;
	}
}

void SCH_Dispatch_Tasks(void) {
	sTask t;
	while (1) {
		__disable_irq();
		if (SCH_count == 0 || SCH_tasks_G[0].RunMe == 0) {
			__enable_irq();
			return;
		}
		t = SCH_tasks_G[0];
		SCH_Remove(0);
		if (t.Period > 0) {
			SCH_Insert(t.pTask, t.Period, t.Period, t.TaskID);
		}
		SCH_Fix_Head();
		__enable_irq();
		t.pTask();
	}
}
