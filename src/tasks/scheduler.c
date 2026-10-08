#include "scheduler.h"
#include "task.h"

uint8_t task_running[(MAX_TASK + sizeof(uint8_t) - 1) / sizeof(uint8_t)];
bool	some_task_is_running = false;

task_list_t task_list = {.task_count = 0, .tasks = {0}};

uint32_t scheduler_tick = 0;

void Timer2Handler(void)
{
	// This is my simple scheduler.
	// Have a list of task. Each task has a function ptr, a frequency, and a priority
	// We also need to have a timer counter.
	// Task are preempted by the scheduler interrupt.
	// But they are not pre
	if (scheduler_tick == 0)
	{
		// for first run
		for (uint8_t i = 0; i < task_list.task_count; i++)
		{
			task_list.tasks[i].ready = true;
		}
	}

	scheduler_tick++;

	if (!some_task_is_running)
	{
		for (uint8_t i = 0; i < task_list.task_count; i++)
		{
			task_t	 task = task_list.tasks[i];
			uint32_t diff = scheduler_tick - task.last_run_tick;
			if (diff > task.frequency_divider || task.ready == true)
			{
				task_list.tasks[i].last_run_tick = scheduler_tick;
				task_list.tasks[i].ready		 = false;
				task_running[i]					 = true;
				some_task_is_running			 = true;
				task.func();

				task_running[i]		 = false;
				some_task_is_running = false;
			}
		}
	}
}

// This version is closer to how it is done.
// No need for a ready list, task are already sorted
// Else, a priority list could be created
// I also didn't use the task_running because, it will be false when its not.
// So when in another task or in main, it will be false.
// So what is the point of knowing itself, when it only happen on itself
void Timer2HandlerSlower(void)
{
	// This is my simple scheduler.
	// Have a list of task. Each task has a function ptr, a frequency, and a priority
	// We also need to have a timer counter.
	// Task are preempted by the scheduler interrupt.
	// But they are not pre
	scheduler_tick++;

	for (uint8_t i = 0; i < task_list.task_count; i++)
	{
		task_t	 task = task_list.tasks[i];
		uint32_t diff = scheduler_tick - task.last_run_tick;
		if (diff > task.frequency_divider)
		{
			task_list.tasks[i].ready = true;
		}
	}

	if (!some_task_is_running)
	{
		for (uint8_t i = 0; i < task_list.task_count; i++)
		{
			task_t task = task_list.tasks[i];
			if (task.ready == true)
			{
				task_list.tasks[i].last_run_tick = scheduler_tick;
				some_task_is_running			 = true;
				task.func();

				some_task_is_running = false;
			}
		}
	}
}
