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
		// for first run or uint32_t overflow.
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
				task.func(); // The tasks are simple. They are the driver.
				// And they put some information available somewhere

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
			// Then, we would add this task to a ready list
		}
	}
	// Work on the wait lists ????
	// Wait on condition variable, semaphore/mutex.
	// Wait for time.
	// and depending on condition, remove from wait list, and put into ready list

	// Here we would dequeue from the ready list to get the task
	// then we would change cr3, change the register
	// and change the kernel stack pointer to the ksp of this task
	// because when this task got the scheduler interrupt called,
	// it had it's own sp. And on sp, it pushed the return address, the ss and cs of what it cames from, and the flags.
	// doing iret will undo this.

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

	// on x86, we would exit the scheduler with iret
	// Before enabling the scheduler, we would create a init process. (the creation puts it on the queue).
	// And put a flag to first entry to true, then start the scheduler.
	// if it's the first entry, we would set first entry to false.
	// And then, we would need to manually consturct the iret frame.
	// and iret to it
}
