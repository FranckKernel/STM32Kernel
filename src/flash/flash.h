#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const uint32_t FLASH_MMIO_ADDRESS_BASE = 0x40023C00;

// ============================================================== THE META STRUCT =====================

typedef struct
{
	uint32_t latency_wait_state_count : 4; // bit 0-3, (rw), treat it as a u4. That's the number of wait states
	uint32_t _reserved1 : 4;
	uint32_t prefetch_enable : 1;
	uint32_t instruction_cache_enable : 1;
	uint32_t data_cache_enable : 1;

	// These two are only writable when the respective cache is disabled
	uint32_t instruction_cache_reset : 1;
	uint32_t data_cache_reset : 1;

} flash_access_control_t;

typedef struct
{
	flash_access_control_t acr;
	// Other stuff
} flash_t;

extern volatile flash_t *const flash;

// =============================== The Functions
void configure_flash();
