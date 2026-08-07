#include "gpio.h"

static volatile gpio_x_register_t *const GPIOA_Register = (gpio_x_register_t *)GPIO_MMIO_A_BASE;
static volatile gpio_x_register_t *const GPIOB_Register = (gpio_x_register_t *)GPIO_MMIO_B_BASE;
static volatile gpio_x_register_t *const GPIOC_Register = (gpio_x_register_t *)GPIO_MMIO_C_BASE;
static volatile gpio_x_register_t *const GPIOD_Register = (gpio_x_register_t *)GPIO_MMIO_D_BASE;
static volatile gpio_x_register_t *const GPIOE_Register = (gpio_x_register_t *)GPIO_MMIO_E_BASE;
static volatile gpio_x_register_t *const GPIOF_Register = (gpio_x_register_t *)GPIO_MMIO_F_BASE;
static volatile gpio_x_register_t *const GPIOG_Register = (gpio_x_register_t *)GPIO_MMIO_G_BASE;

struct GPIO_REGISTERS gpio = {
	.a = GPIOA_Register,
	.b = GPIOB_Register,
	.c = GPIOC_Register,
	.d = GPIOD_Register,
	.e = GPIOE_Register,
	.f = GPIOF_Register,
	.g = GPIOG_Register,
};

void gpio_setup(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_PORT_MODE mode)
{
	uint32_t		   letter_base	  = (GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF);
	uint32_t		   port_mode_addr = letter_base + GPIO_PORT_MODE_ADDRESS_OFFSET;
	volatile uint32_t *port_modes	  = (volatile uint32_t *)port_mode_addr;

	uint32_t port_modes_val = *port_modes;
	port_modes_val &= ~(0b11 << (2 * pin));
	port_modes_val |= ((uint32_t)mode << (2 * pin));

	*port_modes = port_modes_val;
}

void gpio_write(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_PORT_OUTPUT_DATA level)
{
	uint32_t		   letter_base			   = (GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF);
	uint32_t		   port_bit_set_reset_addr = letter_base + GPIO_PORT_BIT_SET_RESET_OFFSET;
	volatile uint32_t *port_bit_set_resets	   = (volatile uint32_t *)port_bit_set_reset_addr;

	if (level)
	{
		// Set the bit to 1
		*port_bit_set_resets |= (0b1 << (pin));
	}
	else
	{
		// Reset the bit to 0
		*port_bit_set_resets |= (0b1 << (pin + 16));
	}
}

void reg_setup()
{

	gpio.a->port_mode.pin5 = GPIO_PORT_MODE_OUTPUT;
}

void reg_setup2()
{
	gpio_setup(GPIO_PORT_LETTER_A, 5, GPIO_PORT_MODE_OUTPUT);
}

void setLD2(enum GPIO_PORT_OUTPUT_DATA level)
{
	if (level)
	{
		gpio.a->bit_set_reset.pin5_set = 1;
	}
	else
	{
		gpio.a->bit_set_reset.pin5_reset = 1;
	}
}
