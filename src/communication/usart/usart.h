#pragma once
#include "assert.h"
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
// Usart Divider = DIV_Mantissa + (DIV_Fraction / 8 × (2 – OVER8))
typedef struct
{
	uint32_t div_fraction : 4;
	uint32_t div_mantissa : 12;
	// WE
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
enum usart_word_length
{
	usart_word_length_8bits = 0b0,
	usart_word_length_9bits = 0b1,

};

enum usart_oversampling
{
	// Commonly called OVER8
	usart_oversampling_16 = 0b0,
	usart_oversampling_8  = 0b1,
};

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

// ===================================== The META Struct
typedef struct
{
	uint32_t	 status;
	usart_data_t data;
	uint32_t	 brr;
	uint32_t	 cr1;
	uint32_t	 cr2;
	uint32_t	 cr3;
	uint32_t	 gtpr;
} usart_t;

extern volatile usart_t *const usart2; // In APB1
extern volatile usart_t *const usart1; // In AHB2
extern volatile usart_t *const usart6; // In AHB2

// =================================== The functions
