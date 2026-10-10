#pragma once
#include "assert.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const uintptr_t SYSCFG_BASE = 0x40013800;

enum syscfg_extiX_t
{
	// The pins.
	pa_x = 0b0000,
	pb_x = 0b0001,
	pc_x = 0b0010,
	pd_x = 0b0011,
	pe_x = 0b0100,
	// pf_x_reserved = 0b0101,
	// pg_x_reserved = 0b0110,
	ph_x = 0b0111,
}; // u4 type

typedef enum syscfg_extiX_t syscfg_pin_letter_t;

typedef struct
{
	enum syscfg_extiX_t exti0 : 4;
	enum syscfg_extiX_t exti1 : 4;
	enum syscfg_extiX_t exti2 : 4;
	enum syscfg_extiX_t exti3 : 4;
	uint32_t			_reserved : 16;

} syscfg_exti_cr1_t;

typedef struct
{
	enum syscfg_extiX_t exti4 : 4;
	enum syscfg_extiX_t exti5 : 4;
	enum syscfg_extiX_t exti6 : 4;
	enum syscfg_extiX_t exti7 : 4;
	uint32_t			_reserved : 16;

} syscfg_exti_cr2_t;

typedef struct
{
	enum syscfg_extiX_t exti8 : 4;
	enum syscfg_extiX_t exti9 : 4;
	enum syscfg_extiX_t exti10 : 4;
	enum syscfg_extiX_t exti11 : 4;
	uint32_t			_reserved : 16;

} syscfg_exti_cr3_t;

typedef struct
{
	enum syscfg_extiX_t exti12 : 4;
	enum syscfg_extiX_t exti13 : 4;
	enum syscfg_extiX_t exti14 : 4;
	enum syscfg_extiX_t exti15 : 4;
	uint32_t			_reserved : 16;

} syscfg_exti_cr4_t;

STATIC_ASSERT(sizeof(syscfg_exti_cr1_t) == sizeof(uint32_t), "syscfg_extic_r1_t must be 32 bit!");

// ========================= The meta struct =======================================================

typedef struct
{
	uint32_t		  memrmp;
	uint32_t		  pmc;
	syscfg_exti_cr1_t exti_cr1;
	syscfg_exti_cr2_t exti_cr2;
	syscfg_exti_cr3_t exti_cr3;
	syscfg_exti_cr4_t exti_cr4;
	uint32_t		  cmpcr;

} syscfg_t;

extern volatile syscfg_t *const syscfg;
