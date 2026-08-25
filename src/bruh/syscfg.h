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

typedef struct
{
	enum syscfg_extiX_t exti0 : 4;
	enum syscfg_extiX_t exti1 : 4;
	enum syscfg_extiX_t exti2 : 4;
	enum syscfg_extiX_t exti3 : 4;
	uint32_t			_reserved : 16;

} syscfg_extic_r1_t;

STATIC_ASSERT(sizeof(syscfg_extic_r1_t) == sizeof(uint32_t), "syscfg_extic_r1_t must be 32 bit!");

// ========================= The meta struct =======================================================

typedef struct
{
	uint32_t		  memrmp;
	uint32_t		  pmc;
	syscfg_extic_r1_t exticr1;
	syscfg_extic_r1_t exticr2;
	syscfg_extic_r1_t exticr3;
	syscfg_extic_r1_t exticr4;
	uint32_t		  cmpcr;

} syscfg_t;
