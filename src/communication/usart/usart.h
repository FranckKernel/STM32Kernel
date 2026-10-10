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
// DIV_Mantissa + (DIV_Fraction / 8 × (2 – OVER8))
typedef struct
{
	uint32_t div_fraction : 4;
	uint32_t div_mantissa : 12;
} usart_brr_t;

STATIC_ASSERT(sizeof(usart_brr_t) == sizeof(uint32_t), "brr must be size of 32 bit");

// =================================== USART Status Register (SR)
typedef struct
{
	uint32_t parrity_error : 1;
	uint32_t framing_error : 1;
	uint32_t noise_detected_flag : 1;

} usart_status_t;

// ================================== USART Control Register 1 (CR1)
typedef struct
{
	uint32_t send_break : 1;
	uint32_t recieved_wakeup : 1;
	uint32_t reciever_enable : 1;
	uint32_t transmitter_enable : 1;
	uint32_t idle_interrupt_enable : 1;
	uint32_t rxne_interrupt_enable : 1; // recieve data register not empty.
	uint32_t tcie_interrupt_enable : 1; // Transmission complete interrupt enable
	uint32_t txe_interrupt_enable : 1;	// Transmitter data register empty

} usart_cr1_t;

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
