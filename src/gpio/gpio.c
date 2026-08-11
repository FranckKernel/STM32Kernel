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

void gpio_port_mode_setup(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_PORT_MODE mode)
{
	uint32_t		   letter_base	  = (GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF);
	uint32_t		   port_mode_addr = letter_base + GPIO_PORT_MODE_ADDRESS_OFFSET;
	volatile uint32_t *port_modes	  = (volatile uint32_t *)port_mode_addr;

	uint32_t port_modes_val = *port_modes;
	port_modes_val &= ~(0b11 << (2 * pin));
	port_modes_val |= ((uint32_t)mode << (2 * pin));

	*port_modes = port_modes_val;
}
void gpio_output_type_setup(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_PORT_OUTPUT_TYPE output_type)
{
	uint32_t								letter_base		= GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF;
	volatile gpio_x_register_t *const		gpioX			= (volatile gpio_x_register_t *const)letter_base;
	volatile gpio_port_output_type_t *const output_type_ptr = &gpioX->output_type;

	uint32_t output_type_raw = *(volatile uint32_t *)output_type_ptr;

	output_type_raw &= ~(0b1u << pin);
	output_type_raw |= ((uint32_t)output_type << pin);

	*(volatile uint32_t *const)output_type_ptr = output_type_raw;
}

void gpio_pull_mode_setup(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_PORT_PULL_MODE pull_mode)
{
	uint32_t							  letter_base	= (GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF);
	volatile gpio_x_register_t *const	  gpioX			= (volatile gpio_x_register_t *const)letter_base;
	volatile gpio_port_pull_mode_t *const pull_mode_ptr = &gpioX->pull_mode;

	uint32_t pull_mode_raw = *(volatile uint32_t *)pull_mode_ptr;

	pull_mode_raw &= ~(0b11u << (2 * pin));				 // set those bits to 0
	pull_mode_raw |= ((uint32_t)pull_mode << (2 * pin)); // set it to the wanted value

	volatile uint32_t *const pull_mode_ptr_raw = (volatile uint32_t *const)pull_mode_ptr;
	*pull_mode_ptr_raw						   = pull_mode_raw;
}

void gpio_output_speed_setup(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_PORT_OUTPUT_SPEED speed)
{
	uint32_t								 letter_base = GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF;
	volatile gpio_x_register_t *const		 gpioX		 = (volatile gpio_x_register_t *const)letter_base;
	volatile gpio_port_output_speed_t *const speed_ptr	 = &gpioX->output_speed;

	uint32_t speed_raw = *(volatile uint32_t *)speed_ptr;

	speed_raw &= ~(0b11u << (2 * pin));
	speed_raw |= ((uint32_t)speed << (2 * pin));

	*(volatile uint32_t *const)speed_ptr = speed_raw;
}

void gpio_write(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_OUTPUT_DATA level)
{
	uint32_t								  letter_base	= (GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF);
	volatile gpio_x_register_t *const		  gpioX			= (volatile gpio_x_register_t *const)letter_base;
	volatile gpio_port_bit_set_reset_t *const set_reset_ptr = &gpioX->bit_set_reset;

	volatile uint32_t *set_reset_ptr_raw = (volatile uint32_t *)set_reset_ptr;

	if (level)
	{
		// Set the bit to 1
		*set_reset_ptr_raw = (0b1 << (pin));
	}
	else
	{
		// Reset the bit to 0
		*set_reset_ptr_raw = (0b1 << (pin + 16));
	}
	// pin + 16*(1-level)
}

void reg_setup()
{
	gpio.a->port_mode.pin5 = GPIO_PORT_MODE_OUTPUT;
}

enum GPIO_INPUT_DATA gpio_read(enum GPIO_PORT_LETTER letter, uint8_t pin)
{

	uint32_t							   letter_base = (GPIO_MMIO_ADDRESS_BASE + letter * GPIO_MMIO_LETTER_DIFF);
	volatile gpio_x_register_t *const	   gpioX	   = (volatile gpio_x_register_t *const)letter_base;
	volatile gpio_port_input_data_t *const input_data  = &gpioX->input_data;

	// return input_data->pin1;

	volatile uint32_t *const input_data_raw = (volatile uint32_t *const)input_data;
	uint32_t				 id				= *input_data_raw;

	return (enum GPIO_INPUT_DATA)((id >> pin) & 1u);
}

void setLD2(enum GPIO_OUTPUT_DATA level)
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
