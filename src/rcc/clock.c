#include "clock.h"
#include "gpio_types.h"

volatile rcc_register_t *const rcc = (volatile rcc_register_t *const)RCC_MMIO_ADDRESS_BASE;

void enable_gpio_clock(enum GPIO_PORT_LETTER letter)
{
	volatile rcc_ahb1enr_t *const enable_register	  = &rcc->ahb1_enable_register;
	volatile uint32_t *const	  enable_register_raw = (volatile uint32_t *const)enable_register;
	*enable_register_raw |= (1 << letter);

	if (false)
	{
		// known example that works
		enable_register->gpio_a_enable = true;
		enable_register->gpio_b_enable = true;
		// In zig, we could have a gpio_enable array. Where you do rcc->rcc_ahb1_enable_register.gpio_enable[.a] = 1;
	}
}
