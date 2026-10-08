#include "task.h"

bool task_add(task_t task)
{
	uint8_t c = task_list.task_count;
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
