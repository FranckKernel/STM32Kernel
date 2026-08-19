#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static inline void nop(void)
{
	__asm volatile("nop");
}
