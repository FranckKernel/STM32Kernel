#include "timer.h"
volatile timer_registers_t *const timers[] = {
	(volatile timer_registers_t *)TIMER2_MMIO_BASE,
	(volatile timer_registers_t *)TIMER3_MMIO_BASE,
	(volatile timer_registers_t *)TIMER4_MMIO_BASE,
	(volatile timer_registers_t *)TIMER5_MMIO_BASE,
};

void configure_timer32(enum TIMER_NUMBER timer_number, uint16_t prescaler_value, uint32_t arr)
{
	// Timer 2 initial clock = 96 Mhz. Timer 2 must be enabled in rcc
	timers[timer_number]->psc.prescaler_value = prescaler_value - 1; // Now 1 Mhz
	timers[timer_number]->arr.arr32			  = arr - 1;			 // Now 10 Khz

	timers[timer_number]->egr.update_generation		  = 1;
	timers[timer_number]->sr.update_interrupt_pending = 0;

	timers[timer_number]->dier.update_interrupt_enable = 1;

	timers[timer_number]->cr1.counter_enable = 1;
}

void configure_timer16(enum TIMER_NUMBER timer_number, uint16_t prescaler_value, uint16_t arr)
{
	// Timer 2 initial clock = 96 Mhz. Timer 2 must be enabled in rcc
	timers[timer_number]->psc.prescaler_value = prescaler_value - 1; // Now 1 Mhz
	timers[timer_number]->arr.arr16			  = arr - 1;			 // Now 10 Khz

	timers[timer_number]->egr.update_generation		  = 1;
	timers[timer_number]->sr.update_interrupt_pending = 0;

	timers[timer_number]->dier.update_interrupt_enable = 1;

	timers[timer_number]->cr1.counter_enable = 1;
}
