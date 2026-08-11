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

void wait_seconds(float seconds)
{
	uint32_t cycles = (uint32_t)(seconds * 1000000.0f);

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

int main(void)
{

	RCC_AHB1ENR |= (1 << 0); // enable GPIOA clock (bit 0)

	// wait_seconds(1);

	char array[50];
	int	 b[] = {1, 2, 3, 4, 5, 6};

	memcpy(array, b, 4);
	printf("ABC is working %d\n", 27);

	gpio_setup(GPIO_PORT_LETTER_A, 5, GPIO_PORT_MODE_OUTPUT);
	gpio_setup(GPIO_PORT_LETTER_D, 12, GPIO_PORT_MODE_OUTPUT); // renode led

	// Configure PA5 as output
	// GPIOA_MODER &= ~(0x3 << 10);
	// GPIOA_MODER |= (0x1 << 10);

	// Set PA5 high (direct write)
	// GPIOA_BSRR = (1 << 5);

	// wait_seconds(3);

	start_of_loop();
	while (1)
	{
		// wait_seconds(1);
		gpio_write(GPIO_PORT_LETTER_A, 5, 1);
		gpio_write(GPIO_PORT_LETTER_D, 12, 1);
		wait_seconds(1);
		in_the_loop();

		gpio_write(GPIO_PORT_LETTER_A, 5, 0);
		gpio_write(GPIO_PORT_LETTER_D, 12, 0);
		wait_seconds(1);
		// in_the_loop();
	}

	// reg_setup();
}
