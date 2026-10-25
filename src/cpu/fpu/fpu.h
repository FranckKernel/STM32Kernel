#pragma once
#include <stddef.h>
#include <stdint.h>

// The addresses
static const uintptr_t CPACR_BASE = 0xE000ED88;

static const uintptr_t FPCCR_BASE = 0xE000EF34;
static const uintptr_t FPCAR_BASE = 0xE000EF38;

static const uintptr_t FPDSCR = 0xE000EF3C;

static const uintptr_t FPSCR = 0x0; // unknown

// The structs

enum cpn
{
	cpn_denied			= 0b00,
	cpn_priviledge_only = 0b01,
	// cpn_reserved = 0b10,
	cpn_full_access = 0b11,
};

typedef struct
{
	uint32_t _reserved1 : 20;
	enum cpn cp10 : 2;
	enum cpn cp11 : 2;
	uint32_t _reserved2 : 8;

} coprocessor_access_control_register_t;

_Static_assert(sizeof(coprocessor_access_control_register_t) == sizeof(uint32_t),
	"coprocessor_access_control_register_t is not the proper size of 32 bit!");

// No meta struct. Registers aren't consecutive
static volatile coprocessor_access_control_register_t *const fpu_cacr = (volatile coprocessor_access_control_register_t *)CPACR_BASE;

// The assignment

// The functions
void fpu_enable();
