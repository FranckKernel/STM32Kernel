#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const uintptr_t EXTI_BASE = 0x4001'3C00;

// ===================== The Meta struct

typedef struct
{
	uint32_t exti_imr;
	uint32_t exti_emr;
	uint32_t exti_rtsr;
	uint32_t exti_ftsr;
	uint32_t exti_swier;
	uint32_t exti_pr;
} exti_t;
