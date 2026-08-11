#include "gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile uint32_t counter = 0;

static inline void nop(void)
{
	__asm volatile("nop");
}

uint32_t clock_frequency = 16 * 1000 * 1000;

void wait_seconds(float seconds)
{

	uint32_t cycles = (uint32_t)(seconds * clock_frequency / 4);

	while (cycles--)
	{
		nop();
	}
}

#define RCC_AHB1ENR (*(volatile uint32_t *)0x40023830)
#define GPIOA_MODER (*(volatile uint32_t *)0x40020000)
#define GPIOA_BSRR (*(volatile uint32_t *)0x40020018)

void start_of_loop()
{
}

void in_the_loop()
{
	__asm volatile("nop");
}

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;

void data_section_init(void)
{
	uint32_t *src = &_sidata;
	uint32_t *dst = &_sdata;

	while (dst < &_edata)
	{
		*dst++ = *src++;
	}
}

int main(void)
{

	data_section_init();

	RCC_AHB1ENR |= (1 << 0); // enable GPIOA clock (bit 0)
	RCC_AHB1ENR |= (1 << 1);

	// wait_seconds(1);

	char array[50];
	int	 b[] = {1, 2, 3, 4, 5, 6};

	// memcpy(array, b, 4);
	// printf("ABC is working %d\n", 27);

	// gpio_port_mode_setup(GPIOA, 5, GPIO_PORT_MODE_OUTPUT);
	// gpio.a->port_mode.pin5 = GPIO_PORT_MODE_OUTPUT;
	gpio_port_mode_setup(GPIOA, 5, GPIO_PORT_MODE_OUTPUT);
	gpio_output_type_setup(GPIOA, 5, GPIO_PORT_OUTPUT_TYPE_PUSH_PULL);

	gpio_port_mode_setup(GPIOB, 12, GPIO_PORT_MODE_INPUT);
	gpio_pull_mode_setup(GPIOB, 12, GPIO_PORT_PULL_MODE_UP);

	// gpio.b->port_mode.pin12 = GPIO_PORT_MODE_OUTPUT;
#define INPUT_LETTER GPIOB
#define INPUT_PIN 12
	// gpio_port_mode_setup(INPUT_LETTER, INPUT_PIN, GPIO_PORT_MODE_INPUT);
	// gpio_pull_mode_setup(INPUT_LETTER, INPUT_PIN, GPIO_PORT_PULL_MODE_UP);

	while (1)
	{
		uint8_t button = !gpio_read(INPUT_LETTER, INPUT_PIN);
		gpio_write(GPIOA, 5, button);
	}
}
