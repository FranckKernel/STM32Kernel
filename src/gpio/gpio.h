#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const uint32_t GPIO_MMIO_ADDRESS_BASE = 0x40020000;

static const uint32_t GPIO_MMIO_LETTER_DIFF = 0x400;

static const uint32_t GPIO_MMIO_A_BASE = GPIO_MMIO_ADDRESS_BASE;
static const uint32_t GPIO_MMIO_B_BASE = GPIO_MMIO_A_BASE + 1 * 0x0400;
static const uint32_t GPIO_MMIO_C_BASE = GPIO_MMIO_A_BASE + 2 * 0x0400;
static const uint32_t GPIO_MMIO_D_BASE = GPIO_MMIO_A_BASE + 3 * 0x0400;
static const uint32_t GPIO_MMIO_E_BASE = GPIO_MMIO_A_BASE + 4 * 0x0400;
static const uint32_t GPIO_MMIO_F_BASE = GPIO_MMIO_A_BASE + 5 * 0x0400;
static const uint32_t GPIO_MMIO_G_BASE = GPIO_MMIO_A_BASE + 6 * 0x0400;

// GPIO PORT MODE ===========================
static const uint32_t GPIO_PORT_MODE_ADDRESS_OFFSET = 0x00;
enum GPIO_PORT_MODE
{
	GPIO_PORT_MODE_INPUT			  = 0b00,
	GPIO_PORT_MODE_OUTPUT			  = 0b01,
	GPIO_PORT_MODE_ALTERNATE_FUNCTION = 0b10,
	GPIO_PORT_MODE_ANALOG_MODE		  = 0b11
};
typedef struct
{
	enum GPIO_PORT_MODE pin0 : 2;
	enum GPIO_PORT_MODE pin1 : 2;
	enum GPIO_PORT_MODE pin2 : 2;
	enum GPIO_PORT_MODE pin3 : 2;
	enum GPIO_PORT_MODE pin4 : 2;
	enum GPIO_PORT_MODE pin5 : 2;
	enum GPIO_PORT_MODE pin6 : 2;
	enum GPIO_PORT_MODE pin7 : 2;
	enum GPIO_PORT_MODE pin8 : 2;
	enum GPIO_PORT_MODE pin9 : 2;
	enum GPIO_PORT_MODE pin10 : 2;
	enum GPIO_PORT_MODE pin11 : 2;
	enum GPIO_PORT_MODE pin12 : 2;
	enum GPIO_PORT_MODE pin13 : 2;
	enum GPIO_PORT_MODE pin14 : 2;
	enum GPIO_PORT_MODE pin15 : 2;

} gpio_port_mode_t;
_Static_assert(sizeof(gpio_port_mode_t) == sizeof(uint32_t), "gpio_port_mode_t : We are fucked!");

// GPIO PORT OUTPUT TYPE  ===========================
static const uint32_t GPIO_PORT_OUTPUT_TYPE = 0x04;
enum GPIO_PORT_OUTPUT_TYPE
{
	GPIO_PORT_OUTPUT_TYPE_PUSH_PULL	 = 0b0,
	GPIO_PORT_OUTPUT_TYPE_OPEN_DRAIN = 0b1,
};
typedef struct
{
	enum GPIO_PORT_OUTPUT_TYPE pin0 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin1 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin2 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin3 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin4 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin5 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin6 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin7 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin8 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin9 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin10 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin11 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin12 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin13 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin14 : 1;
	enum GPIO_PORT_OUTPUT_TYPE pin15 : 1;
	uint32_t				   reserved : 16;

} gpio_port_output_type_t;
_Static_assert(sizeof(gpio_port_output_type_t) == sizeof(uint32_t), "gpio_port_output_type_t : We are fucked!");

// GPIO PORT OUTPUT SPEED  ===========================
static const uint32_t GPIO_PORT_OUTPUT_SPEED_ADDRESS_OFFSET = 0x08;
enum GPIO_PORT_OUTPUT_SPEED
{
	GPIO_PORT_OUTPUT_SPEED_LOW	  = 0b00,
	GPIO_PORT_OUTPUT_SPEED_MEDIUM = 0b01,
	GPIO_PORT_OUTPUT_SPEED_FAST	  = 0b10,
	GPIO_PORT_OUTPUT_SPEED_HIGH	  = 0b11
};
typedef struct
{
	enum GPIO_PORT_OUTPUT_SPEED pin0 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin1 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin2 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin3 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin4 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin5 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin6 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin7 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin8 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin9 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin10 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin11 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin12 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin13 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin14 : 2;
	enum GPIO_PORT_OUTPUT_SPEED pin15 : 2;

} gpio_port_output_speed_t;
_Static_assert(sizeof(gpio_port_output_speed_t) == sizeof(uint32_t), "gpio_port_output_speed_t : We are fucked!");

// GPIO PORT PULL UP/ PULL MODE MODE  ===========================
static const uint32_t GPIO_PORT_PULL_MODE_ADDRESS_OFFSET = 0x0C;
enum GPIO_PORT_PULL_MODE
{
	GPIO_PORT_PULL_MODE_NONE = 0b00,
	GPIO_PORT_PULL_MODE_UP	 = 0b01,
	GPIO_PORT_PULL_MODE_DOWN = 0b10,
	// GPIO_PORT_PULL_MODE_RESERVED = 0b11
};
typedef struct
{
	enum GPIO_PORT_PULL_MODE pin0 : 2;
	enum GPIO_PORT_PULL_MODE pin1 : 2;
	enum GPIO_PORT_PULL_MODE pin2 : 2;
	enum GPIO_PORT_PULL_MODE pin3 : 2;
	enum GPIO_PORT_PULL_MODE pin4 : 2;
	enum GPIO_PORT_PULL_MODE pin5 : 2;
	enum GPIO_PORT_PULL_MODE pin6 : 2;
	enum GPIO_PORT_PULL_MODE pin7 : 2;
	enum GPIO_PORT_PULL_MODE pin8 : 2;
	enum GPIO_PORT_PULL_MODE pin9 : 2;
	enum GPIO_PORT_PULL_MODE pin10 : 2;
	enum GPIO_PORT_PULL_MODE pin11 : 2;
	enum GPIO_PORT_PULL_MODE pin12 : 2;
	enum GPIO_PORT_PULL_MODE pin13 : 2;
	enum GPIO_PORT_PULL_MODE pin14 : 2;
	enum GPIO_PORT_PULL_MODE pin15 : 2;

} gpio_port_pull_mode_t;
_Static_assert(sizeof(gpio_port_pull_mode_t) == sizeof(uint32_t), "gpio_port_pull_mode_t : We are fucked!");

// GPIO PORT INPUT DATA  ===========================
static const uint32_t GPIO_PORT_INPUT_DATA_ADDRESS_OFFSET = 0x10;
enum GPIO_PORT_INPUT_DATA
{
	GPIO_PORT_INPUT_LOW	 = 0b0,
	GPIO_PORT_INPUT_HIGH = 0b1,
};
typedef struct
{
	enum GPIO_PORT_INPUT_DATA pin0 : 1;
	enum GPIO_PORT_INPUT_DATA pin1 : 1;
	enum GPIO_PORT_INPUT_DATA pin2 : 1;
	enum GPIO_PORT_INPUT_DATA pin3 : 1;
	enum GPIO_PORT_INPUT_DATA pin4 : 1;
	enum GPIO_PORT_INPUT_DATA pin5 : 1;
	enum GPIO_PORT_INPUT_DATA pin6 : 1;
	enum GPIO_PORT_INPUT_DATA pin7 : 1;
	enum GPIO_PORT_INPUT_DATA pin8 : 1;
	enum GPIO_PORT_INPUT_DATA pin9 : 1;
	enum GPIO_PORT_INPUT_DATA pin10 : 1;
	enum GPIO_PORT_INPUT_DATA pin11 : 1;
	enum GPIO_PORT_INPUT_DATA pin12 : 1;
	enum GPIO_PORT_INPUT_DATA pin13 : 1;
	enum GPIO_PORT_INPUT_DATA pin14 : 1;
	enum GPIO_PORT_INPUT_DATA pin15 : 1;
	uint32_t				  reserved : 16;

} gpio_port_input_data_t;
_Static_assert(sizeof(gpio_port_input_data_t) == sizeof(uint32_t), "gpio_port_input_data_t : We are fucked!");

// GPIO PORT OUTPUT DATA  ===========================
static const uint32_t GPIO_PORT_OUTPUT_DATA_ADDRESS_OFFSET = 0x14;
enum GPIO_PORT_OUTPUT_DATA
{
	GPIO_PORT_OUTPUT_LOW  = 0b0,
	GPIO_PORT_OUTPUT_HIGH = 0b1,
};
typedef struct
{
	enum GPIO_PORT_OUTPUT_DATA pin0 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin1 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin2 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin3 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin4 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin5 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin6 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin7 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin8 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin9 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin10 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin11 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin12 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin13 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin14 : 1;
	enum GPIO_PORT_OUTPUT_DATA pin15 : 1;
	uint32_t				   reserved : 16;

} gpio_port_output_data_t;
_Static_assert(sizeof(gpio_port_output_data_t) == sizeof(uint32_t), "gpio_port_output_data_t : We are fucked!");

// GPIO PORT BIT SET/RESET  ===========================
static const uint32_t GPIO_PORT_BIT_SET_RESET_OFFSET = 0x18;
enum GPIO_PORT_BIT_SET
{
	GPIO_PORT_BIT_SET_NO_ACTION = 0b0,
	GPIO_PORT_BIT_SET_SET		= 0b1,
};

enum GPIO_PORT_BIT_RESET
{
	GPIO_PORT_BIT_RESET_NO_ACTION = 0b0,
	GPIO_PORT_BIT_RESET_RESET	  = 0b1,
};
typedef struct
{
	enum GPIO_PORT_BIT_SET pin0_set : 1;
	enum GPIO_PORT_BIT_SET pin1_set : 1;
	enum GPIO_PORT_BIT_SET pin2_set : 1;
	enum GPIO_PORT_BIT_SET pin3_set : 1;
	enum GPIO_PORT_BIT_SET pin4_set : 1;
	enum GPIO_PORT_BIT_SET pin5_set : 1;
	enum GPIO_PORT_BIT_SET pin6_set : 1;
	enum GPIO_PORT_BIT_SET pin7_set : 1;
	enum GPIO_PORT_BIT_SET pin8_set : 1;
	enum GPIO_PORT_BIT_SET pin9_set : 1;
	enum GPIO_PORT_BIT_SET pin10_set : 1;
	enum GPIO_PORT_BIT_SET pin11_set : 1;
	enum GPIO_PORT_BIT_SET pin12_set : 1;
	enum GPIO_PORT_BIT_SET pin13_set : 1;
	enum GPIO_PORT_BIT_SET pin14_set : 1;
	enum GPIO_PORT_BIT_SET pin15_set : 1;

	enum GPIO_PORT_BIT_RESET pin0_reset : 1;
	enum GPIO_PORT_BIT_RESET pin1_reset : 1;
	enum GPIO_PORT_BIT_RESET pin2_reset : 1;
	enum GPIO_PORT_BIT_RESET pin3_reset : 1;
	enum GPIO_PORT_BIT_RESET pin4_reset : 1;
	enum GPIO_PORT_BIT_RESET pin5_reset : 1;
	enum GPIO_PORT_BIT_RESET pin6_reset : 1;
	enum GPIO_PORT_BIT_RESET pin7_reset : 1;
	enum GPIO_PORT_BIT_RESET pin8_reset : 1;
	enum GPIO_PORT_BIT_RESET pin9_reset : 1;
	enum GPIO_PORT_BIT_RESET pin10_reset : 1;
	enum GPIO_PORT_BIT_RESET pin11_reset : 1;
	enum GPIO_PORT_BIT_RESET pin12_reset : 1;
	enum GPIO_PORT_BIT_RESET pin13_reset : 1;
	enum GPIO_PORT_BIT_RESET pin14_reset : 1;
	enum GPIO_PORT_BIT_RESET pin15_reset : 1;

} gpio_port_bit_set_reset_t;
_Static_assert(sizeof(gpio_port_bit_set_reset_t) == sizeof(uint32_t), "gpio_port_bit_set_reset_t : We are fucked!");
// ======================================== The meta struct:
typedef struct
{
	gpio_port_mode_t		  port_mode;
	gpio_port_output_type_t	  output_type;
	gpio_port_output_speed_t  output_speed;
	gpio_port_pull_mode_t	  pull_mode;
	gpio_port_input_data_t	  input_data;
	gpio_port_output_data_t	  output_data;
	gpio_port_bit_set_reset_t bit_set_reset;

} gpio_x_register_t;

struct GPIO_REGISTERS
{
	volatile gpio_x_register_t *const a;
	volatile gpio_x_register_t *const b;
	volatile gpio_x_register_t *const c;
	volatile gpio_x_register_t *const d;
	volatile gpio_x_register_t *const e;
	volatile gpio_x_register_t *const f;
	volatile gpio_x_register_t *const g;
};

enum GPIO_PORT_LETTER
{
	GPIO_PORT_LETTER_A = 0,
	GPIO_PORT_LETTER_B,
	GPIO_PORT_LETTER_C,
	GPIO_PORT_LETTER_D,
	GPIO_PORT_LETTER_E,
	GPIO_PORT_LETTER_F,
	GPIO_PORT_LETTER_G,

};

extern struct GPIO_REGISTERS gpio;

void reg_setup();
void gpio_setup(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_PORT_MODE mode);

void gpio_write(enum GPIO_PORT_LETTER letter, uint8_t pin, enum GPIO_PORT_OUTPUT_DATA level);
void setLD2(enum GPIO_PORT_OUTPUT_DATA level);
