#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum usart_parity
{
	usart_parity_none = 0,
	usart_parity_odd  = 1,
	usart_parity_even = 2,
};

enum usart_mode
{
	usart_mode_both = 0,
	usart_mode_transmitter,
	usart_mode_reciever,
};

enum usart_baudrate
{
	usart_baudrate_2400	  = 2400,
	usart_baudrate_9600	  = 9600,
	usart_baudrate_38400  = 38400,
	usart_baudrate_57600  = 57600,
	usart_baudrate_115200 = 115200,
	usart_baudrate_230400 = 230400,

};

enum usart_word_length
{
	usart_word_lengths_8bits = 0b0,
	usart_wl_9				 = 0b1,
};

enum usart_hardware_control
{
	usart_hwc_none,
	usart_hwc_rts,
	usart_hwc_cts,
	usart_hwc_rts_cts,
};

enum usart_oversampling
{
	// Commonly called OVER8 (Which is ass backward!)
	usart_oversampling_16 = 0b0,
	usart_oversampling_8  = 0b1,
};

enum usart_stop_bits_pub
{
	usart_stop_bits_pub_1  = 0b00,
	usart_stop_bits_pub_05 = 0b01,
	usart_stop_bits_pub_2  = 0b10,

};

typedef struct
{
	enum usart_baudrate			baudrate;
	enum usart_word_length		word_length;
	enum usart_stop_bits_pub	stop_bit;
	enum usart_parity			parity;
	enum usart_mode				mode;
	enum usart_hardware_control hardware_control;
	enum usart_oversampling		oversampling_not_over8;

	struct clock_info
	{
		bool clock_enable;
		bool clock_polarity;
		bool clock_phase;
	} clock_info;
} usart_config_t;

typedef struct usart usart_t;

// usart.h

void config_usart(volatile usart_t *const usart, const usart_config_t *const config);
// bool usart_write_u8(volatile usart_t *const usart, uint8_t value);
int usart_write_n(volatile usart_t *const usart, char *str, int len);
int usart_write(volatile usart_t *const usart, char *str);
