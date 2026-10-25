#include "macros.h"
#include "stdio.h"
#include "string.h"
#include "timer_frequency.h"
#include "usart.h"
#include <math.h>

volatile usart_t *const usart2 = (volatile usart_t *const)USART2_BASE;
volatile usart_t *const usart1 = (volatile usart_t *const)USART1_BASE;
volatile usart_t *const usart6 = (volatile usart_t *const)USART6_BASE;

void config_usart(volatile usart_t *const usart, const usart_config_t *const config)
{

	// config.baudrate;
	// config.word_length;
	// config.stop_bit;
	// config.parity;
	// config.mode;
	// config.hardware_control;
	// config.oversampling;

	// Figure out the divider. 32 bit, 23 bit mantissa, 24 bit of integer precision.
	// 2^24 = 16 million. Slower then PLL reachable clock speed
	double clock_frequency_f = clock_frequency / 2.0;

	uint8_t oversampling = config->oversampling_not_over8 ? 8 : 16;

	double divider = clock_frequency_f / (double)config->baudrate;
	divider /= (double)oversampling;
	double divider_fraction = divider - (uint32_t)divider;

	uint16_t div			  = (uint16_t)divider;
	uint8_t	 fraction_diviser = (uint8_t)((double)oversampling * divider_fraction);

	usart_brr_t temp_brr = READ32(&usart->brr);

	// In fixed signed representation
	temp_brr.div_mantissa = div; // Kinda dumb, these are the whole number.
	temp_brr.div_fraction = fraction_diviser;
	WRITE32(&usart->brr, temp_brr);

	// Writting to CR
	usart_cr1_t cr1_temp = READ32(&usart->cr1);
	usart_cr2_t cr2_temp = READ32(&usart->cr2);
	usart_cr3_t cr3_temp = READ32(&usart->cr3);

	cr2_temp.stop_bits = (enum usart_stop_bits)config->stop_bit;
	if (config->clock_info.clock_enable)
	{
		cr2_temp.clock_enable = config->clock_info.clock_enable;
		// These 2 are usually 0
		cr2_temp.clock_phase	= config->clock_info.clock_phase;
		cr2_temp.clock_polarity = config->clock_info.clock_polarity;
	}

	cr1_temp.usart_enable = 1;
	cr1_temp.word_length  = config->word_length;
	cr1_temp.oversampling = config->oversampling_not_over8;

	switch (config->parity)
	{
	case (usart_parity_none):
	{
		cr1_temp.parity_control_enable = 0;
		break;
	}
	case (usart_parity_odd):
	{
		cr1_temp.parity_control_enable		   = 1;
		cr1_temp.parity_selection_odd_not_even = 1;
		cr1_temp.parity_error_interrupt_enable = 1;
		cr3_temp.error_interrupt_enable		   = 1;
		break;
	}
	case (usart_parity_even):
	{
		cr1_temp.parity_control_enable		   = 1;
		cr1_temp.parity_selection_odd_not_even = 0;
		cr1_temp.parity_error_interrupt_enable = 1;
		cr3_temp.error_interrupt_enable		   = 1;
		break;
	}
	}

	switch (config->mode)
	{
	case (usart_mode_reciever):
	{
		cr1_temp.reciever_enable	   = 1;
		cr1_temp.rxne_interrupt_enable = 1;

		break;
	}
	case (usart_mode_transmitter):
	{
		cr1_temp.transmitter_enable = 1;
		// polling is overkill
		// cr1_temp.txe_interrupt_enable = 1;

		// No need for interrupt when transmission complete
		// cr1_temp.tcie_interrupt_enable = 1;
		break;
	}
	case (usart_mode_both):
	{
		cr1_temp.transmitter_enable	   = 1;
		cr1_temp.reciever_enable	   = 1;
		cr1_temp.rxne_interrupt_enable = 1;

		break;
	}
	}
	WRITE32(&usart->cr1, cr1_temp);
	WRITE32(&usart->cr2, cr2_temp);
	WRITE32(&usart->cr3, cr3_temp);
}

uint32_t get_system_tick();

/*
returns true if timed out
*/
bool usart_write_u8(volatile usart_t *const usart, uint8_t value)
{
	uint32_t system_tick = get_system_tick();
	while (!READ32(&usart->status).transmit_data_register_empty)
	{
		// Just poll till the data is empty and I can send it
		// What if there's data in the reader? (Heard its fine and not gonna happen)
		// For now, i'm leaving 2 scheduler tick, which is horrible.
		// But what if it schedules mid function.

		uint32_t new_st = get_system_tick();
		if (new_st - system_tick >= 1000000)
		{
			// With this check, if the scheduler is called twice while this runs
			// We leave early, something bad happened
			return true;
		}
	}
	FORCE_WRITE32(&usart->data, (uint32_t)value);
	return false;
}

int usart_write_n(volatile usart_t *const usart, const char *str, int len)
{

	uint32_t system_tick = get_system_tick();
	for (int i = 0; i < len; i++)
	{
		bool timed_out = usart_write_u8(usart, str[i]);
		if (timed_out || get_system_tick() - system_tick >= 10000)
		// The scheduler is very slow.
		// It is completely possible to stall due to writting.
		// Baud rate = 38kHz.
		// Scheduler = 10 Khz
		// 8 bit + start + stop = 10 bit ber byte.
		// 512 char = 512 * 10 message.
		// Tick count = 5120 * 10000/38400 = 1333.3-
		// With 1000 byte consecutive is even worse.
		// Hence I'm starting to see why one would want to use a DMA setup for printing
		// TODO: DMA usart printing
		{
			return i;
		}
	}
	return len;
}

int usart_write(volatile usart_t *const usart, const char *str)
{
	return usart_write_n(usart, str, strlen(str));
}

int usart2_write(const char *str, size_t n)
{
	return usart_write_n(usart2, str, n);
}

void Usart2HandlerC()
{
	usart_status_t status = READ32(&usart2->status);
	if (status.overrun_error)
	{
		return;
	}
	if (status.framing_error)
	{
		return;
	}
	if (status.parity_error)
	{
		return;
	}

	if (status.rxne)
	{
		usart_data_t data = READ32(&usart2->data);
		char		 c	  = data.data;
		putchar(c);

		// Enter is \r
		if (c == '\r')
		{
			puts("\nPressed enter\n");
		}
	}
}
