#include "timer.h"
volatile timer_registers_t *const timers[] = {
	(volatile timer_registers_t *)TIMER2_MMIO_BASE,
	(volatile timer_registers_t *)TIMER3_MMIO_BASE,
	(volatile timer_registers_t *)TIMER4_MMIO_BASE,
	(volatile timer_registers_t *)TIMER5_MMIO_BASE,
};

void Timer2Handler(void)
{
}

void configure_timer2()
{
}
