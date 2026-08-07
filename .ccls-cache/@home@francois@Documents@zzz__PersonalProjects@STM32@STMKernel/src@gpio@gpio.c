#include "gpio.h"

static gpio_x_register_t *const GPIOA_Register = (gpio_x_register_t *)GPIO_MMIO_A_BASE;
static gpio_x_register_t *const GPIOB_Register = (gpio_x_register_t *)GPIO_MMIO_B_BASE;
static gpio_x_register_t *const GPIOC_Register = (gpio_x_register_t *)GPIO_MMIO_C_BASE;
static gpio_x_register_t *const GPIOD_Register = (gpio_x_register_t *)GPIO_MMIO_D_BASE;
static gpio_x_register_t *const GPIOE_Register = (gpio_x_register_t *)GPIO_MMIO_E_BASE;
static gpio_x_register_t *const GPIOF_Register = (gpio_x_register_t *)GPIO_MMIO_F_BASE;
static gpio_x_register_t *const GPIOG_Register = (gpio_x_register_t *)GPIO_MMIO_G_BASE;

struct GPIO_REGISTERS gpio = {
	.a = GPIOA_Register,
	.b = GPIOB_Register,
	.c = GPIOC_Register,
	.d = GPIOD_Register,
	.e = GPIOE_Register,
	.f = GPIOF_Register,
	.g = GPIOG_Register,
};

void gpio_setup(enum GPIO_PORT_LETTER letter, uint8_t pin, bool f)
{
	gpio_x_register_t *letter_base = (gpio_x_register_t *)(GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF);
}

void reg_setup()
{

	gpio.a->port_mode.pin5 = GPIO_PORT_MODE_OUTPUT;
}
