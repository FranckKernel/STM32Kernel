// Mainly use ArmV7 instruction manual. Start at page 626
// and maybe a bit of STM32f411re programing manual
// Actually, the programming manual is easier to read, so imma use that one mainly.

// NVIC: Nested Vectored interrupt controller. It manage external interrupts,
// deciding which are enabled, pending active and their priority
// IRQ : Interrupt Request. Comes from outside the cpu, not a cpu exception. Managed by NVIC
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const uintptr_t NVIC_BASE = 0xE000E100;

// ISER — Interrupt Set-Enable Register
// Enables an IRQ so the NVIC can forward it to the CPU.
// Usage: write 1 to the bit corresponding to the IRQ number.
static const uintptr_t NVIC_ISER_BASE = NVIC_BASE + 0x80 * 0;

// ICER — Interrupt Clear-Enable Register
// Disables an IRQ in the NVIC.
// Usage: write 1 to the bit corresponding to the IRQ number.
static const uintptr_t NVIC_ICER_BASE = NVIC_BASE + 0x80 * 1;

// ISPR — Interrupt Set-Pending Register
// Manually marks an IRQ as pending, causing it to be serviced when eligible.
// A method to software trigger an irq
// Usage: write 1 to the bit corresponding to the IRQ number.
static const uintptr_t NVIC_ISPR_BASE = NVIC_BASE + 0x80 * 2;

// ICPR — Interrupt Clear-Pending Register
// Removes the pending state of an IRQ that has not yet been serviced.
// Usage: write 1 to the bit corresponding to the IRQ number.
static const uintptr_t NVIC_ICPR_BASE = NVIC_BASE + 0x80 * 3;

// IABR — Interrupt Active Bit Register
// Shows which IRQs are currently being serviced (active).
// Usage: read the corresponding bit; unlike the others, this is read-only.
static const uintptr_t NVIC_IABR_BASE = NVIC_BASE + 0x80 * 4;

// Reserved — 0xE000E380 to 0xE000E3FF
// Must not be accessed.
// static const uintptr_t NVIC_RESERVED_5_BASE = NVIC_BASE + 0x80 * 5;

// IPR — Interrupt Priority Registers
// Controls the priority of each external IRQ.
// Usage: write the implemented priority bits for the IRQ; lower numerical priority means higher urgency.
static const uintptr_t NVIC_IPR_BASE = NVIC_BASE + 0x80 * 6;

// Reserved — 0xE000E480 to 0xE000E4FF
// Must not be accessed.
// static const uintptr_t NVIC_RESERVED_7_BASE = NVIC_BASE + 0x80 * 7;

// STIR — Software Trigger Interrupt Register
// Allows software to trigger an external IRQ as though the peripheral had requested it.
// Prefered method to software trigger an irq
// Usage: write an IRQ number to STIR to make that IRQ pending.
static const uintptr_t STIR_BASE = NVIC_BASE + 0x80 * 8;

// ======== The structures =========

typedef struct
{

	uint32_t irq_32xN_plus0 : 1;
	uint32_t irq_32xN_plus1 : 1;
	uint32_t irq_32xN_plus2 : 1;
	uint32_t irq_32xN_plus3 : 1;
	uint32_t irq_32xN_plus4 : 1;
	uint32_t irq_32xN_plus5 : 1;
	uint32_t irq_32xN_plus6 : 1;
	uint32_t irq_32xN_plus7 : 1;
	uint32_t irq_32xN_plus8 : 1;
	uint32_t irq_32xN_plus9 : 1;
	uint32_t irq_32xN_plus10 : 1;
	uint32_t irq_32xN_plus11 : 1;
	uint32_t irq_32xN_plus12 : 1;
	uint32_t irq_32xN_plus13 : 1;
	uint32_t irq_32xN_plus14 : 1;
	uint32_t irq_32xN_plus15 : 1;
	uint32_t irq_32xN_plus16 : 1;
	uint32_t irq_32xN_plus17 : 1;
	uint32_t irq_32xN_plus18 : 1;
	uint32_t irq_32xN_plus19 : 1;
	uint32_t irq_32xN_plus20 : 1;
	uint32_t irq_32xN_plus21 : 1;
	uint32_t irq_32xN_plus22 : 1;
	uint32_t irq_32xN_plus23 : 1;
	uint32_t irq_32xN_plus24 : 1;
	uint32_t irq_32xN_plus25 : 1;
	uint32_t irq_32xN_plus26 : 1;
	uint32_t irq_32xN_plus27 : 1;
	uint32_t irq_32xN_plus28 : 1;
	uint32_t irq_32xN_plus29 : 1;
	uint32_t irq_32xN_plus30 : 1;
	uint32_t irq_32xN_plus31 : 1;
} nvic_32bit_irq_register_t;

// Interrupt Set-Enable Registers
// ISER0..ISER7
typedef struct
{
	union
	{

		uint32_t				  registers_raw[8];
		nvic_32bit_irq_register_t registers[8];
	};

} nvic_iser_t;

// Interrupt Clear-Enable Registers
// ICER0..ICER7
typedef struct
{
	union
	{

		uint32_t				  registers_raw[8];
		nvic_32bit_irq_register_t registers[8];
	};
} nvic_icer_t;

// Interrupt Set-Pending Registers
// ISPR0..ISPR7
typedef struct
{
	union
	{

		uint32_t				  registers_raw[8];
		nvic_32bit_irq_register_t registers[8];
	};
} nvic_ispr_t;

// Interrupt Clear-Pending Registers
// ICPR0..ICPR7
typedef struct
{
	union
	{

		uint32_t				  registers_raw[8];
		nvic_32bit_irq_register_t registers[8];
	};
} nvic_icpr_t;

// Interrupt Active Bit Registers
// IABR0..IABR7
typedef struct
{
	union
	{

		uint32_t				  registers_raw[8];
		nvic_32bit_irq_register_t registers[8];
	};
} nvic_iabr_t;

// Interrupt Priority Register
//
// There are 60 IPR registers.
// Each IPR register contains four 8-bit priority fields:
//
// IPR0:
//   [31:24] IRQ3 priority
//   [23:16] IRQ2 priority
//   [15:8]  IRQ1 priority
//   [7:0]   IRQ0 priority
//
// IPR1:
//   [31:24] IRQ7 priority
//   [23:16] IRQ6 priority
//   [15:8]  IRQ5 priority
//   [7:0]   IRQ4 priority
//
// and so on.

typedef struct
{
	uint8_t irq_4xN_plus0;
	uint8_t irq_4xN_plus1;
	uint8_t irq_4xN_plus2;
	uint8_t irq_4xN_plus3;
} nvic_ipr_register_t;

typedef struct
{
	nvic_ipr_register_t registers[60];
} nvic_ipr_t;

// Software Trigger Interrupt Register
//
// Bits [8:0] contain the interrupt ID.
// Bits [31:9] are reserved.
typedef struct
{
	uint32_t interrupt_id : 9;
	uint32_t reserved : 23;
} nvic_stir_t;

// ======== The meta structure =========
typedef struct
{
	volatile nvic_iser_t *const interrupt_set_enable;		// length 8
	volatile nvic_icer_t *const interrupt_clear_enable;		// length 8
	volatile nvic_ispr_t *const interrupt_set_pending;		// length 8
	volatile nvic_icpr_t *const interrupt_clear_pending;	// length 8
	volatile nvic_iabr_t *const interrupt_active_bit;		// length 8
	volatile nvic_ipr_t *const	interrupt_priority;			// length 60
	volatile nvic_stir_t *const software_trigger_interrupt; // length 1

} nvic_t;

extern const nvic_t nvic;

// =================================== The functions ======================

void nvic_enable_irq(uint8_t irq_number);
void nvic_disable_irq(uint8_t irq_number);

void nvic_trigger_irq(uint8_t irq_number);
// need interrupt setup
