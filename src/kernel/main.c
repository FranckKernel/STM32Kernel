#include "clock.h"
#include "flash.h"
#include "gpio.h"
#include "interrupt_vectors.h"
#include "intrinsics.h"
#include "nvic.h"
#include "scheduler.h"
#include "syscall.h"
#include "task.h"
#include "timer.h"
#include <stdint.h>

#ifdef USE_LIBC
#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>
#endif

void wait_seconds(float seconds)
{

	uint32_t cycles = (uint32_t)(seconds * clock_frequency / 4);

	while (cycles--)
	{
		nop();
	}
}

#define RCC_AHB1ENR (*(volatile uint32_t *)(0x40023800 + 0x30))
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)
#define GPIOA_BSRR (*(volatile uint32_t *)0x40020018)

void start_of_loop()
{
}

void in_the_loop()
{
	__asm volatile("nop");
}

extern uint32_t __data_image_start;
extern uint32_t __data_start;
extern uint32_t _edata;

void data_section_init(void)
{
	uint32_t *src = &__data_image_start;
	uint32_t *dst = &__data_start;

	while (dst < &_edata)
	{
		*dst++ = *src++;
	}
}

extern uint32_t __bss_start;
extern uint32_t __bss_end;

void bss_section_init(void)
{
	extern uint32_t __bss_start, __bss_end;
	for (uint32_t *p = &__bss_start; p < &__bss_end; p++)
		*p = 0;
}

// extern that shit so it's available someplace elses

int main(void)
{

	// Copy data from flash to ram
	data_section_init();
	bss_section_init();

	enable_gpio_clock(GPIOA);
	enable_gpio_clock(GPIOB);

#ifdef USE_LIBC

	char array[50];
	int	 b[] = {1, 2, 3, 4, 5, 6};
	memcpy(array, b, 4);

#	ifdef CALL_PRINT_MALOC
#		error currently not working

	// LIBC call breaks everything
	// printf("ABC is working %d\n", 27);
	// char *arr = malloc(100); // This line cause crash
#	endif
#endif

	// gpio_struct.a->port_mode.pin5 = GPIO_PORT_MODE_OUTPUT;
	// gpio[GPIOA]->port_mode.pin5 = GPIO_PORT_MODE_OUTPUT;
	gpio_port_mode_setup(GPIOA, 5, GPIO_PORT_MODE_OUTPUT);
	gpio_output_type_setup(GPIOA, 5, GPIO_PORT_OUTPUT_TYPE_PUSH_PULL);

	// use GPIO A7 for pwm
	gpio_port_mode_setup(GPIOB, 6, GPIO_PORT_MODE_ALTERNATE_FUNCTION);
	gpio_output_type_setup(GPIOB, 6, GPIO_PORT_OUTPUT_TYPE_PUSH_PULL);
	gpio_alternate_function_setup(GPIOB, 6, AF2); // AF2 does it so the following happen
	// B6: Tim4 -> Channel 1
	// B7: Tim4 -> Channel 2
	// B8: Tim4 -> Channel 3
	// B9: Tim4 -> Channel 4

	gpio_port_mode_setup(GPIOB, 12, GPIO_PORT_MODE_INPUT);
	gpio_pull_mode_setup(GPIOB, 12, GPIO_PORT_PULL_MODE_UP);

	// The task for timer 2.
	task_add((task_public_t){.func = simple_task, .period_ms = 4, .priority = 3});
	task_add((task_public_t){.func = simple_task2, .period_ms = 0.1, .priority = 10});
	task_reorder();
	// // reorder the tasks

	// nvic_trigger_irq(TIM2_IRQn);

	configure_rcc_timers();
	switch_to_pll(clock_frequency / 1'000'000);
	enable_timers_rcc();

	// Need customer values if i want something else
	// uint16_t pwm_prescaler = timer_prescaler;
	static const uint16_t pwm_prescaler	 = 12; // Lead to duty step of 800
	uint16_t			  pwm_duty_steps = GET_TIMER_MAX_COUNTER_CUSTOM(10000, pwm_prescaler);
	configure_timerPWM(TIMER4, ch1, pwm_prescaler, pwm_duty_steps); // 10 Khz pwm
																	// set_pwm_duty(TIMER4, ch1, 1000);

	configure_timer32(TIMER2, timer_prescaler, get_timer_max_counter(scheduler_frequency)); // 1 Mhz / 100 = 10 Khz
	configure_timer16(TIMER3, timer_prescaler, get_timer_max_counter(1'000));				// 1 Khz
	configure_timer32(TIMER5, timer_prescaler, get_timer_max_counter(2));					// 2 Hz

	nvic_enable_irq(TIM2_IRQn);
	nvic_enable_irq(TIM3_IRQn);
	nvic_enable_irq(TIM5_IRQn);

	start_timer(TIMER4);
	start_timer(TIMER2);
	start_timer(TIMER3);
	start_timer(TIMER5);

	// This basically start the scheduler

	// gpio.b->port_mode.pin12 = GPIO_PORT_MODE_OUTPUT;
#define INPUT_LETTER GPIOB
#define INPUT_PIN 12
	// gpio_port_mode_setup(INPUT_LETTER, INPUT_PIN, GPIO_PORT_MODE_INPUT);
	// gpio_pull_mode_setup(INPUT_LETTER, INPUT_PIN, GPIO_PORT_PULL_MODE_UP);

	while (1)
	{
		// uint8_t button = !gpio_read(INPUT_LETTER, INPUT_PIN);
		// gpio_write(GPIOB, 6, 1);
		// set_pwm_duty(TIMER4, ch1, brightness);
		// wait_seconds(0.001);

		// nvic_trigger_irq(TIM5_IRQn);
	}
}
