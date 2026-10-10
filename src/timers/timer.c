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
}

void start_timer(enum TIMER_NUMBER timer_number)
{

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

	// timers[timer_number]->cr1.counter_enable = 1;
	// moved to start timer
}

void configure_timerPWM(enum TIMER_NUMBER timer_number, enum TIMER_CHANNEL channel, uint16_t prescaler_value, uint32_t max_timer_value)
{
	if ((timer_number == TIMER3) || ((timer_number == TIMER4) && (max_timer_value > 0xFFFF)))
	{
		// Error, arr value too big
	}

	// Timer 2 initial clock = 96 Mhz. Timer 2 must be enabled in rcc
	timers[timer_number]->psc.prescaler_value = prescaler_value - 1; // Now 1 Mhz
	timers[timer_number]->arr.arr16			  = max_timer_value - 1; // Now 10 Khz

	switch (channel)
	{
	case ch1:
	{
		timers[timer_number]->ccmr1.output.cc1_selection				  = cc1_output; // for channel 1
		timers[timer_number]->ccmr1.output.output_compare1_mode			  = ocm_pwm_mode_active_ccr1_over_arr;
		timers[timer_number]->ccmr1.output.output_compare1_preload_enable = 1;

		// timers[timer_number]->ccer.raw = 0b1;
		timers[timer_number]->ccer.output.cc1_enable   = 1;
		timers[timer_number]->ccer.output.cc1_polarity = ccxp_out_active_high;

		timers[timer_number]->ccr1.full32 = 0;

		break;
	}
	case ch2:
	{
		timers[timer_number]->ccmr1.output.cc2_selection				  = cc2_output; // for channel 1
		timers[timer_number]->ccmr1.output.output_compare2_mode			  = ocm_pwm_mode_active_ccr1_over_arr;
		timers[timer_number]->ccmr1.output.output_compare2_preload_enable = 1;

		timers[timer_number]->ccer.output.cc2_enable   = 1;
		timers[timer_number]->ccer.output.cc2_polarity = ccxp_out_active_high;

		timers[timer_number]->ccr2.full32 = 0;

		break;
	}
	case ch3:
	{
		timers[timer_number]->ccmr2.output.cc3_selection				  = cc3_output; // for channel 1
		timers[timer_number]->ccmr2.output.output_compare3_mode			  = ocm_pwm_mode_active_ccr1_over_arr;
		timers[timer_number]->ccmr2.output.output_compare3_preload_enable = 1;

		timers[timer_number]->ccer.output.cc3_enable   = 1;
		timers[timer_number]->ccer.output.cc3_polarity = ccxp_out_active_high;

		timers[timer_number]->ccr3.full32 = 0;

		break;
	}
	case ch4:
	{
		timers[timer_number]->ccmr2.output.cc4_selection				  = cc4_output; // for channel 1
		timers[timer_number]->ccmr2.output.output_compare4_mode			  = ocm_pwm_mode_active_ccr1_over_arr;
		timers[timer_number]->ccmr2.output.output_compare4_preload_enable = 1;

		timers[timer_number]->ccer.output.cc4_enable   = 1;
		timers[timer_number]->ccer.output.cc4_polarity = ccxp_out_active_high;

		timers[timer_number]->ccr4.full32 = 0;

		break;
	}
	}

	timers[timer_number]->egr.update_generation		  = 1;
	timers[timer_number]->sr.update_interrupt_pending = 0;
}

void set_pwm_duty(enum TIMER_NUMBER timer_number, enum TIMER_CHANNEL channel, uint32_t value)
{

	if ((timer_number == TIMER3) || ((timer_number == TIMER4) && (value > 0xFFFF)))
	{
		// Error, arr value too big
	}
	switch (channel)
	{
	case ch1: timers[timer_number]->ccr1.full32 = value; break;
	case ch2: timers[timer_number]->ccr2.full32 = value; break;
	case ch3: timers[timer_number]->ccr3.full32 = value; break;
	case ch4: timers[timer_number]->ccr4.full32 = value; break;
	}
}
