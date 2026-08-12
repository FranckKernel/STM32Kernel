#pragma once
// #include "gpio.h"

// RTC : Reset and Clock Control
// RTC: Real Time Clock
// SW: System Clock switch
// Trapeze symbol: Multiplexer (Selector)
// AHB Presc: Advanced High Performance Bus Prescalor. A Prescaler divide a clock and outputs a slower version of it
// HCLK: AHB Clock. The clock that drives everything sitting on ahb bys. The core, memory interface, gpio and DMA.
// APB = Advanced Peripheral Bus. Simpler slower bus for peripherals (timers, uart, spi, i2c, etc).
//		| It's clock are PCLK1 (APB1), and PCLK2 APB2)
// 		| APB1 : Low speed, APB2 (Max 50Mhz): High speed (Max 100 Mhz)

// The sysclock can come from the hsi, the hse or the pllclk
// The pll clock can be sourced from the hse or the hsi

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// ========= HSI CLOCK (High Speed Internal Clock) ================
// ========= HSE CLOCK (High Speed External Clock) ================
// ========= Main PLL CLOCK (Phase-Locked Loop) ================

// =========  LSI (Low Speed Internal Clock) ==================
/* 32 KHz
   Drives the InDependant WatchDog Clock (IDWDG CLK)
   optionally, the the RTC used for auto-wakeup from the stop/standby mode
   RC : Resistor Capacitor is used to drive the clock.
*/

// =========  LSE (Low Speed External Clock) ==================
/* 32.768 KHz
   Otionally drives the RTC Clock (RTCCLK)
   Crystal: More accurate then RC type clock
*/

// =================================== PLL: ===============================
/*
   Right after the multiplexer, there's a divider by M. (kinda wierd). It chose M for alphabetic nearby-ness.  MN PQ R

   VCO: Voltage controlled Oscillator. The internal oscillator circuit that runs at a high frequency.
		| VCO is in the middle stage of the calculation
		f_VCO = f_input * N/M
   N : The feedback multiplier. Multiplies the clock frequency by N.

   PRQ. The final stage that goes into the output.

   PLL ClK = F_VCO / P   		| PLL CLK goes to SysClock
   PLL48CLK = F_VCO / Q			| Goes to USB, RNG and SDIO. Goal is to hit 48 MHz so usb works
   PLLR = F_VCO / R				| On other boards, connected to I2S, SAI or DSI Clocks

*/

// 1 ============================ RCC Clock Control register (RCC_CR)

typedef struct
{
	uint32_t raw;
} rcc_cr_t;

// 2 ============================ RCC PLL configuration register (RCC_PLLCFGR)

typedef struct
{
	uint32_t raw;
} rcc_pllcfgr_t;

// 3 ============================ RCC clock configuration register (RCC_CFGR)

typedef struct
{
	uint32_t raw;
} rcc_cfgr_t;

// 4 ============================ RCC clock interrupt register (RCC_CIR)

typedef struct
{
	uint32_t raw;
} rcc_cir_t;

// 5 ============================ RCC AHB1 peripheral reset register (RCC_AHB1RSTR)

typedef struct
{
	uint32_t raw;
} rcc_ahb1rstr_t;

// 6 ============================ RCC AHB2 peripheral reset register (RCC_AHB2RSTR)

typedef struct
{
	uint32_t raw;
} rcc_ahb2rstr_t;

// 7 ============================ RCC APB1 peripheral reset register (RCC_APB1RSTR)

typedef struct
{
	uint32_t raw;
} rcc_apb1rstr_t;

// 8 ============================ RCC APB2 peripheral reset register (RCC_APB2RSTR)

typedef struct
{
	uint32_t raw;
} rcc_apb2rstr_t;

// 9 ============================ RCC AHB1 peripheral clock enable register (RCC_AHB1ENR)

typedef struct
{
	uint32_t gpio_a_enable : 1; // bit 0
	uint32_t gpio_b_enable : 1; // bit 1
	uint32_t gpio_c_enable : 1; // bit 2
	uint32_t gpio_d_enable : 1; // bit 3
	uint32_t gpio_e_enable : 1; // bit 4

	uint32_t _reserved_1 : 2; // bit 5-6
	// would be gpio_f, and gpio_h

	uint32_t gpio_h_enable : 1; // bit 7

	uint32_t _reserved_2 : 4; // bit 8-11

	uint32_t crc_clock_enable : 1; // bit 12

	uint32_t _reserved_3 : 8;		// bit 13-20
	uint32_t dma1_clock_enable : 1; // bit 21
	uint32_t dma2_clock_enable : 1; // bit 22

	uint32_t _reserved_4 : 8; // bit 23-31

} rcc_ahb1enr_t;

// 10 ============================ RCC AHB2 peripheral clock enable register (RCC_AHB2ENR)

typedef struct
{
	uint32_t raw;
} rcc_ahb2enr_t;

// 11 ============================ RCC APB1 peripheral clock enable register (RCC_APB1ENR)

typedef struct
{
	uint32_t raw;
} rcc_apb1enr_t;

// 12 ============================ RCC APB2 peripheral clock enable register (RCC_APB2ENR)

typedef struct
{
	uint32_t raw;
} rcc_apb2enr_t;

// 13 ============================ RCC AHB1 peripheral clock enable in low power mode register (RCC_AHB1LPENR)

typedef struct
{
	uint32_t raw;
} rcc_ahb1lpenr_t;

// 14 ============================ RCC AHB2 peripheral clock enable in low power mode register (RCC_AHB2LPENR)

typedef struct
{
	uint32_t raw;
} rcc_ahb2lpenr_t;

// 15 ============================ RCC APB1 peripheral clock enable in low power mode register (RCC_APB1LPENR)

typedef struct
{
	uint32_t raw;
} rcc_apb1lpenr_t;

// 16 ============================ RCC APB2 peripheral clock enable in low power mode register (RCC_APB2LPENR)

typedef struct
{
	uint32_t raw;
} rcc_apb2lpenr_t;

// 17 ============================ RCC Backup domain control register (RCC_BDCR)

typedef struct
{
	uint32_t raw;
} rcc_bdcr_t;

// 18 ============================ RCC clock control & status register (RCC_CSR)

typedef struct
{
	uint32_t raw;
} rcc_csr_t;

// 19 ============================ RCC spread spectrum clock generation register (RCC_SSCGR)

typedef struct
{
	uint32_t raw;
} rcc_sscgr_t;

// 20 ============================ RCC PLLI2S configuration register (RCC_PLLI2SCFGR)

typedef struct
{
	uint32_t raw;
} rcc_plli2scfgr_t;

// 21 ============================ RCC Dedicated Clocks Configuration Register (RCC_DCKCFGR)

typedef struct
{
	uint32_t raw;
} rcc_dckcfgr_t;

// ================================================= The META Struct ===================

typedef struct
{
	rcc_cr_t	  cr;	   // 0x00
	rcc_pllcfgr_t pllcfgr; // 0x04
	rcc_cfgr_t	  cfgr;	   // 0x08
	rcc_cir_t	  cir;	   // 0x0C

	rcc_ahb1rstr_t ahb1rstr; // 0x10
	rcc_ahb2rstr_t ahb2rstr; // 0x14

	uint32_t _reserved_18; // 0x18
	uint32_t _reserved_1c; // 0x1C

	rcc_apb1rstr_t apb1rstr; // 0x20
	rcc_apb2rstr_t apb2rstr; // 0x24

	uint32_t _reserved_28; // 0x28
	uint32_t _reserved_2c; // 0x2C

	rcc_ahb1enr_t ahb1_enable_register; // 0x30
	rcc_ahb2enr_t ahb2enr;				// 0x34

	uint32_t _reserved_38; // 0x38
	uint32_t _reserved_3c; // 0x3C

	rcc_apb1enr_t apb1enr; // 0x40
	rcc_apb2enr_t apb2enr; // 0x44

	uint32_t _reserved_48; // 0x48
	uint32_t _reserved_4c; // 0x4C

	rcc_ahb1lpenr_t ahb1lpenr; // 0x50
	rcc_ahb2lpenr_t ahb2lpenr; // 0x54

	uint32_t _reserved_58; // 0x58
	uint32_t _reserved_5c; // 0x5C

	rcc_apb1lpenr_t apb1lpenr; // 0x60
	rcc_apb2lpenr_t apb2lpenr; // 0x64

	uint32_t _reserved_68; // 0x68
	uint32_t _reserved_6c; // 0x6C

	rcc_bdcr_t bdcr; // 0x70
	rcc_csr_t  csr;	 // 0x74

	uint32_t _reserved_78; // 0x78
	uint32_t _reserved_7c; // 0x7C

	rcc_sscgr_t		 sscgr;		 // 0x80
	rcc_plli2scfgr_t plli2scfgr; // 0x84

	uint32_t _reserved_88; // 0x88

	rcc_dckcfgr_t dckcfgr; // 0x8C

} rcc_register_t;

_Static_assert(sizeof(rcc_register_t) == 0x90, "rcc_register_t : We are fucked!");

static const uint32_t RCC_MMIO_ADDRESS_BASE = 0x40023800;

extern volatile rcc_register_t *const rcc;

#include "gpio_types.h"
void enable_gpio_clock(enum GPIO_PORT_LETTER letter);
