#include "clock.h"
#include "fpu.h"
#include "gpio.h"
#include "interrupt_vectors.h"
#include "intrinsics.h"
#include "macros.h"
#include "nvic.h"
#include "scheduler.h"
#include "stdio.h"
#include "syscall.h"
#include "task.h"
#include "timer.h"
#include "usart.h"
#include "usart_public.h"
#include <stdint.h>

static const uint16_t pwm_prescaler	 = 12; // Lead to duty step of 800
uint16_t			  pwm_duty_steps = GET_TIMER_MAX_COUNTER_CUSTOM(10000, pwm_prescaler);

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

__attribute__((noinline)) void usart2_rw_macro_test();
__attribute__((noinline)) void poll_usart();
void						   zig_main();

int main(void)
{

	// Copy data from flash to ram
	data_section_init();
	bss_section_init();

	fpu_enable();

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

	enable_gpio_clock(GPIOA);
	enable_gpio_clock(GPIOB);
	// rcc->ahb1_enable_register.gpio_a_enable = 1;

	rcc->apb1enr.usart2_enable = 1; // byte access allowed for ahb1

	usart_config_t u2_config = {0};
	u2_config.baudrate		 = usart_baudrate_38400;
	u2_config.word_length	 = usart_word_lengths_8bits;
	u2_config.stop_bit		 = usart_stop_bits_pub_1;
	// u2_config.stop_bit				 = 1;
	u2_config.parity				 = usart_parity_none;
	u2_config.mode					 = usart_mode_both;
	u2_config.hardware_control		 = usart_hwc_none;
	u2_config.oversampling_not_over8 = usart_oversampling_16;

	config_usart(usart2, &u2_config);
	configure_rcc_timers();
	switch_to_pll(clock_frequency / 1'000'000);
	enable_timers_rcc();

	// USART2 TX pin
	gpio_port_mode_setup(GPIOA, 2, GPIO_PORT_MODE_ALTERNATE_FUNCTION);
	gpio_output_type_setup(GPIOA, 2, GPIO_PORT_OUTPUT_TYPE_PUSH_PULL);
	gpio_alternate_function_setup(GPIOA, 2, AF7); // AF7 does it so the following happen
	// A0 -> USART2 CTS
	// A1 -> USART2 RTS
	// A2 -> USART2 TX
	// A3 -> USART2 RX

	// USART2 RX pin
	gpio_port_mode_setup(GPIOA, 3, GPIO_PORT_MODE_ALTERNATE_FUNCTION);
	gpio_pull_mode_setup(GPIOA, 3, GPIO_PORT_PULL_MODE_NONE);
	gpio_alternate_function_setup(GPIOA, 3, AF7); // AF7 does it so the following happen
	// A0 -> USART2 CTS
	// A1 -> USART2 RTS
	// A2 -> USART2 TX
	// A3 -> USART2 RX

	puts("\n\n\n======================== START OF CONFIG ====================\n\n\n");

	// The blue led pin
	puts("Configuring GPIO A5 as output (The blue LED)\n");
	gpio_port_mode_setup(GPIOA, 5, GPIO_PORT_MODE_OUTPUT);
	gpio_output_type_setup(GPIOA, 5, GPIO_PORT_OUTPUT_TYPE_PUSH_PULL);

	// The button pin
	puts("Configuring GPIO B12 as input (the button)\n");
	gpio_port_mode_setup(GPIOB, 12, GPIO_PORT_MODE_INPUT);
	gpio_pull_mode_setup(GPIOB, 12, GPIO_PORT_PULL_MODE_UP);

	// use GPIO A7 for pwm
	puts("Configuring GPIO B6 as Alternate Function AF2 : Timer 4 Channel 1 (for pwm)\n");
	gpio_port_mode_setup(GPIOB, 6, GPIO_PORT_MODE_ALTERNATE_FUNCTION);
	gpio_output_type_setup(GPIOB, 6, GPIO_PORT_OUTPUT_TYPE_PUSH_PULL);
	gpio_alternate_function_setup(GPIOB, 6, AF2); // AF2 does it so the following happen
	// B6: Tim4 -> Channel 1
	// B7: Tim4 -> Channel 2
	// B8: Tim4 -> Channel 3
	// B9: Tim4 -> Channel 4

	// The task for timer 2.
	puts("Adding the task runned by the scheduler\n");
	task_add((task_public_t){.func = simple_task, .period_ms = 1000, .priority = 3});
	task_add((task_public_t){.func = simple_task2, .period_ms = 1, .priority = 10});
	task_reorder();
	// // reorder the tasks

	// uint16_t pwm_prescaler = timer_prescaler;
	puts("Configuring the timer 4 channel 1 for PWM at 10Khz with precision 1/800\n");
	configure_timerPWM(TIMER4, ch1, pwm_prescaler, pwm_duty_steps); // 10 Khz pwm
																	// set_pwm_duty(TIMER4, ch1, 1000);

	puts("Configuring timer 2 (scheduler), timer 3 (The led light changer)\n");
	configure_timer32(TIMER2, timer_prescaler, get_timer_max_counter(scheduler_frequency)); // 1 Mhz / 100 = 10 Khz
	configure_timer16(TIMER3, timer_prescaler, get_timer_max_counter(1'000));				// 1 Khz
	configure_timer32(TIMER5, timer_prescaler, get_timer_max_counter(2));					// 2 Hz

	puts("Enabling the IRQs\n");
	nvic_enable_irq(TIM2_IRQn);
	nvic_enable_irq(TIM3_IRQn);
	nvic_enable_irq(TIM5_IRQn);
	puts("Enabling USART 2 IRQ\n");
	nvic_enable_irq(USART2_IRQn);

	puts("Starting the timers\n");
	puts("4");
	start_timer(TIMER4);
	puts("2");
	start_timer(TIMER2);
	puts("3");
	start_timer(TIMER3);
	puts("5\n");
	start_timer(TIMER5);
	puts("Timer 5 started\n");

	// This basically start the scheduler

	// gpio.b->port_mode.pin12 = GPIO_PORT_MODE_OUTPUT;
#define INPUT_LETTER GPIOB
#define INPUT_PIN 12
	// gpio_port_mode_setup(INPUT_LETTER, INPUT_PIN, GPIO_PORT_MODE_INPUT);
	// gpio_pull_mode_setup(INPUT_LETTER, INPUT_PIN, GPIO_PORT_PULL_MODE_UP);

	usart2_rw_macro_test();

	int r = puts("\n\n\n================================ START OF 'UserSpace' main loop ====================\n\n\n");
	// volatile double x = get_time_s();
	zig_main();
	while (1)
	{
		uint8_t button = !gpio_read(INPUT_LETTER, INPUT_PIN);
		gpio_write(GPIOA, 5, button);
		// set_pwm_duty(TIMER4, ch1, brightness);
		// puts("Hello\r\n");
		// wait_seconds(1);

		// nvic_trigger_irq(TIM5_IRQn);

		// poll_usart();
	}
}

void poll_usart()
{

	usart_status_t status = READ32(&usart2->status);
	if (status.rxne)
	{
		usart_data_t data = READ32(&usart2->data);
		char		 c	  = data.data;
		putchar(c);
	}
}

/*
This function serves to test READ and WRITE 32
*/
void usart2_rw_macro_test(void)
{
#define MACRO_TESTER 2 // better at file scope, above the function

#if MACRO_TESTER == 1
	usart_status_t status = READ32(&usart2->status);
	(void)status;
#elif MACRO_TESTER == 2
	usart_data_t data = {0};
	data.data		  = 0xff;
	WRITE32(&usart2->data, data);
#elif MACRO_TESTER == 3
	FORCE_WRITE32(&usart2->data, (uint32_t)0xff);
#endif
}
