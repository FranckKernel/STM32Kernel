#include "timer.h"
volatile timer_registers_t *const timers[] = {
	(volatile timer_registers_t *)TIMER2_MMIO_BASE,
	(volatile timer_registers_t *)TIMER3_MMIO_BASE,
	(volatile timer_registers_t *)TIMER4_MMIO_BASE,
	(volatile timer_registers_t *)TIMER5_MMIO_BASE,
};

void configure_timer2()
{
	// Timer 2 initial clock = 96 Mhz. Timer 2 must be enabled in rcc
	timers[TIMER2]->psc.prescaler_value = 96 - 1;  // Now 1 Mhz
	timers[TIMER2]->arr.arr32			= 100 - 1; // Now 10 Khz

	timers[TIMER2]->egr.update_generation		= 1;
	timers[TIMER2]->sr.update_interrupt_pending = 0;

	timers[TIMER2]->dier.update_interrupt_enable = 1;

	timers[TIMER2]->cr1.counter_enable = 1;
}
