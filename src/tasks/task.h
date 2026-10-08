#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_TASK 16

typedef struct
{
	void (*func)(void);
	uint32_t frequency_divider;
	/* We want the task to be run each N ms. So we need to know what the timer frequency is.
	   f = 1 / (T * 0.001) = 1000 / T

	   frequency_divider = Timer Frequency / task frequency
	   Ie:
	   50 Khz / 1 Khz
	   frequency_divider= 50

	*/
	uint32_t last_run_tick;
	uint8_t	 priority;
	bool	 ready;

} task_t;

typedef struct
{
	void (*func)(void);
	double period_ms;
	/* We want the task to be run each N ms. So we need to know what the timer frequency is.
	   f = 1 / (T * 0.001) = 1000 / T

	   frequency_divider = Timer Frequency / task frequency
	   Ie:
	   50 Khz / 1 Khz
	   frequency_divider= 50

	*/
	uint8_t priority;

} task_public_t;

typedef struct
{
	task_t	tasks[16];
	uint8_t task_count;
} task_list_t;

extern task_list_t task_list;

// The functions :
// task_t task_create(task_public_t data);
bool task_add(task_public_t task);
void task_reorder();

void simple_task();
void simple_task2();
