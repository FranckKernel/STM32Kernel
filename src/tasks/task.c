#include "task.h"

uint32_t timer_frequency = 50'000;

task_t task_create(task_public_t data)
{
	double period_ms = data.period_ms;

	uint32_t task_frequency = 1 / period_ms;
	uint32_t divider		= timer_frequency / task_frequency;

	task_t task = {.func = data.func, .priority = data.priority, .frequency_divider = divider, .last_run_tick = 0, .ready = false};
	return task;
}

bool task_add(task_public_t data)
{
	task_t	task = task_create(data);
	uint8_t c	 = task_list.task_count;
	c++;
	if (c > MAX_TASK)
	{
		return false;
	}
	task_list.task_count = c;
	task_list.tasks[c]	 = task;

	return true;
}

void task_swap(task_t *t1, task_t *t2)
{
	task_t copy = *t1;
	*t1			= *t2;
	*t2			= copy;
}

void task_reorder()
{
	// We want highest priority first in the list
	// Bubble sort
	// We only call this once, at setup time.
	// Technically, preferably, this would even be compile time

	if (task_list.task_count < 2)
		return;

	uint8_t i		 = 0;
	bool	full_run = true;
	while (true)
	{
		task_t now	= task_list.tasks[i];
		task_t next = task_list.tasks[i + 1];

		if (now.priority < next.priority)
		{
			task_swap(&task_list.tasks[i], &task_list.tasks[i + 1]);
			full_run = false;
		}
		i++;

		if (i == task_list.task_count - 1)
		{
			if (full_run == true)
			{
				break;
			}
			else
			{
				full_run = true;
				i		 = 0;
			}
		}
	}
}

void simple_task()
{
	// do something
}

void simple_task2()
{
	// do something
}
