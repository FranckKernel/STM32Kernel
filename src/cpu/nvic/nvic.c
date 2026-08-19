#include "macros.h"
#include "nvic.h"

const nvic_t nvic = {
	.interrupt_set_enable = (volatile nvic_iser_t *)NVIC_ISER_BASE,

	.interrupt_clear_enable = (volatile nvic_icer_t *)NVIC_ICER_BASE,

	.interrupt_set_pending = (volatile nvic_ispr_t *)NVIC_ISPR_BASE,

	.interrupt_clear_pending = (volatile nvic_icpr_t *)NVIC_ICPR_BASE,

	.interrupt_active_bit = (volatile nvic_iabr_t *)NVIC_IABR_BASE,

	.interrupt_priority = (volatile nvic_ipr_t *)NVIC_IPR_BASE,

	.software_trigger_interrupt = (volatile nvic_stir_t *)STIR_BASE,
};

void nvic_enable_irq(uint8_t irq_number)
{

	uint8_t register32_offset	   = irq_number / 32;
	uint8_t in_register_bit_number = irq_number % 32;

#ifdef DEBUG
	if (register32_offset > 7)
	{
		// print something here?
		return;
	}
	if (register32_offset == 7 && in_register_bit_number > 16)
	{
		// Reserved
		return;
	}
#endif

	nvic.interrupt_set_enable->registers_raw[register32_offset] = (1 << in_register_bit_number);
}

void nvic_disable_irq(uint8_t irq_number)
{

	uint8_t register32_offset	   = irq_number / 32;
	uint8_t in_register_bit_number = irq_number % 32;

#ifdef DEBUG
	if (register32_offset > 7)
	{
		// print something here?
		return;
	}
	if (register32_offset == 7 && in_register_bit_number > 16)
	{
		// Reserved
		return;
	}
#endif

	nvic.interrupt_clear_enable->registers_raw[register32_offset] = (1 << in_register_bit_number);
}

__attribute__((noinline)) void nvic_trigger_irq(uint8_t irq_number)
{
#ifdef DEBUG
	if (irq_number > 239)
	{
		// out of range
		return;
	}
#endif

	nvic_stir_t irq_command						 = {.interrupt_id = irq_number, .reserved = 0};
	*(uint32_t *)nvic.software_trigger_interrupt = BITCAST(uint32_t, irq_command);
}
