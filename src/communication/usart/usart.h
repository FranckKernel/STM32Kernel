#pragma once
#include "assert.h"
#include "usart_public.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const uintptr_t USART2_BASE = 0x40013800; // The main one connected to my machine
// APB1

static const uintptr_t USART1_BASE = 0x4001'1000;
static const uintptr_t USART6_BASE = 0x4001'1400;
// AHB2

// The peripheral registers have to be accessed by half-words (16 bits) or words (32 bits).

// ================================ USART Data Register (DR)

typedef struct
{
	uint32_t data : 9; // bit 0-8, (rw). Reading get from pc, writting send to pc. 9 bit long
	// If using parity, and Length = 9, the 9th bit is parity bit. Write to the parity bit are not cared about
	// Franck pro tip: Use length = 9 when parity, and l = 8 when no parity
	uint32_t _reserved : (32 - 9);

} usart_data_t;

// ================================== USART Baud Rate Register (BRR)
// Usart Divider = DIV_Mantissa +  { DIV_Fraction / [8 × (2 – OVER8)] }
// With 96Mhz, we need Divider = 2500
typedef struct
{
	uint32_t div_fraction : 4;
	uint32_t div_mantissa : 12;
	// It's fixed signed representation. mantissa = whole number
	// fraction = bits of minus index. (2^-1, 2^-2, 2^-3, 2^-4)
} usart_brr_t;

STATIC_ASSERT(sizeof(usart_brr_t) == sizeof(uint32_t), "brr must be size of 32 bit");

// =================================== USART Status Register (SR)
typedef struct
{
	uint32_t parity_error : 1;	// Set by hardware. Cleared by software by r/w DR after RXNE set. (need to poll rxne, or interrupt rxne)
	uint32_t framing_error : 1; // r only
	uint32_t noise_detected_flag : 1; // r only
	uint32_t overrun_error : 1;		  // r only
	uint32_t idle_detected : 1;		  // r only

	uint32_t rxne : 1; // read data register not empty
	uint32_t transmission_complete : 1;
	// A frame is one complete unit of serial transmission. (start, 8 bits of data, 1 bit parity (optional), stop)
	uint32_t transmit_data_register_empty : 1;

	uint32_t lin_break_detected : 1;
	uint32_t cts_flag : 1; // cts = clear to send. Used in Hardware Control Flow

} usart_status_t;

// ================================== USART Control Register 1 (CR1)

typedef struct
{
	uint32_t send_break : 1;			// bit 0
	uint32_t recieved_wakeup : 1;		// bit 1
	uint32_t reciever_enable : 1;		// bit 2
	uint32_t transmitter_enable : 1;	// bit 3
	uint32_t idle_interrupt_enable : 1; // bit 4
	uint32_t rxne_interrupt_enable : 1; // bit 5
	// recieve data register not empty interrupt enable. (Get an interrupt every time you receieve data)
	uint32_t tcie_interrupt_enable : 1;			// bit 6  Transmission complete interrupt enable
	uint32_t txe_interrupt_enable : 1;			// bit 7  Transmitter data register empty interrupt enable
	uint32_t parity_error_interrupt_enable : 1; // bit 8

	uint32_t parity_selection_odd_not_even : 1; // bit 9

	uint32_t parity_control_enable : 1; // To activate or not the parity
	uint32_t wakeup_method : 1;

	enum usart_word_length word_length : 1;

	uint32_t				usart_enable : 1;
	uint32_t				_reserved_bit14 : 1;
	enum usart_oversampling oversampling : 1; // (RG note) : called over8 refers to. over8 is the oversampling

} usart_cr1_t;
STATIC_ASSERT(sizeof(usart_cr1_t) == sizeof(uint32_t), "USART CR1 must be 32 bit long!");

// ================================== USART Control Register 2 (CR2)

enum usart_stop_bits
{
	usart_stop_bits_1  = 0b00,
	usart_stop_bits_05 = 0b01,
	usart_stop_bits_2  = 0b10,

};

typedef struct
{
	uint32_t address : 3;			   // bit 0-3
	uint32_t _reserved1 : 1;		   // bit 4
	uint32_t line_break_detection : 1; // bit 5 (10 bit break = 0, 11 bit break = 1)
	uint32_t lbd_interrupt_enable : 1; // bit 6
	uint32_t _reserved2 : 1;		   // bit 7
	uint32_t last_bit_clock_pulse : 1; // bit 8 (Pretty much irrelevant)

	uint32_t clock_phase : 1; // can leave these two default
	uint32_t clock_polarity : 1;

	uint32_t			 clock_enable : 1; // Makes it usart, otherwise uart
	enum usart_stop_bits stop_bits : 2;
	uint32_t			 lin_mode_enable : 1;

} usart_cr2_t;

STATIC_ASSERT(sizeof(usart_cr2_t) == sizeof(uint32_t), "USART CR2 must be 32 bit long!");

// ================================== USART Control Register 3 (CR3)

typedef struct
{
	uint32_t error_interrupt_enable : 1;
	uint32_t irda_mode_enable : 1;
	uint32_t irda_low_power : 1;
	uint32_t half_duplex_selected : 1;
	uint32_t smart_card_nack_enable : 1;
	uint32_t smart_card_mode_enable : 1;
	uint32_t dma_enable_reciever : 1;
	uint32_t dma_enable_transmitter : 1;
	uint32_t rts_hardware_control_flow_enabled : 1;
	uint32_t cts_hardware_control_flow_enabled : 1;
	uint32_t cts_interrupt_enable : 1;
	uint32_t onebit : 1;

} usart_cr3_t;

STATIC_ASSERT(sizeof(usart_cr3_t) == sizeof(uint32_t), "USART CR3 must be 32 bit long!");

// ===================================== The META Struct
typedef struct usart
{
	usart_status_t status;
	usart_data_t   data;
	usart_brr_t	   brr;
	usart_cr1_t	   cr1;
	usart_cr2_t	   cr2;
	usart_cr3_t	   cr3;
	uint32_t	   gtpr;
} usart_t;

// =================================== The functions
