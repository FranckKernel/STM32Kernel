#include "flash.h"

volatile flash_t *const flash = (volatile flash_t *const)FLASH_MMIO_ADDRESS_BASE;
void					configure_flash()
{
	// need to change the number of wait states to 3.
	// checking table 5. Oage 45 of 842.
	// for 3.3V and 96Mhz.
	flash_access_control_t fa	= flash->acr;
	fa.latency_wait_state_count = 3;
	fa.prefetch_enable			= 1;
	fa.data_cache_enable		= 1;
	fa.instruction_cache_enable = 1;

	flash->acr = fa;
	while (flash->acr.latency_wait_state_count != 3)
	{
	} // confirm the new latency took effect
}
