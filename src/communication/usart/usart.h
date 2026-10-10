#pragma once
#include "assert.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const uintptr_t USART2_BASE = 0x40013800; // The main one connected to my machine

static const uintptr_t USART1_BASE = 0x4001'1000;
static const uintptr_t USART6_BASE = 0x4001'1400;

// The peripheral registers have to be accessed by half-words (16 bits) or words (32 bits).

// Data Register

typedef struct
{
	uint32_t data : 9; // bit 0-8, (rw). Reading get from pc, writting send to pc. 9 bit long
	// If using parity, and Length = 9, the 9th bit is parity bit. Write to the parity bit are not cared about
	// Franck pro tip: Use length = 9 when parity, and l = 8 when no parity
	uint32_t _reserved : (32 - 9);

} usart_data_t;

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
